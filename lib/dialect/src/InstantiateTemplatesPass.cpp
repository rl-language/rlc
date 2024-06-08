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
#include "llvm/ADT/TypeSwitch.h"
#include "mlir/IR/BuiltinDialect.h"
#include "rlc/dialect/Operations.hpp"
#include "rlc/dialect/Passes.hpp"
#include "rlc/dialect/conversion/TypeConverter.h"

namespace mlir::rlc
{

#define GEN_PASS_DEF_INSTANTIATETEMPLATESPASS
#include "rlc/dialect/Passes.inc"

	static bool isFullyDeterminedInstantiation(
			mlir::rlc::TemplateInstantiationOp op)
	{
		for (auto type : op.getType().getInputs())
			if (isTemplateType(type).succeeded())
				return false;
		return true;
	}

	static void registerFunctionTypes(
			mlir::FunctionType t,
			llvm::StringMap<llvm::DenseSet<mlir::rlc::ClassType>>& out)
	{
		for (auto t : t.getInputs())
			if (auto casted = t.dyn_cast<mlir::rlc::ClassType>())
				out[casted.getName()].insert(casted);

		if (t.getNumResults() == 0)
			return;

		if (auto casted = t.getResult(0).dyn_cast<mlir::rlc::ClassType>())
			out[casted.getName()].insert(casted);
	}

	static void collectAllTypesOnFunctionAndActions(
			mlir::ModuleOp op,
			llvm::StringMap<llvm::DenseSet<mlir::rlc::ClassType>>& out)
	{
		for (auto op : op.getOps<mlir::rlc::FunctionOp>())
			registerFunctionTypes(op.getType(), out);

		for (auto op : op.getOps<mlir::rlc::ActionFunction>())
			for (auto emittedFunction : op.getResultTypes())
				if (emittedFunction.isa<mlir::FunctionType>())
					registerFunctionTypes(
							emittedFunction.cast<mlir::FunctionType>(), out);
	}

	static void instantiateStructDeclarationIfNeeded(
			mlir::ModuleOp op,
			mlir::rlc::ClassType type,
			mlir::rlc::ClassDeclaration originalDecl)
	{
		assert(isTemplateType(originalDecl.getType()).succeeded());
		mlir::IRRewriter rewriter(op.getContext());
		rewriter.setInsertionPoint(originalDecl);
		rewriter.create<mlir::rlc::ClassDeclaration>(
				originalDecl.getLoc(),
				type,
				originalDecl.getNameAttr(),
				rewriter.getTypeArrayAttr(type.getBody()),
				originalDecl.getMemberNames(),
				rewriter.getArrayAttr({}));
	}

	static void declareInstantiatedStructs(
			llvm::StringMap<mlir::rlc::ClassDeclaration>& originalDecls,
			llvm::DenseSet<mlir::Type>& alreadyDeclared,
			mlir::ModuleOp op)
	{
		llvm::StringMap<llvm::DenseSet<mlir::rlc::ClassType>>
				outwardExposedTemplateTypes;

		collectAllTypesOnFunctionAndActions(op, outwardExposedTemplateTypes);

		for (auto& pair : outwardExposedTemplateTypes)
			for (auto type : pair.second)
			{
				if (alreadyDeclared.contains(type))
					continue;
				instantiateStructDeclarationIfNeeded(
						op, type, originalDecls[type.getName()]);
				alreadyDeclared.insert(type);
			}
	}

