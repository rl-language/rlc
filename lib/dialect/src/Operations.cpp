#include "rlc/dialect/Operations.hpp"

#include "rlc/dialect/Dialect.h"
#define GET_OP_CLASSES
#include "./Operations.inc"

void mlir::rlc::RLCDialect::registerOperations()
{
	addOperations<
#define GET_OP_LIST
#include "./Operations.inc"
			>();
}

static llvm::SmallVector<mlir::Operation *, 4> ops(mlir::Region &region)
{
	llvm::SmallVector<mlir::Operation *, 4> toReturn;
	for (auto &op : region.getOps())
		toReturn.push_back(&op);

	return toReturn;
}

::mlir::LogicalResult mlir::rlc::ArrayCallOp::verifySymbolUses(
		::mlir::SymbolTableCollection &symbolTable)
{
	return LogicalResult::success();
}

::mlir::LogicalResult mlir::rlc::CallOp::verifySymbolUses(
		::mlir::SymbolTableCollection &symbolTable)
{
	return LogicalResult::success();
}

mlir::LogicalResult mlir::rlc::ExpressionStatement::typeCheck(
		mlir::IRRewriter &rewriter,
		mlir::rlc::SymbolTable<mlir::Value> &table,
		mlir::rlc::RLCTypeConverter &converter)
{
	for (auto *op : ops(getBody()))
		if (mlir::rlc::typeCheck(*op, rewriter, table, converter).failed())
			return mlir::failure();

	return mlir::success();
}

mlir::LogicalResult mlir::rlc::DeclarationStatement::typeCheck(
		mlir::IRRewriter &rewriter,
		mlir::rlc::SymbolTable<mlir::Value> &table,
		mlir::rlc::RLCTypeConverter &conv)
{
	for (auto *child : ops(getBody()))
	{
		if (mlir::rlc::typeCheck(*child, rewriter, table, conv).failed())
			return mlir::failure();
	}

	rewriter.setInsertionPoint(getOperation());
	auto deducedType = getBody().front().getTerminator()->getOperand(0).getType();

	auto newOne = rewriter.create<mlir::rlc::DeclarationStatement>(
			getLoc(), deducedType, getName());
	newOne.getBody().takeBody(getBody());
	rewriter.replaceOp(*this, { newOne });

	table.add(newOne.getSymName(), newOne);
	return mlir::success();
}

mlir::LogicalResult mlir::rlc::ArrayAccess::typeCheck(
		mlir::IRRewriter &rewriter,
		mlir::rlc::SymbolTable<mlir::Value> &table,
		mlir::rlc::RLCTypeConverter &conv)
{
	rewriter.replaceOpWithNewOp<mlir::rlc::ArrayAccess>(
			*this, getValue(), getMemberIndex());
	return mlir::success();
}

mlir::LogicalResult mlir::rlc::Yield::typeCheck(
		mlir::IRRewriter &rewriter,
		mlir::rlc::SymbolTable<mlir::Value> &table,
		mlir::rlc::RLCTypeConverter &conv)
{
	return mlir::success();
}

mlir::LogicalResult mlir::rlc::ReturnStatement::typeCheck(
		mlir::IRRewriter &rewriter,
		mlir::rlc::SymbolTable<mlir::Value> &table,
		mlir::rlc::RLCTypeConverter &conv)
{
	for (auto *op : ops(getBody()))
	{
		if (mlir::rlc::typeCheck(*op, rewriter, table, conv).failed())
			return mlir::failure();
	}
	rewriter.setInsertionPoint(*this);
	auto *yield = getBody().front().getTerminator();
	auto newOne = rewriter.create<mlir::rlc::ReturnStatement>(
			getLoc(),
			yield->getNumOperands() != 0 ? yield->getOpOperand(0).get().getType()
																	 : mlir::rlc::VoidType::get(getContext()));
	newOne.getBody().takeBody(getBody());
	rewriter.eraseOp(*this);

	return mlir::success();
}

