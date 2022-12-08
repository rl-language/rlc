#include "rlc/dialect/SymbolTable.h"

mlir::Value mlir::rlc::findOverload(
		mlir::Operation& errorEmitter,
		ValueTable& table,
		llvm::StringRef name,
		mlir::TypeRange arguments)
{
	llvm::SmallVector<mlir::Value> matching;
	for (auto candidate : table.get(name))
	{
		if (not candidate.getType().isa<mlir::FunctionType>())
			continue;
		if (candidate.getType().cast<mlir::FunctionType>().getInputs() == arguments)
			matching.push_back(candidate);
	}

	assert(matching.size() <= 1);
	if (not matching.empty())
		return matching.front();

	errorEmitter.emitError("could not find matching function " + name);
	for (auto candidate : table.get(name))
	{
		if (not candidate.getType().isa<mlir::FunctionType>())
			continue;
		candidate.getDefiningOp()->emitRemark("candidate");
	}
	return nullptr;
}

llvm::SmallVector<mlir::Value, 2> mlir::rlc::findOverloads(
		ValueTable& table, llvm::StringRef name, mlir::TypeRange arguments)
{
	llvm::SmallVector<mlir::Value> matching;
	for (auto candidate : table.get(name))
	{
		if (not candidate.getType().isa<mlir::FunctionType>())
			continue;
		if (candidate.getType().cast<mlir::FunctionType>().getInputs() == arguments)
			matching.push_back(candidate);
	}
	return matching;
}
