#include "rlc/dialect/Interfaces.hpp"

mlir::LogicalResult mlir::rlc::TypeCheckable::typeCheck(
		mlir::IRRewriter &rew,
		mlir::rlc::SymbolTable<mlir::Value> &table,
		mlir::rlc::RLCTypeConverter &conv)
{
	mlir::rlc::TypeCheckable::Concept c;
	return getInterfaceFor(this->getOperation())
			->typeCheck(&c, this->getOperation(), rew, table, conv);
}