mlir::LogicalResult mlir::rlc::UnresolvedReference::typeCheck(
		mlir::IRRewriter &rewriter,
		mlir::rlc::SymbolTable<mlir::Value> &table,
		mlir::rlc::RLCTypeConverter &conv)
{
	bool usedByCall = llvm::any_of(
			getResult().getUsers(), [&](const mlir::OpOperand &operand) -> bool {
				auto call = mlir::dyn_cast<mlir::rlc::CallOp>(*operand.getOwner());
				return call and call.getCallee() == getResult();
			});
	if (usedByCall)
	{
		assert(llvm::count_if(getResult().getUsers(), [](const auto &) {
						 return true;
					 }) == 1);

		return mlir::success();
	}
	auto candidates = table.get(getName());
	if (candidates.size() > 2)
	{
		emitError("ambigous reference to " + getName());
		for (auto candidate : candidates)
		{
			candidate.getDefiningOp()->emitRemark("candidate");
		}
		return mlir::failure();
	}

	if (candidates.empty())
	{
		emitError("no known value " + getName());

		return mlir::failure();
	}

	replaceAllUsesWith(candidates.front());
	rewriter.eraseOp(*this);
	return mlir::success();
}

mlir::LogicalResult mlir::rlc::Constant::typeCheck(
		mlir::IRRewriter &rewriter,
		mlir::rlc::SymbolTable<mlir::Value> &table,
		mlir::rlc::RLCTypeConverter &conv)
{
	return mlir::success();
}

mlir::LogicalResult mlir::rlc::ValueUpcastOp::typeCheck(
		mlir::IRRewriter &rewriter,
		mlir::rlc::SymbolTable<mlir::Value> &table,
		mlir::rlc::RLCTypeConverter &conv)
{
	return mlir::success();
}

mlir::LogicalResult mlir::rlc::UncheckedIsOp::typeCheck(
		mlir::IRRewriter &rewriter,
		mlir::rlc::SymbolTable<mlir::Value> &table,
		mlir::rlc::RLCTypeConverter &conv)
{
	auto deducedType = conv.convertType(getTypeOrTrait());
	if (deducedType == nullptr)
	{
		emitRemark("In Is expression");
		return mlir::failure();
	}

	rewriter.setInsertionPoint(getOperation());
	rewriter.replaceOpWithNewOp<mlir::rlc::IsOp>(
			*this, getExpression(), deducedType);

	return mlir::success();
}

mlir::LogicalResult mlir::rlc::StatementList::typeCheck(
		mlir::IRRewriter &rewriter,
		mlir::rlc::SymbolTable<mlir::Value> &table,
		mlir::rlc::RLCTypeConverter &conv)
{
	mlir::rlc::SymbolTable innerTable(&table);

	for (auto *op : ops(getBody()))
		if (mlir::rlc::typeCheck(*op, rewriter, innerTable, conv).failed())
			return mlir::failure();

	return mlir::success();
}

mlir::LogicalResult mlir::rlc::UncheckedTraitDefinition::typeCheck(
		mlir::IRRewriter &rewriter,
		mlir::rlc::SymbolTable<mlir::Value> &table,
		mlir::rlc::RLCTypeConverter &conv)
{
	mlir::rlc::RLCTypeConverter templateConverter(&conv);
	templateConverter.registerType(
			getTemplateParameter(), getTemplateParameterType());

	llvm::SmallVector<mlir::rlc::FunctionOp, 4> Ops(
			getBody().getOps<mlir::rlc::FunctionOp>());
	for (auto op : Ops)
		if (op.typeCheckFunctionDeclaration(rewriter, templateConverter).failed())
			return mlir::failure();

	llvm::SmallVector<mlir::StringAttr> names;
	llvm::SmallVector<mlir::FunctionType> types;

	for (auto op : getBody().getOps<mlir::rlc::FunctionOp>())
	{
		names.push_back(op.getUnmangledNameAttr());
		types.push_back(op.getFunctionType());
	}

	auto type = mlir::rlc::TraitMetaType::get(
			getContext(), getName(), getTemplateParameter(), types, names);
	rewriter.setInsertionPointAfter(*this);
	rewriter.create<mlir::rlc::TraitDefinition>(getLoc(), type);
	rewriter.eraseOp(*this);
	return mlir::success();
}

