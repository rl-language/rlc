/*
Copyright 2024 Massimo Fioravanti

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	 http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/
#include "rlc/lsp/LSP.hpp"

#include "llvm/Support/Error.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Path.h"
#include "mlir/Pass/PassManager.h"
#include "rlc/dialect/Operations.hpp"
#include "rlc/dialect/Passes.hpp"
#include "rlc/dialect/Types.hpp"
#include "rlc/parser/MultiFileParser.hpp"
#include "rlc/utils/Error.hpp"

using namespace mlir::rlc::lsp;

static mlir::lsp::Position locToPos(mlir::Location location)
{
	auto castedBegin = location.cast<mlir::FileLineColLoc>();
	return mlir::lsp::Position(
			static_cast<int>(castedBegin.getLine()) - 1,
			static_cast<int>(castedBegin.getColumn()) - 1);
}

static llvm::Expected<mlir::lsp::Location> locToLoc(mlir::Location location)
{
	auto castedBegin = location.cast<mlir::FileLineColLoc>();
	llvm::errs() << castedBegin.getFilename().str();
	auto uri = mlir::lsp::URIForFile::fromFile(castedBegin.getFilename().str());
	if (not uri)
	{
		llvm::cantFail(uri.takeError());
	}

	return mlir::lsp::Location(
			*uri, mlir::lsp::Range(locToPos(location), locToPos(location)));
}

static mlir::lsp::Range locsToRange(mlir::Location begin, mlir::Location end)
{
	auto toReturn = mlir::lsp::Range(locToPos(begin), locToPos(end));
	return toReturn;
}

static mlir::lsp::Diagnostic rlcDiagToLSPDiag(
		const mlir::rlc::lsp::Diagnostic &diag)
{
	mlir::lsp::Diagnostic toReturn;

	toReturn.message = diag.text;
	if (diag.severity == mlir::DiagnosticSeverity::Error)
		toReturn.severity = mlir::lsp::DiagnosticSeverity::Error;
	else if (diag.severity == mlir::DiagnosticSeverity::Warning)
		toReturn.severity = mlir::lsp::DiagnosticSeverity::Warning;
	else if (diag.severity == mlir::DiagnosticSeverity::Note)
		toReturn.severity = mlir::lsp::DiagnosticSeverity::Information;
	else if (diag.severity == mlir::DiagnosticSeverity::Remark)
		toReturn.severity = mlir::lsp::DiagnosticSeverity::Hint;

	auto endPos = locToPos(diag.location);
	endPos.character = endPos.character + 1;
	toReturn.range = mlir::lsp::Range(locToPos(diag.location), endPos);

	return toReturn;
}

static mlir::rlc::ClassDeclaration getClassDefinition(
		mlir::ModuleOp module, mlir::lsp::Position pos)
{
	for (auto decl : module.getOps<mlir::rlc::ClassDeclaration>())
	{
		if (not decl.getTypeLocation().has_value())

			continue;
		auto loc = *decl.getTypeLocation();
		if (locsToRange(loc.getStart(), loc.getEnd()).contains(pos))
			return decl;
	}
	return nullptr;
}

class mlir::rlc::lsp::LSPModuleInfoImpl
{
	public:
	~LSPModuleInfoImpl() { module->erase(); }
	explicit LSPModuleInfoImpl(
			llvm::StringRef path, llvm::StringRef contents, LSPContext &lspContext)
			: context(mlir::MLIRContext::Threading::DISABLED),
				diagnosticHandler(
						&context,
						[this](mlir::Diagnostic &diagnostic) {
							std::string text;
							llvm::raw_string_ostream stream(text);
							diagnostic.print(stream);
							stream.flush();
							diagnostics.emplace_back(mlir::rlc::lsp::Diagnostic{
									text, diagnostic.getLocation(), diagnostic.getSeverity() });
						}),
				module(mlir::ModuleOp::create(
						mlir::FileLineColLoc::get(&context, path, 0, 0), path)),
				currentFileContent(contents.str()),
				lspContext(&lspContext)
	{
		Registry.insert<mlir::BuiltinDialect, mlir::rlc::RLCDialect>();
		context.appendDialectRegistry(Registry);
		context.loadAllAvailableDialects();
		loadFile(path, contents, lspContext);
		module.walk([this](mlir::rlc::DeclarationStatement decl) {
			auto yieldLoc = decl.getBody()
													.back()
													.getTerminator()
													->getLoc()
													.cast<mlir::FileLineColLoc>();
			auto loc = decl.getLoc().cast<mlir::FileLineColLoc>();
			declarations.emplace_back(
					locsToRange(loc, yieldLoc), decl.getOperation());
		});
		module.walk([this](mlir::rlc::FunctionOp decl) {
			if (decl.getBody().empty())
				return;
			auto firstInstructionLoc =
					decl.getBody().front().front().getLoc().cast<mlir::FileLineColLoc>();
			auto loc = decl.getLoc().cast<mlir::FileLineColLoc>();
			declarations.emplace_back(
					locsToRange(loc, firstInstructionLoc), decl.getOperation());
			auto yieldLoc =
					decl.getBody().front().back().getLoc().cast<mlir::FileLineColLoc>();
			functionAndActionFunctions.emplace_back(
					locsToRange(loc, yieldLoc), decl.getOperation());
			// increment the range by one so that if the user
			// writes the first empty line after the function it thinks
			// it belongs to the function
			functionAndActionFunctions.back().first.end.line++;
		});
		module.walk([this](mlir::rlc::ActionFunction decl) {
			if (decl.getBody().empty())
				return;
			auto firstInstructionLoc =
					decl.getBody().front().front().getLoc().cast<mlir::FileLineColLoc>();
			auto loc = decl.getLoc().cast<mlir::FileLineColLoc>();
			declarations.emplace_back(
					locsToRange(loc, firstInstructionLoc), decl.getOperation());
			auto yieldLoc =
					decl.getBody().front().back().getLoc().cast<mlir::FileLineColLoc>();
			functionAndActionFunctions.emplace_back(
					locsToRange(loc, yieldLoc), decl.getOperation());
		});
		module.walk([this](mlir::rlc::ActionStatement decl) {
			if (decl->getNextNode() == nullptr)
				return;
			auto firstInstructionLoc =
					decl->getNextNode()->getLoc().cast<mlir::FileLineColLoc>();
			auto loc = decl.getLoc().cast<mlir::FileLineColLoc>();
			declarations.emplace_back(
					locsToRange(loc, firstInstructionLoc), decl.getOperation());
		});
		module.walk([this](mlir::rlc::CallOp decl) {
			if (decl->getNextNode() == nullptr)
				return;
			auto nextInst =
					decl->getNextNode()->getLoc().cast<mlir::FileLineColLoc>();
			auto loc = decl.getLoc().cast<mlir::FileLineColLoc>();
			declarations.emplace_back(
					locsToRange(loc, nextInst), decl.getOperation());
		});

		module.walk([this](mlir::rlc::UnresolvedMemberAccess decl) {
			if (decl->getNextNode() == nullptr)
				return;
			auto nextInst =
					decl->getNextNode()->getLoc().cast<mlir::FileLineColLoc>();
			auto loc = decl.getLoc().cast<mlir::FileLineColLoc>();
			memberAcceses.emplace_back(
					locsToRange(loc, nextInst), decl.getOperation());
			memberAcceses.back().first.end.character++;
		});

		module.walk([this](mlir::rlc::MemberAccess decl) {
			if (decl->getNextNode() == nullptr)
				return;
			auto nextInst =
					decl->getNextNode()->getLoc().cast<mlir::FileLineColLoc>();
			auto loc = decl.getLoc().cast<mlir::FileLineColLoc>();
			memberAcceses.emplace_back(
					locsToRange(loc, nextInst), decl.getOperation());
			memberAcceses.back().first.end.character++;
		});
	}

	mlir::ModuleOp getModule() const { return module; }

	// zero based positions
	mlir::Operation *getOperation(const mlir::lsp::Position &pos)
	{
		for (const auto &pair :
				 llvm::make_range(declarations.rbegin(), declarations.rend()))
		{
			if (pair.first.contains(pos) and opIsInSameFile(pair.second))
				return pair.second;
		}
		return nullptr;
	}

	mlir::Operation *getNearestMemberAccess(const mlir::lsp::Position &pos)
	{
		for (const auto &pair :
				 llvm::make_range(memberAcceses.rbegin(), memberAcceses.rend()))
		{
			if (pair.first.contains(pos) and opIsInSameFile(pair.second))
				return pair.second;
		}
		return nullptr;
	}

	bool opIsInSameFile(mlir::Operation *op)
	{
		return op->getLoc().cast<mlir::FileLineColLoc>().getFilename() ==
					 this->module->getLoc().cast<mlir::FileLineColLoc>().getFilename();
	}

	mlir::Operation *getEnclosingFunction(const mlir::lsp::Position &pos)
	{
		for (const auto &pair : llvm::make_range(
						 functionAndActionFunctions.rbegin(),
						 functionAndActionFunctions.rend()))
		{
			if (pair.first.contains(pos) and opIsInSameFile(pair.second))
				return pair.second;
		}
		return nullptr;
	}

	mlir::LogicalResult getCompleteFunction(
			const mlir::lsp::Position &completePos, mlir::lsp::CompletionList &list)
	{
		auto *fun = getEnclosingFunction(completePos);
		if (fun == nullptr)
		{
			return mlir::failure();
		}
		using KeyType = std::pair<std::string, const void *>;
		std::set<KeyType> alreadyEmitted;
		auto registerArgument = [&](llvm::StringRef name,
																mlir::Type t,
																mlir::rlc::FunctionInfoAttr info) {
			KeyType key(name.str(), t.getAsOpaquePointer());
			if (alreadyEmitted.contains(key))
				return;

			alreadyEmitted.insert(key);
			mlir::lsp::CompletionItem item;
			item.label = name.str();
			item.kind = mlir::lsp::CompletionItemKind::Variable;
			item.insertTextFormat = mlir::lsp::InsertTextFormat::PlainText;
			if (auto casted = t.dyn_cast<mlir::FunctionType>();
					casted and info != nullptr)
			{
				item.detail = prettyPrintFunctionTypeWithNameArgs(casted, info);
			}
			else
				item.detail = prettyType(t);
			list.items.push_back(item);
		};
		if (auto casted = mlir::dyn_cast<mlir::rlc::FunctionOp>(fun);
				casted and not casted.getBody().empty())
		{
			for (auto arg :
					 llvm::zip(casted.getArgNames(), casted.getType().getInputs()))
			{
				registerArgument(std::get<0>(arg), std::get<1>(arg), nullptr);
			}
		}

		if (auto casted = mlir::dyn_cast<mlir::rlc::ActionFunction>(fun);
				casted and not casted.getBody().empty())
		{
			for (auto arg :
					 llvm::zip(casted.getArgNames(), casted.getType().getInputs()))
			{
				registerArgument(std::get<0>(arg), std::get<1>(arg), nullptr);
			}
		}

		fun->walk([&](mlir::rlc::DeclarationStatement statement) {
			registerArgument(statement.getSymName(), statement.getType(), nullptr);
		});

		for (auto op : module.getOps<mlir::rlc::ActionFunction>())
			registerArgument(op.getUnmangledName(), op.getType(), nullptr);

		for (auto op : module.getOps<mlir::rlc::FunctionOp>())
			if (not op.getIsMemberFunction())
				registerArgument(op.getUnmangledName(), op.getType(), op.getInfo());

		return mlir::success();
	}

	void getTypes(mlir::lsp::CompletionList &list)
	{
		for (auto name : { "Int", "Byte", "Float", "Bool" })
		{
			mlir::lsp::CompletionItem toReturn;
			toReturn.kind = mlir::lsp::CompletionItemKind::TypeParameter;
			toReturn.label = name;
			list.items.push_back(toReturn);
		}

		module.walk([&](mlir::rlc::ClassDeclaration op) {
			mlir::lsp::CompletionItem toReturn;
			toReturn.kind = mlir::lsp::CompletionItemKind::TypeParameter;
			toReturn.label = op.getName().str();
			toReturn.detail = "<";
			if (op.getTemplateParameters().size() > 1)
				for (auto parameter : llvm::drop_end(op.getTemplateParameters()))
					toReturn.detail +=
							prettyType(parameter.cast<mlir::TypeAttr>().getValue()) + ", ";
			auto params = op.getTemplateParameters();
			if (not params.empty())
				toReturn.detail += prettyType(
						params[params.size() - 1].cast<mlir::TypeAttr>().getValue());
			toReturn.detail += ">";
			list.items.push_back(toReturn);
		});

		module.walk([&](mlir::rlc::TypeAliasOp op) {
			mlir::lsp::CompletionItem toReturn;
			toReturn.kind = mlir::lsp::CompletionItemKind::TypeParameter;
			toReturn.label = op.getName();
			list.items.push_back(toReturn);
		});
	}

	bool sameLineAsOp(const mlir::lsp::Position &completePos, mlir::Operation *op)
	{
		auto opLoc = op->getLoc().cast<mlir::FileLineColLoc>();
		auto moduleLoc = module.getLoc().cast<mlir::FileLineColLoc>();
		if (static_cast<int>(opLoc.getLine()) != completePos.line + 1)
			return false;
		if (opLoc.getFilename() != moduleLoc.getFilename())
			return false;
		return true;
	}

	llvm::StringRef lineOfPosition(const mlir::lsp::Position &completePos) const
	{
		auto line = completePos.line;
		size_t current_pos = 0;
		llvm::StringRef currentLine = currentFileContent;
		while (line != 0 and current_pos != llvm::StringRef::npos)
		{
			current_pos = currentLine.find('\n', current_pos + 1);
			line--;
		}
		if (current_pos == llvm::StringRef::npos)
			return "";

		size_t end = currentLine.find('\n', current_pos + 1);
		if (end == llvm::StringRef::npos)
			end = currentLine.end() - currentLine.begin();
		return currentLine.substr(current_pos, end - current_pos);
	}

	mlir::LogicalResult getCompleteType(
			const mlir::lsp::Position &completePos, mlir::lsp::CompletionList &list)
	{
		mlir::Operation *construct = nullptr;

		module.walk([&](mlir::Operation *current) {
			if (auto op = llvm::dyn_cast<mlir::rlc::DeclarationStatement>(current))
				if (sameLineAsOp(completePos, op) and
						not lineOfPosition(completePos).contains(" ="))
				{
					construct = op;
					return mlir::WalkResult::interrupt();
				}

			if (auto op = llvm::dyn_cast<mlir::rlc::FunctionOp>(current))
				if (sameLineAsOp(completePos, op))
				{
					construct = op;
					return mlir::WalkResult::interrupt();
				}

			if (auto op = llvm::dyn_cast<mlir::rlc::ActionFunction>(current))
				if (sameLineAsOp(completePos, op))
				{
					construct = op;
					return mlir::WalkResult::interrupt();
				}

			if (auto op = llvm::dyn_cast<mlir::rlc::ActionStatement>(current))
				if (sameLineAsOp(completePos, op))
				{
					construct = op;
					return mlir::WalkResult::interrupt();
				}

			if (auto op = llvm::dyn_cast<mlir::rlc::TypeAliasOp>(current))
				if (sameLineAsOp(completePos, op))
				{
					construct = op;
					return mlir::WalkResult::interrupt();
				}

			return mlir::WalkResult::advance();
		});

		if (construct == nullptr)
			return mlir::failure();
		getTypes(list);

		return mlir::success();
	}

	mlir::LogicalResult getCompleteImport(
			const mlir::lsp::Position &completePos, mlir::lsp::CompletionList &list)
	{
		llvm::StringRef strRef(currentFileContent);
		auto copy = completePos;
		int i = 0;
		while (copy.line != 0)
		{
			while (strRef[i] != '\n' and strRef[i] != '\0')
				i++;
			if (strRef[i] != '\0')
				i++;
			copy.line -= 1;
		}

		strRef = strRef.drop_front(i);
		if (!strRef.starts_with("import "))
			return mlir::failure();

		llvm::StringRef line(strRef.begin(), copy.character);
		line.consume_front("import ");
		auto lastDot = line.rfind('.');
		if (lastDot == llvm::StringRef::npos)
			line = "";
		else
			line = line.drop_back(line.size() - lastDot);
		auto path = line.str();
		for (auto &c : path)
			if (c == '.')
				c = '/';

		for (auto &dir : lspContext->getSourceManager().getIncludeDirs())
		{
			auto fullPath = dir + "/" + path;
			llvm::SmallVector<char, 16> nativePath;
			llvm::sys::path::native(fullPath, nativePath);

			if (not llvm::sys::fs::exists(nativePath) or
					not llvm::sys::fs::is_directory(nativePath))
				continue;
			std::error_code ec;

			llvm::sys::fs::directory_iterator iterator(nativePath, ec);
			if (ec)
				continue;

			while (iterator != llvm::sys::fs::directory_iterator())
			{
				auto &entry = iterator->path();

				mlir::lsp::CompletionItem toReturn;
				toReturn.kind = mlir::lsp::CompletionItemKind::Module;
				if (entry.ends_with(".rl"))
					toReturn.label = llvm::sys::path::filename(entry).drop_back(3);
				else
					toReturn.label = llvm::sys::path::filename(entry);
				list.items.push_back(toReturn);
				iterator.increment(ec);
				if (ec)
					break;
			}
		}

		return mlir::success();
	}

	mlir::LogicalResult getCompleteAccessMember(
			const mlir::lsp::Position &completePos, mlir::lsp::CompletionList &list)
	{
		auto *memberAccess = getNearestMemberAccess(completePos);
		if (memberAccess == nullptr)
		{
			return mlir::failure();
		}

		auto type = memberAccess->getOperand(0).getType();
		if (auto casted = type.dyn_cast<mlir::rlc::ClassType>())
		{
			for (auto field : casted.getMembers())
			{
				mlir::lsp::CompletionItem item;
				item.label = field.getName();
				item.kind = mlir::lsp::CompletionItemKind::Field;
				item.insertTextFormat = mlir::lsp::InsertTextFormat::PlainText;
				item.detail = prettyType(field.getType());
				list.items.push_back(item);
			}
		}

		mlir::rlc::ValueTable table;
		mlir::rlc::OverloadResolver resolver(table);

		for (auto fun : module.getOps<mlir::rlc::FunctionOp>())
		{
			if (fun.getType().getNumInputs() == 0)
				continue;

			llvm::DenseMap<mlir::rlc::TemplateParameterType, mlir::Type>
					substitutions;
			if (resolver
							.deduceSubstitutions(
									memberAccess->getLoc(),
									substitutions,
									fun.getType().getInput(0),
									type)
							.succeeded() and
					fun.getIsMemberFunction())
			{
				mlir::lsp::CompletionItem item;
				item.label = fun.getUnmangledName();
				item.kind = mlir::lsp::CompletionItemKind::Function;
				item.insertTextFormat = mlir::lsp::InsertTextFormat::PlainText;
				item.detail =
						prettyPrintFunctionTypeWithNameArgs(fun.getType(), fun.getInfo());
				list.items.push_back(item);
			}
		}

		for (auto fun : module.getOps<mlir::rlc::ActionFunction>())
		{
			if (fun.getMainActionType().getResult(0) != type)
				continue;

			using KeyType = std::pair<std::string, const void *>;
			std::set<KeyType> alreadyEmitted;
			fun.walk([&](mlir::rlc::ActionStatement statemet) {
				auto fType = mlir::FunctionType::get(
						fun.getContext(), statemet.getResultTypes(), {});
				KeyType key(statemet.getName().str(), fType.getAsOpaquePointer());
				if (alreadyEmitted.contains(key))
					return;

				alreadyEmitted.insert(key);
				mlir::lsp::CompletionItem item;
				item.label = statemet.getName();
				item.kind = mlir::lsp::CompletionItemKind::Function;
				item.insertTextFormat = mlir::lsp::InsertTextFormat::PlainText;
				item.detail =
						prettyPrintFunctionTypeWithNameArgs(fType, statemet.getInfo());
				list.items.push_back(item);
			});

			mlir::lsp::CompletionItem item;
			item.label = "is_done()";
			item.kind = mlir::lsp::CompletionItemKind::Function;
			item.insertTextFormat = mlir::lsp::InsertTextFormat::PlainText;
			item.detail = prettyType(fun.getIsDoneFunctionType());
			list.items.push_back(item);
		}
		return mlir::success();
	}

	void getLocationsOf(
			const mlir::lsp::Position &defPos,
			std::vector<mlir::lsp::Location> &locations)
	{
		auto *nearestDecl = getOperation(defPos);
		if (nearestDecl == nullptr)
			return;

		auto casted = mlir::dyn_cast<mlir::rlc::CallOp>(nearestDecl);
		if (not casted)
			return;

		auto templateInst =
				mlir::dyn_cast<mlir::rlc::TemplateInstantiationOp>(nearestDecl);
		if (templateInst)
		{
			auto maybeLoc = locToLoc(templateInst.getInputTemplate().getLoc());
			if (maybeLoc)
				locations.push_back(*maybeLoc);
			else
				llvm::consumeError(maybeLoc.takeError());
			return;
		};
		auto maybeLoc = locToLoc(casted.getCallee().getLoc());
		if (maybeLoc)
			locations.push_back(*maybeLoc);
		else
			llvm::consumeError(maybeLoc.takeError());
	}

	void findReferencesOf(
			const mlir::lsp::Position &pos,
			std::vector<mlir::lsp::Location> &references)
	{
		auto *nearestDecl = getOperation(pos);
		if (nearestDecl == nullptr)
			return;
		for (auto result : nearestDecl->getResults())
		{
			for (const auto &use : result.getUsers())
			{
				auto maybeLoc = locToLoc(use->getLoc());
				if (maybeLoc)
					references.push_back(*maybeLoc);
				else
					llvm::consumeError(maybeLoc.takeError());
			}
		}
	}

	void loadFile(
			llvm::StringRef path, llvm::StringRef contents, LSPContext &lspContext)
	{
		llvm::SmallVector<std::string, 4> includes(lspContext.getIncludePaths());
		auto directory = llvm::sys::path::parent_path(path);
		includes.push_back(directory.str());
		::rlc::MultiFileParser parser(
				&context, includes, &lspContext.getSourceManager(), module);
		auto res = parser.parseFromBuffer(contents, path);

		if (!res)
		{
			auto error = llvm::handleErrors(
					res.takeError(),
					[&](const llvm::StringError &error) {
						diagnostics.emplace_back(
								error.getMessage(),
								mlir::FileLineColLoc::get(&context, "-", 1, 1),
								mlir::DiagnosticSeverity::Error);
					},
					[&](const ::rlc::RlcError &error) {
						diagnostics.emplace_back(
								error.getText(),
								error.getPosition(),
								mlir::DiagnosticSeverity::Error);
					});
			if (error)
			{
				diagnostics.emplace_back(
						"lsp: unkown llvm error",
						mlir::FileLineColLoc::get(&context, "-", 1, 1),
						mlir::DiagnosticSeverity::Error);
				llvm::consumeError(std::move(error));
			}
		}

		mlir::PassManager manager(&context);
		manager.addPass(mlir::rlc::createEmitEnumEntitiesPass());
		manager.addPass(mlir::rlc::createMemberFunctionsToRegularFunctionsPass());
		manager.addPass(mlir::rlc::createTypeCheckEntitiesPass());
		manager.addPass(mlir::rlc::createTypeCheckPass());
		manager.addPass(mlir::rlc::createValidateStorageQualifiersPass());
		auto typeCheckResult = manager.run(module);
	}

	llvm::ArrayRef<mlir::rlc::lsp::Diagnostic> getDiagnostics() const
	{
		return diagnostics;
	}

	void clearDiagnostics() { diagnostics.clear(); }

	private:
	llvm::SmallVector<std::pair<mlir::lsp::Range, mlir::Operation *>>
			declarations;

	llvm::SmallVector<std::pair<mlir::lsp::Range, mlir::Operation *>>
			memberAcceses;

	llvm::SmallVector<std::pair<mlir::lsp::Range, mlir::Operation *>>
			functionAndActionFunctions;

	llvm::SmallVector<mlir::rlc::lsp::Diagnostic> diagnostics;
	mlir::DialectRegistry Registry;
	mlir::MLIRContext context;
	mlir::ScopedDiagnosticHandler diagnosticHandler;
	mlir::ModuleOp module;
	std::string currentFileContent;
	LSPContext *lspContext;
};

LSPModuleInfo::LSPModuleInfo(
		llvm::StringRef path,
		llvm::StringRef content,
		int64_t version,
		LSPContext &context)
		: impl(new LSPModuleInfoImpl(path, content, context)), version(version)
{
}

mlir::LogicalResult LSPModuleInfo::getCompleteFunction(
		const mlir::lsp::Position &completePos,
		mlir::lsp::CompletionList &list) const
{
	return impl->getCompleteFunction(completePos, list);
}

void LSPModuleInfo::getTypes(mlir::lsp::CompletionList &list) const
{
	impl->getTypes(list);
}

mlir::LogicalResult LSPModuleInfo::getCompleteAccessMember(
		const mlir::lsp::Position &completePos,
		mlir::lsp::CompletionList &list) const
{
	return impl->getCompleteAccessMember(completePos, list);
}

mlir::LogicalResult LSPModuleInfo::getCompleteType(
		const mlir::lsp::Position &completePos,
		mlir::lsp::CompletionList &list) const
{
	return impl->getCompleteType(completePos, list);
}

mlir::LogicalResult LSPModuleInfo::getCompleteImport(
		const mlir::lsp::Position &completePos,
		mlir::lsp::CompletionList &list) const
{
	return impl->getCompleteImport(completePos, list);
}

mlir::ModuleOp LSPModuleInfo::getModule() const { return impl->getModule(); }

void LSPModuleInfo::getLocationsOf(
		const mlir::lsp::Position &defPos,
		std::vector<mlir::lsp::Location> &locations) const
{
	impl->getLocationsOf(defPos, locations);
}

void LSPModuleInfo::findReferencesOf(
		const mlir::lsp::Position &pos,
		std::vector<mlir::lsp::Location> &references) const
{
	impl->findReferencesOf(pos, references);
}

llvm::ArrayRef<mlir::rlc::lsp::Diagnostic> LSPModuleInfo::getDiagnostics() const
{
	return impl->getDiagnostics();
}

void LSPModuleInfo::clearDiagnostics() { impl->clearDiagnostics(); }

LSPModuleInfo &LSPModuleInfo::operator=(LSPModuleInfo &&other)
{
	if (this == &other)
		return *this;

	delete impl;
	impl = other.impl;
	other.impl = nullptr;
	version = other.version;
	return *this;
}

LSPModuleInfo::~LSPModuleInfo() { delete impl; }

const LSPModuleInfo *RLCServer::getModuleFromUri(
		const mlir::lsp::URIForFile &uri)
{
	if (not fileToModule.contains(uri.uri()))
	{
		return nullptr;
	}
	return &fileToModule.at(uri.uri());
}

mlir::Operation *LSPModuleInfo::getOperation(
		const mlir::lsp::Position &pos) const
{
	return impl->getOperation(pos);
}

static std::string hoverText(mlir::Operation *op)
{
	if (auto casted = llvm::dyn_cast<mlir::rlc::DeclarationStatement>(op))
	{
		return (casted.getSymName() + ": " +
						mlir::rlc::prettyType(casted.getType()))
				.str();
	}

	if (auto casted = llvm::dyn_cast<mlir::rlc::FunctionOp>(op))
	{
		return (casted.getUnmangledName() + ": " +
						mlir::rlc::prettyType(casted.getType()))
				.str();
	}

	if (auto casted = llvm::dyn_cast<mlir::rlc::ActionFunction>(op))
	{
		return (casted.getUnmangledName() + ": " +
						mlir::rlc::prettyType(casted.getType()))
				.str();
	}

	if (auto casted = llvm::dyn_cast<mlir::rlc::ActionStatement>(op))
	{
		return casted.getName().str();
	}
	if (auto casted = llvm::dyn_cast<mlir::rlc::CallOp>(op))
	{
		return hoverText(casted.getCallee().getDefiningOp());
	}
	if (auto casted = llvm::dyn_cast<mlir::rlc::TemplateInstantiationOp>(op))
	{
		return hoverText(casted.getInputTemplate().getDefiningOp());
	}

	return "lsp error: no known hover information for: " +
				 op->getName().getIdentifier().str();
}

std::optional<mlir::lsp::Hover> RLCServer::findHover(
		const mlir::lsp::URIForFile &uri, const mlir::lsp::Position &hoverPos)
{
	const auto *maybeInfo = getModuleFromUri(uri);
	auto toReturn = mlir::lsp::Hover(
			mlir::lsp::Range(mlir::lsp::Position(0, 0), mlir::lsp::Position(0, 0)));
	toReturn.contents.kind = mlir::lsp::MarkupKind::PlainText;
	if (maybeInfo == nullptr)
	{
		toReturn.contents.value = "no file!!!!";
		return toReturn;
	}

	mlir::Operation *op = maybeInfo->getOperation(hoverPos);

	if (op == nullptr)
		return std::nullopt;

	toReturn.contents.value = hoverText(op);
	return toReturn;
}

void RLCServer::findDocumentSymbols(
		const mlir::lsp::URIForFile &uri,
		std::vector<mlir::lsp::DocumentSymbol> &symbols)
{
	// ToDo
}

void RLCServer::addOrUpdateDocument(
		const mlir::lsp::URIForFile &uri,
		llvm::StringRef contents,
		int64_t version,
		std::vector<mlir::lsp::Diagnostic> &diagnostics)
{
	if (fileToModule.contains(uri.uri()))
		fileToModule.erase(uri.uri());

	auto result = fileToModule.try_emplace(
			uri.uri(), LSPModuleInfo(uri.file(), contents, version, *context));

	for (const auto &diag : result.first->second.getDiagnostics())
		diagnostics.push_back(rlcDiagToLSPDiag(diag));
	result.first->second.clearDiagnostics();
}

mlir::lsp::CompletionList RLCServer::getCodeCompletion(
		const mlir::lsp::URIForFile &uri, const mlir::lsp::Position &completePos)
{
	const auto *maybeInfo = getModuleFromUri(uri);
	mlir::lsp::CompletionList list;

	if (maybeInfo == nullptr)
		return list;

	if (maybeInfo->getCompleteType(completePos, list).succeeded())
	{
		return list;
	}

	if (maybeInfo->getCompleteImport(completePos, list).succeeded())
	{
		return list;
	}

	if (maybeInfo->getCompleteAccessMember(completePos, list).succeeded())
	{
		return list;
	}

	if (maybeInfo->getCompleteFunction(completePos, list).succeeded())
	{
		return list;
	}

	// if we don't know what the user is typing, just provide types
	maybeInfo->getTypes(list);

	list.isIncomplete = true;

	return list;
}

std::optional<int64_t> RLCServer::removeDocument(
		const mlir::lsp::URIForFile &uri)
{
	if (fileToModule.contains(uri.uri()))
	{
		int64_t version = fileToModule.at(uri.uri()).getVersion();
		fileToModule.erase(uri.uri());
		return version;
	}

	return std::nullopt;
}

void RLCServer::getLocationsOf(
		const mlir::lsp::URIForFile &uri,
		const mlir::lsp::Position &defPos,
		std::vector<mlir::lsp::Location> &locations)
{
	const auto *maybeInfo = getModuleFromUri(uri);
	if (maybeInfo == nullptr)
		return;

	maybeInfo->getLocationsOf(defPos, locations);
}

void RLCServer::findReferencesOf(
		const mlir::lsp::URIForFile &uri,
		const mlir::lsp::Position &pos,
		std::vector<mlir::lsp::Location> &references)
{
	const auto *maybeInfo = getModuleFromUri(uri);
	if (maybeInfo == nullptr)
		return;

	maybeInfo->findReferencesOf(pos, references);
}

mlir::lsp::WorkspaceEdit RLCServer::rename(
		const mlir::lsp::URIForFile &uri,
		llvm::StringRef newName,
		mlir::lsp::Position position)
{
	mlir::lsp::WorkspaceEdit action;

	const auto *maybeInfo = getModuleFromUri(uri);
	if (maybeInfo == nullptr)
		return action;

	auto decl = getClassDefinition(maybeInfo->getModule(), position);
	if (decl != nullptr)
	{
		maybeInfo->getModule().walk([&](mlir::rlc::TypeUser user) {
			for (auto [range, type] :
					 llvm::zip(user.getTypeSourceRange(), user.getExplicitType()))
			{
				auto file = range.getStart().getFilename().str();
				if (type != decl.getType())
					continue;

				action.changes[file].emplace_back();
				action.changes[file].back().newText = newName;
				action.changes[file].back().range.start = locToPos(range.getStart());
				action.changes[file].back().range.end = locToPos(range.getEnd());
			}
		});
	}

	return action;
}
