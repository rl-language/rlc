#include "rlc/lsp/LSP.hpp"

#include "rlc/dialect/Operations.hpp"
#include "rlc/dialect/Types.hpp"
#include "rlc/utils/Error.hpp"

using namespace mlir::rlc::lsp;

class mlir::rlc::lsp::LSPModuleInfoImpl
{
	public:
	~LSPModuleInfoImpl() { module->erase(); }
	explicit LSPModuleInfoImpl(mlir::ModuleOp module): module(module)
	{
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
		});
		module.walk([this](mlir::rlc::ActionFunction decl) {
			if (decl.getBody().empty())
				return;
			auto firstInstructionLoc =
					decl.getBody().front().front().getLoc().cast<mlir::FileLineColLoc>();
			auto loc = decl.getLoc().cast<mlir::FileLineColLoc>();
			declarations.emplace_back(
					locsToRange(loc, firstInstructionLoc), decl.getOperation());
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
		});

		module.walk([this](mlir::rlc::MemberAccess decl) {
			if (decl->getNextNode() == nullptr)
				return;
			auto nextInst =
					decl->getNextNode()->getLoc().cast<mlir::FileLineColLoc>();
			auto loc = decl.getLoc().cast<mlir::FileLineColLoc>();
			memberAcceses.emplace_back(
					locsToRange(loc, nextInst), decl.getOperation());
		});
	}

	mlir::ModuleOp getModule() const { return module; }

	// zero based positions
	mlir::Operation *getOperation(const mlir::lsp::Position &pos)
	{
		for (const auto &pair :
				 llvm::make_range(declarations.rbegin(), declarations.rend()))
		{
			if (pair.first.contains(pos))
				return pair.second;
		}
		return nullptr;
	}

	mlir::Operation *getNearestMemberAccess(const mlir::lsp::Position &pos)
	{
		for (const auto &pair :
				 llvm::make_range(memberAcceses.rbegin(), memberAcceses.rend()))
		{
			if (pair.first.contains(pos))
				return pair.second;
		}
		return nullptr;
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
		if (auto casted = type.dyn_cast<mlir::rlc::EntityType>())
		{
			for (const auto &field :
					 llvm::zip(casted.getFieldNames(), casted.getBody()))
			{
				mlir::lsp::CompletionItem item;
				item.label = std::get<0>(field);
				item.kind = mlir::lsp::CompletionItemKind::Field;
				item.insertTextFormat = mlir::lsp::InsertTextFormat::PlainText;
				item.detail = prettyType(std::get<1>(field));
				list.items.push_back(item);
			}
		}

		for (auto fun : module.getOps<mlir::rlc::FunctionOp>())
		{
			if (fun.getType().getNumInputs() == 0)
				continue;

			if (fun.getType().getInput(0) == type or
					isTemplateType(fun.getType().getInput(0)).succeeded())
			{
				mlir::lsp::CompletionItem item;
				item.label = (fun.getUnmangledName() + "()").str();
				item.kind = mlir::lsp::CompletionItemKind::Function;
				item.insertTextFormat = mlir::lsp::InsertTextFormat::PlainText;
				item.detail = prettyType(fun.getType());
				list.items.push_back(item);
			}
		}

		for (auto fun : module.getOps<mlir::rlc::ActionFunction>())
		{
			if (fun.getMainActionType() == type)
				continue;

			fun.walk([&](mlir::rlc::ActionStatement statemet) {
				mlir::lsp::CompletionItem item;
				item.label = (statemet.getName() + "()").str();
				item.kind = mlir::lsp::CompletionItemKind::Function;
				item.insertTextFormat = mlir::lsp::InsertTextFormat::PlainText;
				item.detail = prettyType(mlir::FunctionType::get(
						fun.getContext(), statemet.getResultTypes(), {}));
				list.items.push_back(item);
			});
		}
		return mlir::success();
	}

	private:
	static mlir::lsp::Range locsToRange(mlir::Location begin, mlir::Location end)
	{
		auto castedBegin = begin.cast<mlir::FileLineColLoc>();
		auto castedEnd = end.cast<mlir::FileLineColLoc>();

		return mlir::lsp::Range(
				mlir::lsp::Position(
						static_cast<int>(castedBegin.getLine()) - 1,
						static_cast<int>(castedBegin.getColumn()) - 1),
				mlir::lsp::Position(
						static_cast<int>(castedEnd.getLine()) - 1,
						static_cast<int>(castedEnd.getColumn()) - 1));
	}
	mlir::ModuleOp module;
	llvm::SmallVector<std::pair<mlir::lsp::Range, mlir::Operation *>>
			declarations;

	llvm::SmallVector<std::pair<mlir::lsp::Range, mlir::Operation *>>
			memberAcceses;
};