static void promoteArgumentOfIsOp(
		mlir::IRRewriter &rewriter,
		mlir::rlc::SymbolTable<mlir::Value> &table,
		mlir::rlc::IfStatement op)
{
	rewriter.setInsertionPointToStart(&op.getTrueBranch().front());
	auto isOp = op.getCondition()
									.front()
									.getTerminator()
									->getOperand(0)
									.getDefiningOp<mlir::rlc::IsOp>();
	if (not isOp)
		return;

	if (auto trait = isOp.getTypeOrTrait().isa<mlir::rlc::TraitMetaType>())
		return;

	if (auto name = table.lookUpValue(isOp.getExpression()); not name.empty())
	{
		auto upcastedValue = rewriter.create<mlir::rlc::ValueUpcastOp>(
				isOp.getLoc(), isOp.getTypeOrTrait(), isOp.getExpression());
		table.add(name, upcastedValue);
	}
}

mlir::LogicalResult mlir::rlc::IfStatement::typeCheck(
		mlir::IRRewriter &rewriter,
		mlir::rlc::SymbolTable<mlir::Value> &table,
		mlir::rlc::RLCTypeConverter &conv)
{
	for (auto *op : ops(getCondition()))
		if (mlir::rlc::typeCheck(*op, rewriter, table, conv).failed())
			return mlir::failure();

	if (not getCondition()
							.front()
							.getTerminator()
							->getOperand(0)
							.getType()
							.isa<mlir::rlc::BoolType>())
	{
		emitError("while loop condition is not boolean");
		return mlir::failure();
	}

	mlir::rlc::SymbolTable innerTable(&table);
	promoteArgumentOfIsOp(rewriter, innerTable, *this);
	for (auto *op : ops(getTrueBranch()))
		if (mlir::rlc::typeCheck(*op, rewriter, innerTable, conv).failed())
			return mlir::failure();

	mlir::rlc::SymbolTable innerTable2(&table);
	for (auto *op : ops(getElseBranch()))
		if (mlir::rlc::typeCheck(*op, rewriter, innerTable, conv).failed())
			return mlir::failure();

	return mlir::success();
}

mlir::LogicalResult mlir::rlc::ArrayCallOp::typeCheck(
		mlir::IRRewriter &rewriter,
		mlir::rlc::SymbolTable<mlir::Value> &table,
		mlir::rlc::RLCTypeConverter &conv)
{
	assert(
			(getNumResults() == 0 or
			 not getResult(0).getType().isa<mlir::rlc::UnknownType>()) and
			"unuspported, array calls should be only emitted with a correct type "
			"already");
	return mlir::success();
}

mlir::LogicalResult mlir::rlc::CallOp::typeCheck(
		mlir::IRRewriter &rewriter,
		mlir::rlc::SymbolTable<mlir::Value> &table,
		mlir::rlc::RLCTypeConverter &conv)
{
	auto callee = getCallee().getDefiningOp<mlir::rlc::UnresolvedReference>();
	mlir::Value toCall = getCallee();
	if (callee != nullptr)
	{
		mlir::rlc::OverloadResolver resolver(table, *this);
		rewriter.setInsertionPoint(getOperation());
		auto candidate = resolver.instantiateOverload(
				rewriter, getLoc(), callee.getName(), getArgs().getType());
		if (candidate == nullptr)
		{
			emitRemark("while calling");
			return mlir::failure();
		}
		toCall = candidate;
	}

	if (not toCall.getType().isa<mlir::FunctionType>())
	{
		emitError("cannot call non function type");
		return LogicalResult::failure();
	}

	auto newOne = rewriter.create<mlir::rlc::CallOp>(getLoc(), toCall, getArgs());

	if (newOne.getNumResults() != 0)
	{
		rewriter.replaceOp(*this, newOne.getResults());
		if (callee)
			rewriter.eraseOp(callee);
		return mlir::success();
	}

	rewriter.eraseOp(*this);
	if (callee)
		rewriter.eraseOp(callee);
	return mlir::success();
}

mlir::LogicalResult mlir::rlc::WhileStatement::typeCheck(
		mlir::IRRewriter &rewriter,
		mlir::rlc::SymbolTable<mlir::Value> &table,
		mlir::rlc::RLCTypeConverter &conv)
{
	for (auto *op : ops(getCondition()))
		if (mlir::rlc::typeCheck(*op, rewriter, table, conv).failed())
			return mlir::failure();

	if (not getCondition()
							.front()
							.getTerminator()
							->getOperand(0)
							.getType()
							.isa<mlir::rlc::BoolType>())
	{
		emitError("while loop condition is not boolean");
		return mlir::failure();
	}

	mlir::rlc::SymbolTable innerTable(&table);
	for (auto *op : ops(getBody()))
		if (mlir::rlc::typeCheck(*op, rewriter, innerTable, conv).failed())
			return mlir::failure();

	return mlir::success();
}

