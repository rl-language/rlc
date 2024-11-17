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

#include "rlc/dialect/Attrs.hpp"

#include "llvm/ADT/TypeSwitch.h"
#include "llvm/Support/SMLoc.h"
#include "rlc/dialect/Dialect.h"

#define GET_ATTRDEF_CLASSES
#include "Attrs.inc"

void mlir::rlc::RLCDialect::registerAttrs()
{
	addAttributes<
#define GET_ATTRDEF_LIST
#include "Attrs.inc"
			>();
}

mlir::Attribute mlir::rlc::RLCDialect::parseAttribute(
		mlir::DialectAsmParser &parser, mlir::Type type) const
{
	::llvm::StringRef mnemonic;
	::mlir::Attribute genAttr;
	auto parseResult = generatedAttributeParser(parser, &mnemonic, type, genAttr);
	if (parseResult.has_value())
		return genAttr;
	return nullptr;
}

void mlir::rlc::RLCDialect::printAttribute(
		mlir::Attribute attribute, mlir::DialectAsmPrinter &printer) const
{
	if (::mlir::succeeded(generatedAttributePrinter(attribute, printer)))
		return;
}
