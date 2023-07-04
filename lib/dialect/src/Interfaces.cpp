#include "rlc/dialect/Interfaces.hpp"

mlir::LogicalResult mlir::rlc::TypeCheckable::typeCheck(
		mlir::rlc::ModuleBuilder& builder)
{
	mlir::rlc::TypeCheckable::Concept c;
	return getInterfaceFor(this->getOperation())
			->typeCheck(&c, this->getOperation(), builder);
}
