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
#include "rlc/dialect/DebugInfo.hpp"

#include "llvm/BinaryFormat/Dwarf.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/Path.h"

namespace mlir::rlc
{

	static mlir::LLVM::DITypeAttr genBasicType(
			mlir::MLIRContext* context,
			mlir::StringAttr name,
			unsigned bitSize,
			unsigned decoding)
	{
		return mlir::LLVM::DIBasicTypeAttr::get(
				context, llvm::dwarf::DW_TAG_base_type, name, bitSize, decoding);
	}

	mlir::LLVM::DIFileAttr DebugInfoGenerator::getFileOfLoc(
			mlir::Location location) const
	{
		auto loc = location.cast<mlir::FileLineColLoc>();
		llvm::StringRef file = loc.getFilename();
		return mlir::LLVM::DIFileAttr::get(
				op.getContext(),
				llvm::sys::path::filename(file),
				llvm::sys::path::parent_path(file));
	}

	DebugInfoGenerator::DebugInfoGenerator(
			mlir::ModuleOp op,
			mlir::OpBuilder& rewriter,
			mlir::TypeConverter* converter,
			mlir::DataLayout* dl)
			: op(op), rewriter(rewriter), converter(converter), dl(dl)
	{
		compileUnit = LLVM::DICompileUnitAttr::get(
				DistinctAttr::create(UnitAttr::get(op.getContext())),
				llvm::dwarf::DW_LANG_C,
				getFileOfLoc(op.getLoc()),
				StringAttr::get(op.getContext(), "rlc_program"),
				/*isOptimized=*/true,
				LLVM::DIEmissionKind::Full);
	}

	static mlir::LLVM::DITypeAttr makePointerType(
			mlir::LLVM::DITypeAttr underlying)
	{
		return mlir::LLVM::DIDerivedTypeAttr::get(
				underlying.getContext(),
				llvm::dwarf::DW_TAG_pointer_type,
				mlir::StringAttr::get(underlying.getContext(), ""),
				underlying,
				64,
				/*alignInBits=*/0,
				/*offset=*/0,
				/*optional<address space>=*/std::nullopt,
				/*extra data=*/nullptr);
	}

	mlir::LLVM::DITypeAttr DebugInfoGenerator::getDIAttrOf(mlir::Type type) const
	{
		auto iter = typeToDITypeMap.find(type);
		if (iter != typeToDITypeMap.end())
			return (*iter).getSecond();
		auto attr = getDIAttrOfImpl(type);
		typeToDITypeMap[type] = attr;
		return attr;
	}

