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
#include "rlc/python/Interfaces.hpp"

mlir::LogicalResult mlir::rlc::python::EmitPython::emit(
		llvm::raw_ostream& OS, SerializationContext& context)
{
	mlir::rlc::python::EmitPython::Concept c;
	return getInterfaceFor(this->getOperation())
			->emit(&c, this->getOperation(), OS, context);
}
