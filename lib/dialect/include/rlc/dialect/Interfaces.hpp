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
#pragma once

#include "mlir/Transforms/DialectConversion.h"
#include "rlc/dialect/Dialect.h"
#include "rlc/dialect/SymbolTable.h"

namespace mlir
{
	class IRRewriter;
}

namespace mlir::rlc
{
	class SerializationContext;
	class ConstraintsLattice;
	class SourceRangeAttr;
	class ConstraintsAnalysis;
}	 // namespace mlir::rlc

#include "rlc/dialect/Interfaces.inc"