	mlir::LLVM::DITypeAttr DebugInfoGenerator::getDIAttrOfImpl(
			mlir::Type type) const
	{
		if (auto originalType = type.dyn_cast<mlir::rlc::IntegerType>())
			return genBasicType(
					op.getContext(),
					mlir::StringAttr::get(op.getContext(), "Int"),
					originalType.getSize(),
					llvm::dwarf::DW_ATE_signed);

		if (auto originalType = type.dyn_cast<mlir::rlc::FloatType>())
			return genBasicType(
					op.getContext(),
					mlir::StringAttr::get(op.getContext(), "Float"),
					64,
					llvm::dwarf::DW_ATE_float);

		if (auto originalType = type.dyn_cast<mlir::rlc::BoolType>())
			return genBasicType(
					op.getContext(),
					mlir::StringAttr::get(op.getContext(), "Bool"),
					8,
					llvm::dwarf::DW_ATE_boolean);

		if (auto originalType = type.dyn_cast<mlir::rlc::VoidType>())
			return nullptr;

		if (auto originalType = type.dyn_cast<mlir::FunctionType>())
		{
			// already account for the fact that functions signatures will be
			// rewritten to have the return paramater as first argument
			llvm::SmallVector<mlir::LLVM::DITypeAttr> elements({ nullptr });
			if (not originalType.getResults().empty() and
					not originalType.getResults()[0].isa<mlir::rlc::VoidType>())
				elements.push_back(getDIAttrOf(originalType.getResults()[0]));

			for (auto type : originalType.getInputs())
			{
				auto param = getDIAttrOf(type);
				if (not param)
					return param;
				elements.push_back(param);
			}

			return mlir::LLVM::DISubroutineTypeAttr::get(
					originalType.getContext(), llvm::dwarf::DW_CC_normal, elements);
		}

		if (auto originalType = type.dyn_cast<mlir::rlc::ReferenceType>())
		{
			auto underlying = getDIAttrOf(originalType.getUnderlying());
			if (!underlying)
				return nullptr;
			return makePointerType(underlying);
		}

		if (auto originalType = type.dyn_cast<mlir::rlc::OwningPtrType>())
		{
			auto underlying = getDIAttrOf(originalType.getUnderlying());
			if (!underlying)
				return nullptr;
			return makePointerType(underlying);
		}
		if (auto originalType = type.dyn_cast<mlir::rlc::ArrayType>())
		{
			llvm::SmallVector<mlir::LLVM::DINodeAttr> elements;

			auto underlying = getDIAttrOf(originalType.getUnderlying());
			if (!underlying)
				return nullptr;
			auto intTy = mlir::IntegerType::get(originalType.getContext(), 64);
			auto countAttr = mlir::IntegerAttr::get(
					intTy, llvm::APInt(64, originalType.getArraySize()));
			auto lowerAttr = mlir::IntegerAttr::get(intTy, llvm::APInt(64, 1));
			auto subrangeTy = mlir::LLVM::DISubrangeAttr::get(
					originalType.getContext(),
					countAttr,
					lowerAttr,
					/*upperBound=*/nullptr,
					/*stride=*/nullptr);
			elements.push_back(subrangeTy);

			return mlir::LLVM::DICompositeTypeAttr::get(
					type.getContext(),
					llvm::dwarf::DW_TAG_array_type,
					/*recursive_id=*/{},
					/*name=*/nullptr,
					/*file=*/nullptr,
					/*line=*/0,
					/*scope=*/nullptr,
					underlying,
					mlir::LLVM::DIFlags::Zero,
					/*sizeInBits=*/0,
					/*alignInBits=*/0,
					elements,
					/*dataLocation=*/nullptr,
					/*rank=*/nullptr,
					/*allocated=*/nullptr,
					/*associated=*/nullptr);
		}

		if (auto originalType = type.dyn_cast<mlir::rlc::AlternativeType>())
		{
			llvm::SmallVector<mlir::LLVM::DINodeAttr> elements;

			size_t max_size = 0;
			auto intType = mlir::IntegerType::get(originalType.getContext(), 64);
			size_t intAlign = dl->getTypePreferredAlignment(intType) * 8;
			size_t max_align = intAlign;
			for (auto type : originalType.getUnderlying())
			{
				auto llvmType = converter->convertType(type);
				auto underlying = getDIAttrOf(type);
				if (!underlying)
					return nullptr;

				size_t align = dl->getTypePreferredAlignment(llvmType) * 8;
				size_t typeSize = dl->getTypeSizeInBits(llvmType);
				max_align = std::max(align, max_align);
				max_size = std::max(max_size, typeSize);

				auto member = mlir::LLVM::DIDerivedTypeAttr::get(
						underlying.getContext(),
						llvm::dwarf::DW_TAG_member,
						mlir::StringAttr::get(underlying.getContext(), ""),
						underlying,
						typeSize,
						/*alignInBits=*/align,
						/*offset=*/0,
						/*optional<address space>=*/std::nullopt,
						/*extra data=*/nullptr);

				elements.push_back(member);
			}

			auto diUnion = mlir::LLVM::DICompositeTypeAttr::get(
					type.getContext(),
					llvm::dwarf::DW_TAG_union_type,
					/*recursive_id=*/{},
					/*name=*/
					mlir::StringAttr::get(type.getContext(), ""),
					/*file=*/nullptr,
					/*line=*/0,
					/*scope=*/nullptr,
					nullptr,
					mlir::LLVM::DIFlags::Zero,
					/*sizeInBits=*/max_size,
					/*alignInBits=*/max_align,
					elements,
					/*dataLocation=*/nullptr,
					/*rank=*/nullptr,
					/*allocated=*/nullptr,
					/*associated=*/nullptr);

			auto member = mlir::LLVM::DIDerivedTypeAttr::get(
					diUnion.getContext(),
					llvm::dwarf::DW_TAG_member,
					mlir::StringAttr::get(diUnion.getContext(), ""),
					diUnion,
					max_size,
					/*alignInBits=*/max_align,
					/*offset=*/0,
					/*optional<address space>=*/std::nullopt,
					/*extra data=*/nullptr);

			auto activeIndex = genBasicType(
					op.getContext(),
					mlir::StringAttr::get(op.getContext(), "Int"),
					64,
					llvm::dwarf::DW_ATE_signed);

			uint64_t padding = (intAlign - (max_size % intAlign)) % intAlign;

			uint64_t total_size = max_size + padding + dl->getTypeSizeInBits(intType);

			auto activeIndexMember = mlir::LLVM::DIDerivedTypeAttr::get(
					diUnion.getContext(),
					llvm::dwarf::DW_TAG_member,
					mlir::StringAttr::get(diUnion.getContext(), ""),
					activeIndex,
					max_size,
					/*alignInBits=*/intAlign,
					/*offset=*/max_size + padding,
					/*optional<address space>=*/std::nullopt,
					/*extra data=*/nullptr);

			return mlir::LLVM::DICompositeTypeAttr::get(
					type.getContext(),
					llvm::dwarf::DW_TAG_structure_type,
					/*recursive_id=*/{},
					/*name=*/
					mlir::StringAttr::get(type.getContext(), ""),
					/*file=*/nullptr,
					/*line=*/0,
					/*scope=*/nullptr,
					nullptr,
					mlir::LLVM::DIFlags::Zero,
					/*sizeInBits=*/total_size,
					/*alignInBits=*/max_align,
					{ member, activeIndexMember },
					/*dataLocation=*/nullptr,
					/*rank=*/nullptr,
					/*allocated=*/nullptr,
					/*associated=*/nullptr);
		}

		if (auto originalType = type.dyn_cast<mlir::rlc::ClassType>())
		{
			llvm::SmallVector<mlir::LLVM::DINodeAttr> elements;
			auto converted = converter->convertType(originalType)
													 .cast<mlir::LLVM::LLVMStructType>();
			size_t offset = 0;

			for (auto [type, llvmType] :
					 llvm::zip(originalType.getMembers(), converted.getBody()))
			{
				auto underlying = getDIAttrOf(type.getType());
				if (!underlying)
					return nullptr;

				size_t alignment = dl->getTypePreferredAlignment(llvmType) * 8;
				uint64_t padding = (alignment - (offset % alignment)) % alignment;
				offset += padding;
				size_t typeSize = dl->getTypeSizeInBits(llvmType);

				auto member = mlir::LLVM::DIDerivedTypeAttr::get(
						underlying.getContext(),
						llvm::dwarf::DW_TAG_member,
						mlir::StringAttr::get(underlying.getContext(), type.getName()),
						underlying,
						typeSize,
						/*alignInBits=*/alignment,
						/*offset=*/offset,
						/*optional<address space>=*/std::nullopt,
						/*extra data=*/nullptr);
				offset += typeSize;

				elements.push_back(member);
			}

			return mlir::LLVM::DICompositeTypeAttr::get(
					type.getContext(),
					llvm::dwarf::DW_TAG_structure_type,
					/*recursive_id=*/{},
					/*name=*/
					mlir::StringAttr::get(type.getContext(), originalType.getName()),
					/*file=*/nullptr,
					/*line=*/0,
					/*scope=*/nullptr,
					nullptr,
					mlir::LLVM::DIFlags::Zero,
					/*sizeInBits=*/dl->getTypeSizeInBits(converted),
					/*alignInBits=*/0,
					elements,
					/*dataLocation=*/nullptr,
					/*rank=*/nullptr,
					/*allocated=*/nullptr,
					/*associated=*/nullptr);
		}

		if (auto originalType = type.dyn_cast<mlir::rlc::StringLiteralType>())
		{
			auto charType = genBasicType(
					op.getContext(),
					mlir::StringAttr::get(op.getContext(), "Char"),
					8,
					llvm::dwarf::DW_ATE_signed_char);
			return makePointerType(charType);
		}

		// type.dump();
		// assert(false && "Unhandled");
		return nullptr;
	}

