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
		mlir::TypeConverter &converter)
{
	for (auto *op : ops(getBody()))
		if (mlir::rlc::typeCheck(*op, rewriter, table, converter).failed())
			return mlir::failure();

	return mlir::success();
}

mlir::LogicalResult mlir::rlc::DeclarationStatement::typeCheck(
		mlir::IRRewriter &rewriter,
		mlir::rlc::SymbolTable<mlir::Value> &table,
		mlir::TypeConverter &conv)
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
		mlir::TypeConverter &conv)
{
	rewriter.replaceOpWithNewOp<mlir::rlc::ArrayAccess>(
			*this, getValue(), getMemberIndex());
	return mlir::success();
}

mlir::LogicalResult mlir::rlc::Yield::typeCheck(
		mlir::IRRewriter &rewriter,
		mlir::rlc::SymbolTable<mlir::Value> &table,
		mlir::TypeConverter &conv)
{
	return mlir::success();
}

mlir::LogicalResult mlir::rlc::ReturnStatement::typeCheck(
		mlir::IRRewriter &rewriter,
		mlir::rlc::SymbolTable<mlir::Value> &table,
		mlir::TypeConverter &conv)
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
		mlir::TypeConverter &conv)
{
	bool usedByCall = llvm::any_of(
			getResult().getUsers(), [](const mlir::OpOperand &operand) -> bool {
				return mlir::isa<mlir::rlc::CallOp>(*operand.getOwner());
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
		mlir::TypeConverter &conv)
{
	return mlir::success();
}

mlir::LogicalResult mlir::rlc::StatementList::typeCheck(
		mlir::IRRewriter &rewriter,
		mlir::rlc::SymbolTable<mlir::Value> &table,
		mlir::TypeConverter &conv)
{
	mlir::rlc::SymbolTable innerTable(&table);

	for (auto *op : ops(getBody()))
		if (mlir::rlc::typeCheck(*op, rewriter, innerTable, conv).failed())
			return mlir::failure();

	return mlir::success();
}

mlir::LogicalResult mlir::rlc::IfStatement::typeCheck(
		mlir::IRRewriter &rewriter,
		mlir::rlc::SymbolTable<mlir::Value> &table,
		mlir::TypeConverter &conv)
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
	for (auto *op : ops(getTrueBranch()))
		if (mlir::rlc::typeCheck(*op, rewriter, innerTable, conv).failed())
			return mlir::failure();

	mlir::rlc::SymbolTable innerTable2(&table);
	for (auto *op : ops(getElseBranch()))
		if (mlir::rlc::typeCheck(*op, rewriter, innerTable, conv).failed())
			return mlir::failure();

	return mlir::success();
}

mlir::LogicalResult mlir::rlc::CallOp::typeCheck(
		mlir::IRRewriter &rewriter,
		mlir::rlc::SymbolTable<mlir::Value> &table,
		mlir::TypeConverter &conv)
{
	auto callee = getCallee().getDefiningOp<mlir::rlc::UnresolvedReference>();
	mlir::Value toCall = getCallee();
	if (callee != nullptr)
	{
		auto candidate = mlir::rlc::findOverload(
				*getOperation(), table, callee.getName(), getArgs().getType());
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
		mlir::TypeConverter &conv)
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
		mlir::TypeConverter &conv)
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

	auto candidate = findOverload(*getOperation(), table, "_init", deducedType);
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
		mlir::TypeConverter &conv)
{
	return mlir::success();
}

mlir::LogicalResult mlir::rlc::ActionStatement::typeCheck(
		mlir::IRRewriter &rewriter,
		mlir::rlc::SymbolTable<mlir::Value> &table,
		mlir::TypeConverter &conv)
{
	llvm::SmallVector<mlir::Type> deducedTypes;

	for (auto result : getResultTypes())
	{
		auto deduced = conv.convertType(result);
		if (deduced == nullptr)
			return mlir::failure();
		deducedTypes.push_back(deduced);
	}

	auto newDecl = rewriter.replaceOpWithNewOp<mlir::rlc::ActionStatement>(
			*this, deducedTypes, getName(), getDeclaredNames());

	for (auto [name, res] :
			 llvm::zip(newDecl.getDeclaredNames(), newDecl.getResults()))
		table.add(name.cast<mlir::StringAttr>(), res);
	return mlir::success();
}

mlir::LogicalResult mlir::rlc::InitOp::typeCheck(
		mlir::IRRewriter &rewriter,
		mlir::rlc::SymbolTable<mlir::Value> &table,
		mlir::TypeConverter &conv)
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
		mlir::TypeConverter &conv)
{
	rewriter.replaceOpWithNewOp<mlir::rlc::MemberAccess>(
			*this, getValue(), getMemberIndex());
	return mlir::success();
}

mlir::LogicalResult mlir::rlc::UnresolvedMemberAccess::typeCheck(
		mlir::IRRewriter &rewriter,
		mlir::rlc::SymbolTable<mlir::Value> &table,
		mlir::TypeConverter &conv)
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
		mlir::TypeConverter &typeConverter)
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
