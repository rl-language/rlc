#include "rlc/python/Interfaces.hpp"

mlir::LogicalResult mlir::rlc::python::EmitPython::emit(
		llvm::raw_ostream& OS, SerializationContext& context)
{
	mlir::rlc::python::EmitPython::Concept c;
	return getInterfaceFor(this->getOperation())
			->emit(&c, this->getOperation(), OS, context);
}
