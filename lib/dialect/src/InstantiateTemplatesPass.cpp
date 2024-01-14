/*
This file is part of the RLC project.

RLC is free software: you can redistribute it and/or modify it under the terms
of the GNU General Public License version 2 as published by the Free Software
Foundation.

RLC is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
RLC. If not, see <https://www.gnu.org/licenses/>.
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
			llvm::StringMap<llvm::DenseSet<mlir::rlc::EntityType>>& out)
	{
		for (auto t : t.getInputs())
			if (auto casted = t.dyn_cast<mlir::rlc::EntityType>())
				out[casted.getName()].insert(casted);

		if (t.getNumResults() == 0)
			return;

		if (auto casted = t.getResult(0).dyn_cast<mlir::rlc::EntityType>())
			out[casted.getName()].insert(casted);
	}

	static void collectAllTypesOnFunctionAndActions(
			mlir::ModuleOp op,
			llvm::StringMap<llvm::DenseSet<mlir::rlc::EntityType>>& out)
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
			mlir::rlc::EntityType type,
			mlir::rlc::EntityDeclaration originalDecl)
	{
		if (type == originalDecl.getType())
			return;
		assert(isTemplateType(originalDecl.getType()).succeeded());
		mlir::IRRewriter rewriter(op.getContext());
		rewriter.setInsertionPoint(originalDecl);
		rewriter.create<mlir::rlc::EntityDeclaration>(
				originalDecl.getLoc(),
				type,
				originalDecl.getNameAttr(),
				rewriter.getTypeArrayAttr(type.getBody()),
				originalDecl.getMemberNames(),
				rewriter.getArrayAttr({}));
	}

	static void declareInstantiatedStructs(mlir::ModuleOp op)
	{
		llvm::StringMap<llvm::DenseSet<mlir::rlc::EntityType>>
				outwardExposedTemplateTypes;

		collectAllTypesOnFunctionAndActions(op, outwardExposedTemplateTypes);
		llvm::StringMap<mlir::rlc::EntityDeclaration> decls;
		for (auto decl : op.getOps<mlir::rlc::EntityDeclaration>())
		{
			decls[decl.getName()] = decl;
		}

		for (auto& pair : outwardExposedTemplateTypes)
			for (auto type : pair.second)
				instantiateStructDeclarationIfNeeded(op, type, decls[pair.first()]);

		for (auto& originalDecl : decls)
			if (isTemplateType(originalDecl.second.getType()).succeeded())
				originalDecl.second.erase();
	}

	struct InstantiateTemplatesPass
			: impl::InstantiateTemplatesPassBase<InstantiateTemplatesPass>
	{
		using impl::InstantiateTemplatesPassBase<
				InstantiateTemplatesPass>::InstantiateTemplatesPassBase;
		llvm::DenseMap<mlir::Type, bool> destructorsCache;

		mlir::Value redirectMaterializedCallFromTraitToExplicitFunction(
				mlir::IRRewriter& rewriter,
				mlir::rlc::ValueTable& symbolTable,
				mlir::rlc::TemplateInstantiationOp op,
				mlir::rlc::TraitDefinition trait)

		{
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
			if (auto trait =
							op.getInputTemplate().getDefiningOp<mlir::rlc::TraitDefinition>())
			{
				auto replacedTraitMethodUsage =
						redirectMaterializedCallFromTraitToExplicitFunction(
								rewriter, symbolTable, op, trait);
				if (auto casted =
								replacedTraitMethodUsage
										.getDefiningOp<mlir::rlc::TemplateInstantiationOp>())
					op = casted;
				else
					return replacedTraitMethodUsage;
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
			op.erase();

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

			bool replacedAtLeastOne = true;
			while (replacedAtLeastOne)
			{
				emitImplicitAssign(getOperation());
				emitImplicitInits(getOperation());
				mlir::rlc::ModuleBuilder builder(getOperation());
				replacedAtLeastOne = false;
				llvm::SmallVector<mlir::rlc::TemplateInstantiationOp, 4> ops;
				getOperation().walk([&](mlir::rlc::TemplateInstantiationOp op) {
					if (isFullyDeterminedInstantiation(op))
						ops.push_back(op);
				});

				for (auto op : ops)
				{
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

			declareInstantiatedStructs(getOperation());

			getOperation().walk([&](mlir::rlc::TemplateInstantiationOp op) {
				op->getParentOfType<mlir::rlc::FunctionOp>().dump();
				assert(
						false && "found a template instantiation op surviving the template "
										 "instantiaion pass");
			});
		}
	};
}	 // namespace mlir::rlc
