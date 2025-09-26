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

	AliasResult getAlias(mlir::Attribute type, llvm::raw_ostream &OS) const final
	{
		if (auto casted = mlir::dyn_cast<mlir::rlc::ShugarizedTypeAttr>(type))
		{
			OS << "shugar_type";
			return AliasResult::FinalAlias;
		}
		if (auto casted = mlir::dyn_cast<mlir::rlc::FunctionInfoAttr>(type))
		{
			OS << "fun_info";
			return AliasResult::FinalAlias;
		}
		if (auto casted = mlir::dyn_cast<mlir::rlc::ClassFieldAttr>(type))
		{
			OS << "field_info";
			return AliasResult::FinalAlias;
		}

		return AliasResult::NoAlias;
	}

	AliasResult getAlias(mlir::Type type, llvm::raw_ostream &OS) const final
	{
		if (auto casted = mlir::dyn_cast<mlir::rlc::ClassType>(type))
		{
			OS << casted.mangledName();
			if (counter.find(casted) == counter.end())
			{
				counter[casted] = index;
			}

			OS << "_" << counter[casted];
			return AliasResult::FinalAlias;
		}
		if (auto casted = mlir::dyn_cast<mlir::rlc::TraitMetaType>(type))
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
	if (auto casted = mlir::dyn_cast<mlir::IntegerAttr>(attr))
	{
		if (casted.getType() ==
				mlir::IntegerType::get(
						attr.getContext(),
						1,
						mlir::IntegerType::SignednessSemantics::Signless))
			return mlir::rlc::BoolType::get(attr.getContext());
		return mlir::rlc::IntegerType::get(
				attr.getContext(),
				mlir::dyn_cast<mlir::IntegerType>(casted.getType()).getWidth());
	}
	if (auto casted = mlir::dyn_cast<mlir::BoolAttr>(attr))
	{
		return mlir::rlc::BoolType::get(attr.getContext());
	}
	if (auto casted = mlir::dyn_cast<mlir::FloatAttr>(attr))
	{
		assert(mlir::dyn_cast<mlir::FloatType>(casted.getType()).getWidth() == 64);
		return mlir::rlc::FloatType::get(attr.getContext());
	}

	if (auto casted = mlir::dyn_cast<mlir::ArrayAttr>(attr))
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
	if (auto boolAttr = mlir::dyn_cast<mlir::BoolAttr>(value))
	{
		if (mlir::isa<mlir::rlc::BoolType>(type))
		{
			return builder.create<mlir::rlc::Constant>(loc, boolAttr.getValue());
		}
	}
	if (auto intAttr = mlir::dyn_cast<mlir::IntegerAttr>(value))
	{
		if (mlir::isa<mlir::rlc::IntegerType>(type))
		{
			return builder.create<mlir::rlc::Constant>(loc, type, intAttr);
		}
	}
	if (auto floatAttr = mlir::dyn_cast<mlir::FloatAttr>(value))
	{
		if (mlir::isa<mlir::rlc::FloatType>(type))
		{
			return builder.create<mlir::rlc::Constant>(loc, type, floatAttr);
		}
	}

	if (auto array = mlir::dyn_cast<mlir::ArrayAttr>(value))
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
