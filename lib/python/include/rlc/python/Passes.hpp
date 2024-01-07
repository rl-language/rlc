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
#pragma once

#include "mlir/Pass/Pass.h"
#include "rlc/python/Dialect.h"

namespace mlir::python
{
#define GEN_PASS_DECL
#include "rlc/python/Passes.inc"

#define GEN_PASS_REGISTRATION
#include "rlc/python/Passes.inc"

}	 // namespace mlir::python