LSPModuleInfo::LSPModuleInfo(mlir::ModuleOp op, int64_t version)
		: impl(new LSPModuleInfoImpl(op)), version(version)
{
}

mlir::LogicalResult LSPModuleInfo::getCompleteAccessMember(
		const mlir::lsp::Position &completePos,
		mlir::lsp::CompletionList &list) const
{
	return impl->getCompleteAccessMember(completePos, list);
}

mlir::ModuleOp LSPModuleInfo::getModule() const { return impl->getModule(); }

LSPModuleInfo &LSPModuleInfo::operator=(LSPModuleInfo &&other)
{
	if (this == &other)
		return *this;

	delete impl;
	impl = other.impl;
	other.impl = nullptr;
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
		return (casted.getName() + ": " + mlir::rlc::prettyType(casted.getType()))
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

	auto ast = mlir::ModuleOp::create(
			mlir::FileLineColLoc::get(context->getContext(), uri.file(), 0, 0),
			uri.file());
	auto maybeModule = context->loadFile(uri.file(), contents, version, ast);
	if (not maybeModule)
	{
		auto error = llvm::handleErrors(
				maybeModule.takeError(),
				[&](const llvm::StringError &error) {
					mlir::lsp::Diagnostic diag;
					diag.severity = mlir::lsp::DiagnosticSeverity::Error;
					diag.range = mlir::lsp::Range(
							mlir::lsp::Position(0, 0), mlir::lsp::Position(0, 1));
					diag.message = error.getMessage();
					diagnostics.push_back(diag);
				},
				[&](const ::rlc::RlcError &error) {
					mlir::lsp::Diagnostic diag;
					diag.severity = mlir::lsp::DiagnosticSeverity::Error;
					auto casted = error.getPosition().cast<mlir::FileLineColLoc>();
					diag.range = mlir::lsp::Range(
							mlir::lsp::Position(casted.getLine() - 1, casted.getColumn() - 1),
							mlir::lsp::Position(
									casted.getLine() - 1, casted.getColumn() - 1));
					diag.message = error.getText();
					diagnostics.push_back(diag);
				});
		if (error)
		{
			mlir::lsp::Diagnostic diag;
			diag.message = "lsp: unkown llvm error";
			diag.severity = mlir::lsp::DiagnosticSeverity::Error;
			diag.range = mlir::lsp::Range(
					mlir::lsp::Position(0, 0), mlir::lsp::Position(0, 1));
			diagnostics.push_back(diag);
			llvm::consumeError(std::move(error));
		}
	}
	fileToModule.try_emplace(uri.uri(), LSPModuleInfo(ast, version));
}

mlir::lsp::CompletionList RLCServer::getCodeCompletion(
		const mlir::lsp::URIForFile &uri, const mlir::lsp::Position &completePos)
{
	const auto *maybeInfo = getModuleFromUri(uri);
	mlir::lsp::CompletionList list;

	if (maybeInfo == nullptr)
		return list;

	if (maybeInfo->getCompleteAccessMember(completePos, list).succeeded())
	{
		return list;
	}

	list.isIncomplete = true;
	for (auto global : maybeInfo->getModule().getOps<mlir::rlc::FunctionOp>())
	{
		mlir::lsp::CompletionItem item;
		item.label = global.getUnmangledName();
		item.kind = mlir::lsp::CompletionItemKind::Function;
		item.insertTextFormat = mlir::lsp::InsertTextFormat::PlainText;
		item.detail = prettyType(global.getType());
		list.items.push_back(item);
	}

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