	struct InstantiateTemplatesPass
			: impl::InstantiateTemplatesPassBase<InstantiateTemplatesPass>
	{
		using impl::InstantiateTemplatesPassBase<
				InstantiateTemplatesPass>::InstantiateTemplatesPassBase;
		llvm::DenseMap<mlir::Type, bool> destructorsCache;

		// if the template instantiation refers to a trait, resolves the real
		// function the trait was poiting at, and returns a template instantiation
		// to it.
		mlir::Value replaceTraitInstantiationWithUnderlyingFunctionInstantiation(
				mlir::IRRewriter& rewriter,
				mlir::rlc::ValueTable& symbolTable,
				mlir::rlc::TemplateInstantiationOp op)

		{
			auto trait =
					op.getInputTemplate().getDefiningOp<mlir::rlc::TraitDefinition>();
			if (not trait)
				return op;

			mlir::rlc::OverloadResolver resolver(symbolTable);
			rewriter.setInsertionPoint(op);
			size_t index = 0;
			for (auto result : trait.getResults())
				if (result != op.getInputTemplate())
					index++;
				else
					break;

			auto newInstantiation = resolver.instantiateOverload(
					rewriter,
					false,
					op.getLoc(),
					trait.getMetaType().getRequestedFunctionNames()[index],
					op.getType().getInputs());

			if (newInstantiation == nullptr)
			{
				op->emitError(
						llvm::Twine("could not find overload for function ") +
						trait.getMetaType().getRequestedFunctionNames()[index].strref());
				op.getType().dump();
				op->getParentOfType<mlir::rlc::FunctionOp>().dump();
				abort();
			}

			op.replaceAllUsesWith(newInstantiation);
			op.erase();
			return newInstantiation;
		}

		mlir::Value instantiate(
				mlir::rlc::ModuleBuilder& builder,
				mlir::IRRewriter& rewriter,
				mlir::rlc::ValueTable& symbolTable,
				mlir::rlc::TemplateInstantiationOp op)
		{
			if (isTemplateType(op.getInputTemplate().getType()).failed())
			{
				auto input = op.getInputTemplate();
				op.replaceAllUsesWith(input);
				op.erase();
				return input;
			}

			rewriter.setInsertionPoint(op.getInputTemplate().getDefiningOp());
			OverloadResolver resolver(builder.getSymbolTable());
			auto* clone = rewriter.clone(*op.getInputTemplate().getDefiningOp());

			llvm::DenseMap<mlir::rlc::TemplateParameterType, mlir::Type> deductions;
			for (auto [first, second] : llvm::zip(
							 op.getInputTemplate().getType().getInputs(),
							 op.getType().getInputs()))
			{
				resolver.deduceSubstitutions(op.getLoc(), deductions, first, second)
						.succeeded();
			}
			if (op.getInputTemplate().getType().getNumResults() != 0)
				resolver
						.deduceSubstitutions(
								op.getLoc(),
								deductions,
								op.getInputTemplate().getType().getResult(0),
								op.getType().getResult(0))
						.succeeded();

			for (auto sobstitution : deductions)
			{
				mlir::AttrTypeReplacer replacer;
				replacer.addReplacement([&](mlir::Type t) -> std::optional<mlir::Type> {
					if (t == sobstitution.first)
						return sobstitution.second;
					return std::nullopt;
				});
				replacer.recursivelyReplaceElementsIn(clone, true, true, true);
			}

			auto resolvedFunction =
					mlir::cast<mlir::rlc::FunctionOp>(clone).getResult();

			lowerForFields(builder, clone);
			lowerIsOperations(clone, symbolTable);
			lowerAssignOps(builder, clone);
			lowerConstructOps(builder, clone);
			lowerDestructors(destructorsCache, builder, clone);

			op.replaceAllUsesWith(resolvedFunction);

			return resolvedFunction;
		}

		struct AlreadyReplacedMapEntry
		{
			public:
			mlir::Operation* originalTemplate;
			mlir::Type instationationType;

			bool operator<(const AlreadyReplacedMapEntry& other) const
			{
				return std::pair{ originalTemplate,
													instationationType.getAsOpaquePointer() } <
							 std::pair{ other.originalTemplate,
													other.instationationType.getAsOpaquePointer() };
			}
		};

		void runOnOperation() override
		{
			std::map<AlreadyReplacedMapEntry, mlir::Value> alreadyReplaced;
			mlir::IRRewriter rewriter(&getContext());

			llvm::DenseSet<mlir::Type> alreadDeclared;
			llvm::StringMap<mlir::rlc::ClassDeclaration> originalEntiDecl;
			for (auto decl : getOperation().getOps<mlir::rlc::ClassDeclaration>())
			{
				alreadDeclared.insert(decl.getType());
				originalEntiDecl[decl.getName()] = decl;
			}

			bool replacedAtLeastOne = true;
			while (replacedAtLeastOne)
			{
				declareInstantiatedStructs(
						originalEntiDecl, alreadDeclared, getOperation());
				emitImplicitAssign(getOperation());
				emitImplicitInits(getOperation());
				mlir::rlc::ModuleBuilder builder(getOperation());
				replacedAtLeastOne = false;
				llvm::SmallVector<mlir::rlc::TemplateInstantiationOp, 4> ops;
				getOperation().walk([&](mlir::rlc::TemplateInstantiationOp op) {
					if (isFullyDeterminedInstantiation(op))
					{
						ops.push_back(op);
					}
				});

				for (auto op : ops)
				{
					mlir::Value unwrappedInstantiation =
							replaceTraitInstantiationWithUnderlyingFunctionInstantiation(
									rewriter, builder.getSymbolTable(), op);
					if (op != unwrappedInstantiation.getDefiningOp())
					{
						auto casted = dyn_cast_or_null<mlir::rlc::TemplateInstantiationOp>(
								unwrappedInstantiation.getDefiningOp());
						if (casted)
							op = casted;
						else
							continue;
					}

					AlreadyReplacedMapEntry mapKey{ op.getInputTemplate().getDefiningOp(),
																					op.getType() };
					if (auto iter = alreadyReplaced.find(mapKey);
							iter != alreadyReplaced.end())
					{
						op.replaceAllUsesWith(iter->second);
						op.erase();
					}
					else
					{
						replacedAtLeastOne = true;
						auto newInstance =
								instantiate(builder, rewriter, builder.getSymbolTable(), op);
						alreadyReplaced[mapKey] = newInstance;
					}
				}
			}

			llvm::SmallVector<mlir::rlc::FunctionOp> templates;
			for (auto op : getOperation().getOps<mlir::rlc::FunctionOp>())
			{
				if (op.isTemplate().succeeded())
					templates.push_back(op);
			}

			for (auto op : templates)
			{
				op.getOperation()->dropAllUses();
				op.erase();
			}

			llvm::DenseSet<mlir::rlc::ClassDeclaration> templateDecl;
			for (auto decl : getOperation().getOps<mlir::rlc::ClassDeclaration>())
			{
				if (isTemplateType(decl.getType()).succeeded())
					templateDecl.insert(decl);
			}
			for (auto& originalDecl : templateDecl)
				originalDecl.erase();

			getOperation().walk([&](mlir::rlc::TemplateInstantiationOp op) {
				op->getParentOfType<mlir::rlc::FunctionOp>().dump();
				assert(
						false && "found a template instantiation op surviving the template "
										 "instantiaion pass");
			});
		}
	};
}	 // namespace mlir::rlc