	mlir::LLVM::DISubprogramAttr DebugInfoGenerator::makeFunctionAttr(
			llvm::StringRef name,
			llvm::StringRef mangledName,
			mlir::LLVM::DISubroutineTypeAttr type,
			bool declaration,
			mlir::Location location,
			bool artificial) const
	{
		LLVM::DICompileUnitAttr compileUnitAttr = compileUnit;

		StringAttr funcNameAttr = rewriter.getStringAttr(name);
		// Only definitions need a distinct identifier and a compilation unit.
		DistinctAttr id;
		auto subprogramFlags = LLVM::DISubprogramFlags::Optimized;
		if (!declaration)
		{
			id = mlir::DistinctAttr::create(mlir::UnitAttr::get(op.getContext()));
			subprogramFlags = subprogramFlags | LLVM::DISubprogramFlags::Definition;
		}
		else
		{
			compileUnitAttr = {};
		}

		auto loc = location.cast<mlir::FileLineColLoc>();
		auto file = getFileOfLoc(location);
		auto subprogramAttr = LLVM::DISubprogramAttr::get(
				op.getContext(),
				id,
				compileUnitAttr,
				file,
				funcNameAttr,
				rewriter.getStringAttr(mangledName),
				file,
				/*line=*/loc.getLine(),
				/*scopeline=*/loc.getLine(),
				subprogramFlags,
				type);

		return subprogramAttr;
	}