mlir::LogicalResult mlir::rlc::UnresConstructOp::typeCheck(
		mlir::IRRewriter &rewriter,
		mlir::rlc::SymbolTable<mlir::Value> &table,
		mlir::rlc::RLCTypeConverter &conv)
{
	auto deducedType = conv.convertType(getType());
	if (deducedType == nullptr)
	{
		emitRemark("in construction expression");
		return mlir::failure();
	}

	auto realType = deducedType;
	bool isArray = false;
	if (auto array = deducedType.dyn_cast<mlir::rlc::ArrayType>();
			array != nullptr)
	{
		isArray = true;
		deducedType = array.getUnderlying();
	}

	OverloadResolver resolver(table, getOperation());
	auto candidate = resolver.findOverload(
			mlir::rlc::builtinOperatorName<mlir::rlc::InitOp>(), deducedType);
	if (candidate == nullptr)
	{
		emitRemark("in construction expression");
		return mlir::failure();
	}

	if (isArray)
		rewriter.replaceOpWithNewOp<mlir::rlc::ArrayConstructOp>(
				*this, realType, candidate.getDefiningOp<mlir::rlc::FunctionOp>());
	else
		rewriter.replaceOpWithNewOp<mlir::rlc::ConstructOp>(
				*this, realType, candidate.getDefiningOp<mlir::rlc::FunctionOp>());

	return mlir::success();
}

mlir::LogicalResult mlir::rlc::CastOp::typeCheck(
		mlir::IRRewriter &rewriter,
		mlir::rlc::SymbolTable<mlir::Value> &table,
		mlir::rlc::RLCTypeConverter &conv)
{
	return mlir::success();
}

mlir::LogicalResult mlir::rlc::ActionStatement::typeCheck(
		mlir::IRRewriter &rewriter,
		mlir::rlc::SymbolTable<mlir::Value> &table,
		mlir::rlc::RLCTypeConverter &conv)
{
	llvm::SmallVector<mlir::Type> deducedTypes;

	for (auto &operand : getPrecondition().front().getArguments())
	{
		auto converted = conv.convertType(operand.getType());
		if (not converted)
		{
			getOperation()->emitRemark("in of argument of action statement");
			return mlir::failure();
		}
		operand.setType(converted);
	}

	for (auto result : getResultTypes())
	{
		auto deduced = conv.convertType(result);
		if (deduced == nullptr)
			return mlir::failure();
		deducedTypes.push_back(deduced);
	}

	llvm::SmallVector<mlir::Operation *, 4> ToCheck;
	for (auto &ops : getPrecondition().getOps())
	{
		ToCheck.push_back(&ops);
	}

	mlir::rlc::SymbolTable inner(&table);
	for (auto [name, res] :
			 llvm::zip(getDeclaredNames(), getPrecondition().getArguments()))
		inner.add(name.cast<mlir::StringAttr>(), res);

	for (auto *op : ToCheck)
	{
		if (mlir::rlc::typeCheck(*op, rewriter, inner, conv).failed())
			return mlir::failure();
	}

	rewriter.setInsertionPoint(*this);
	auto newDecl = rewriter.create<mlir::rlc::ActionStatement>(
			getLoc(), deducedTypes, getName(), getDeclaredNames());
	newDecl.getPrecondition().takeBody(getPrecondition());
	rewriter.replaceOp(getOperation(), newDecl.getResults());

	for (auto [name, res] :
			 llvm::zip(newDecl.getDeclaredNames(), newDecl.getResults()))
		table.add(name.cast<mlir::StringAttr>(), res);
	return mlir::success();
}

