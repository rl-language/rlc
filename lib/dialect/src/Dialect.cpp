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

#include "rlc/dialect/Dialect.h"

#include "Dialect.inc"
#include "mlir/Interfaces/FoldInterfaces.h"
#include "rlc/dialect/Operations.hpp"
#include "rlc/dialect/Types.hpp"

class TypeAliasASMInterface: public mlir::OpAsmDialectInterface
{
	mutable llvm::DenseMap<mlir::rlc::ClassType, int> counter;
	mutable int index = 0;

	public:
	using mlir::OpAsmDialectInterface::OpAsmDialectInterface;

	AliasResult getAlias(mlir::Type type, llvm::raw_ostream &OS) const final
	{
		if (auto casted = type.dyn_cast<mlir::rlc::ClassType>())
		{
			OS << casted.mangledName();
			if (counter.find(casted) == counter.end())
			{
				counter[casted] = index;
			}

			OS << "_" << counter[casted];
			return AliasResult::FinalAlias;
		}
		if (auto casted = type.dyn_cast<mlir::rlc::TraitMetaType>())
		{
			OS << "trait_" << casted.getName();
			return AliasResult::FinalAlias;
		}

		return AliasResult::NoAlias;
	}
};

struct RLCDialectFoldInterface: public mlir::DialectFoldInterface
{
	using DialectFoldInterface::DialectFoldInterface;

	virtual bool shouldMaterializeInto(mlir::Region *region) const override
	{
		return true;
	}
};

void mlir::rlc::RLCDialect::initialize()
{
	registerTypes();
	registerAttrs();
	registerOperations();
	addInterfaces<TypeAliasASMInterface>();
	addInterfaces<RLCDialectFoldInterface>();
}

static mlir::Type attrToRLCType(mlir::Attribute attr)
{
	if (auto casted = attr.dyn_cast<mlir::IntegerAttr>())
	{
		if (casted.getType() ==
				mlir::IntegerType::get(
						attr.getContext(),
						1,
						mlir::IntegerType::SignednessSemantics::Signless))
			return mlir::rlc::BoolType::get(attr.getContext());
		return mlir::rlc::IntegerType::get(
				attr.getContext(),
				casted.getType().dyn_cast<mlir::IntegerType>().getWidth());
	}
	if (auto casted = attr.dyn_cast<mlir::BoolAttr>())
	{
		return mlir::rlc::BoolType::get(attr.getContext());
	}
	if (auto casted = attr.dyn_cast<mlir::FloatAttr>())
	{
		assert(casted.getType().dyn_cast<mlir::FloatType>().getWidth() == 64);
		return mlir::rlc::FloatType::get(attr.getContext());
	}

	if (auto casted = attr.dyn_cast<mlir::ArrayAttr>())
	{
		return mlir::rlc::ArrayType::get(
				attr.getContext(), attrToRLCType(casted.getValue()[0]), casted.size());
	}

	assert(false);
	return nullptr;
}

mlir::Operation *mlir::rlc::RLCDialect::materializeConstant(
		OpBuilder &builder, Attribute value, Type type, Location loc)
{
	if (auto boolAttr = value.dyn_cast<mlir::BoolAttr>())
	{
		if (type.isa<mlir::rlc::BoolType>())
		{
			return builder.create<mlir::rlc::Constant>(loc, boolAttr.getValue());
		}
	}
	if (auto intAttr = value.dyn_cast<mlir::IntegerAttr>())
	{
		if (type.isa<mlir::rlc::IntegerType>())
		{
			return builder.create<mlir::rlc::Constant>(loc, type, intAttr);
		}
	}
	if (auto floatAttr = value.dyn_cast<mlir::FloatAttr>())
	{
		if (type.isa<mlir::rlc::FloatType>())
		{
			return builder.create<mlir::rlc::Constant>(loc, type, floatAttr);
		}
	}

	if (auto array = value.dyn_cast<mlir::ArrayAttr>())
	{
		assert(not array.empty());
		return builder.create<mlir::rlc::Constant>(
				loc, attrToRLCType(array), array);
	}

	return nullptr;
}

mlir::LogicalResult mlir::rlc::logRemark(mlir::Operation *op, llvm::Twine twine)
{
	op->getContext()->getDiagEngine().emit(
			op->getLoc(), mlir::DiagnosticSeverity::Remark)
			<< twine;
	return mlir::failure();
}

mlir::LogicalResult mlir::rlc::logError(mlir::Operation *op, llvm::Twine twine)
{
	op->getContext()->getDiagEngine().emit(
			op->getLoc(), mlir::DiagnosticSeverity::Error)
			<< twine;
	return mlir::failure();
}