	mlir::LLVM::DISubprogramAttr DebugInfoGenerator::getFunctionAttr(
			mlir::rlc::FlatFunctionOp fun) const
	{
		auto iter = functionToSubProgram.find(fun);
		if (iter != functionToSubProgram.end())
			return iter->getSecond();

		auto subroutineTypeAttr =
				getDIAttrOf(fun.getType()).dyn_cast<mlir::LLVM::DISubroutineTypeAttr>();
		if (not subroutineTypeAttr)
			subroutineTypeAttr = LLVM::DISubroutineTypeAttr::get(
					op.getContext(), llvm::dwarf::DW_CC_normal, {});

		auto subprogramAttr = makeFunctionAttr(
				fun.getUnmangledName(),
				fun.getMangledName(),
				subroutineTypeAttr,
				fun.isDeclaration(),
				fun.getLoc(),
				false);
		functionToSubProgram[fun] = subprogramAttr;
		return subprogramAttr;
	}
	mlir::LLVM::DISubprogramAttr DebugInfoGenerator::getFunctionAttr(
			mlir::LLVM::LLVMFuncOp fun,
			llvm::StringRef unmangledName,
			mlir::FunctionType rlcType,
			bool artificial) const
	{
		auto iter = functionToSubProgram.find(fun);
		if (iter != functionToSubProgram.end())
			return iter->getSecond();
		if (unmangledName == "")
			unmangledName = fun.getSymName();

		auto subroutineTypeAttr = LLVM::DISubroutineTypeAttr::get(
				op.getContext(), llvm::dwarf::DW_CC_normal, {});

		if (rlcType)
		{
			auto maybeType =
					getDIAttrOf(rlcType).dyn_cast<mlir::LLVM::DISubroutineTypeAttr>();
			if (maybeType)
				subroutineTypeAttr = maybeType;
		}

		auto subprogramAttr = makeFunctionAttr(
				unmangledName,
				fun.getName(),
				subroutineTypeAttr,
				fun.isDeclaration(),
				fun.getLoc(),
				artificial);
		functionToSubProgram[fun] = subprogramAttr;
		return subprogramAttr;
	}

	mlir::LLVM::DILocalVariableAttr DebugInfoGenerator::getArgument(
			mlir::Value value,
			mlir::LLVM::DISubprogramAttr functionAttr,
			llvm::StringRef name,
			mlir::FileLineColLoc location) const
	{
		auto DIType = getDIAttrOf(value.getType());
		if (not DIType)
		{
			return nullptr;
		}
		auto pointerTo = makePointerType(DIType);
		return mlir::LLVM::DILocalVariableAttr::get(
				functionAttr,
				name,
				getFileOfLoc(location),
				location.getLine(),
				0,
				0,
				DIType);
	}

	mlir::LLVM::DILocalVariableAttr DebugInfoGenerator::getLocalVar(
			mlir::Value value,
			llvm::StringRef name,
			mlir::FileLineColLoc location) const
	{
		assert(value.getDefiningOp());
		auto DIType = getDIAttrOf(value.getType());
		if (not DIType)
		{
			return nullptr;
		}
		mlir::LLVM::DISubprogramAttr funAttr;

		if (auto fun = (*value.getDefiningOp())
											 .getParentOfType<mlir::rlc::FlatFunctionOp>())
		{
			funAttr = getFunctionAttr(fun);
		}
		else if (
				auto fun =
						(*value.getDefiningOp()).getParentOfType<mlir::LLVM::LLVMFuncOp>())
		{
			funAttr = getFunctionAttr(fun);
		}
		return mlir::LLVM::DILocalVariableAttr::get(
				funAttr,
				name,
				getFileOfLoc(location),
				location.getLine(),
				0,
				0,
				DIType);
	}

}	 // namespace mlir::rlc