mlir::LogicalResult mlir::rlc::InitOp::typeCheck(
		mlir::IRRewriter &rewriter,
		mlir::rlc::SymbolTable<mlir::Value> &table,
		mlir::rlc::RLCTypeConverter &conv)
{
	llvm::SmallVector<mlir::Type> acceptable;
	acceptable.push_back(mlir::rlc::IntegerType::get(this->getContext()));
	acceptable.push_back(mlir::rlc::BoolType::get(this->getContext()));
	acceptable.push_back(mlir::rlc::FloatType::get(this->getContext()));
	return mlir::rlc::detail::typeCheckInteralOp(
			*this,
			rewriter,
			table,
			conv,
			acceptable,
			mlir::rlc::VoidType::get(this->getContext()));
}

mlir::LogicalResult mlir::rlc::MemberAccess::typeCheck(
		mlir::IRRewriter &rewriter,
		mlir::rlc::SymbolTable<mlir::Value> &table,
		mlir::rlc::RLCTypeConverter &conv)
{
	rewriter.replaceOpWithNewOp<mlir::rlc::MemberAccess>(
			*this, getValue(), getMemberIndex());
	return mlir::success();
}

mlir::LogicalResult mlir::rlc::UnresolvedMemberAccess::typeCheck(
		mlir::IRRewriter &rewriter,
		mlir::rlc::SymbolTable<mlir::Value> &table,
		mlir::rlc::RLCTypeConverter &conv)
{
	auto structType = getValue().getType().dyn_cast<mlir::rlc::EntityType>();
	if (structType == nullptr)
	{
		emitError("tried access member of non-entity type");
		return mlir::failure();
	}

	for (const auto &index : llvm::enumerate(structType.getFieldNames()))
	{
		if (index.value() != getMemberName())
			continue;

		rewriter.replaceOpWithNewOp<mlir::rlc::MemberAccess>(
				*this, getValue(), index.index());
		return mlir::success();
	}

	emitError(
			"no known member " + getMemberName() + " in struct " +
			structType.getName());
	return mlir::failure();
}

mlir::LogicalResult mlir::rlc::typeCheck(
		mlir::Operation &op,
		mlir::IRRewriter &rewriter,
		mlir::rlc::ValueTable &table,
		mlir::rlc::RLCTypeConverter &typeConverter)
{
	if (not op.hasTrait<mlir::rlc::TypeCheckable::Trait>())
	{
		op.emitOpError("does not implement type check");
		return mlir::failure();
	}

	rewriter.setInsertionPoint(&op);
	if (mlir::cast<mlir::rlc::TypeCheckable>(op)
					.typeCheck(rewriter, table, typeConverter)
					.failed())
		return mlir::failure();

	return mlir::LogicalResult::success();
}

mlir::LogicalResult mlir::rlc::FunctionOp::typeCheckFunctionDeclaration(
		mlir::IRRewriter &rewriter, mlir::rlc::RLCTypeConverter &converter)
{
	rewriter.setInsertionPoint(*this);
	auto scopedConverter = mlir::rlc::RLCTypeConverter(&converter);
	llvm::SmallVector<mlir::Type, 2> checkedTemplateParameters;
	for (auto parameter : getTemplateParameters())
	{
		auto unchecked = parameter.cast<mlir::TypeAttr>()
												 .getValue()
												 .cast<mlir::rlc::UncheckedTemplateParameterType>();

		auto checkedParameterType = converter.convertType(unchecked);
		if (not checkedParameterType)
		{
			emitRemark("in function definition");
			return mlir::failure();
		}
		checkedTemplateParameters.push_back(checkedParameterType);
		auto actualType =
				checkedParameterType.cast<mlir::rlc::TemplateParameterType>();
		scopedConverter.registerType(actualType.getName(), actualType);
	}

	auto deducedType = scopedConverter.convertType(getFunctionType());
	if (deducedType == nullptr)
	{
		emitRemark("in function declaration " + getUnmangledName());
		return mlir::failure();
	}
	assert(deducedType.isa<mlir::FunctionType>());

	auto newF = rewriter.create<mlir::rlc::FunctionOp>(
			getLoc(),
			getUnmangledName(),
			deducedType.cast<mlir::FunctionType>(),
			getArgNames(),
			checkedTemplateParameters);
	newF.getBody().takeBody(getBody());
	newF.getPrecondition().takeBody(getPrecondition());
	rewriter.eraseOp(*this);
	return mlir::success();
}
