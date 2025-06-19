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
#include "llvm/ADT/TypeSwitch.h"
#include "mlir/IR/BuiltinDialect.h"
#include "rlc/dialect/Operations.hpp"
#include "rlc/dialect/Passes.hpp"

namespace
{
	bool areConsecutive(mlir::Location l, mlir::Location r)
	{
		auto first = mlir::dyn_cast<mlir::FileLineColLoc>(l);
		auto second = mlir::dyn_cast<mlir::FileLineColLoc>(r);
		if (not first or not second)
			return false;
		return first.getLine() + 1 == second.getLine();
	}

	void reinjectMemberFunctions(
			mlir::ModuleOp op, mlir::rlc::ModuleBuilder& builder)
	{
		mlir::IRRewriter rewriter(op);
		mlir::DenseMap<mlir::Type, mlir::rlc::ClassDeclaration> typeToClass;
		mlir::DenseMap<mlir::Type, mlir::rlc::EnumDeclarationOp> typeToEnum;
		mlir::DenseMap<llvm::StringRef, mlir::Type> nameToType;
		llvm::SmallVector<mlir::rlc::FunctionOp, 2> funs;
		for (auto fun : op.getOps<mlir::rlc::FunctionOp>())
			if (fun.getIsMemberFunction())
				funs.push_back(fun);

		for (auto cls : op.getOps<mlir::rlc::ClassDeclaration>())
		{
			typeToClass[cls.getDeclaredType()] = cls;
			rewriter.createBlock(&cls.getBody());
			nameToType[cls.getName()] = cls.getDeclaredType();
		}

		for (auto enumT : op.getOps<mlir::rlc::EnumDeclarationOp>())
		{
			typeToEnum[nameToType[enumT.getName()]] = enumT;
		}

		for (auto fun : funs)
		{
			auto selfType = fun.getArgumentTypes()[0];
			auto decl = typeToClass[selfType];
			if (typeToEnum.contains(selfType))
			{
				auto casted = typeToEnum[selfType];
				fun->moveBefore(
						&casted.getBody().front(), casted.getBody().front().end());
			}
			else
			{
				auto casted = mlir::cast<mlir::rlc::ClassDeclaration>(decl);
				fun->moveBefore(
						&casted.getBody().front(), casted.getBody().front().end());
			}
		}
	}
}	 // namespace

namespace mlir::rlc
{
#define GEN_PASS_DEF_SERIALIZERLPASS
#include "rlc/dialect/Passes.inc"

	struct SerializeRLPass: impl::SerializeRLPassBase<SerializeRLPass>
	{
		using impl::SerializeRLPassBase<SerializeRLPass>::SerializeRLPassBase;

		void runOnOperation() override
		{
			mlir::rlc::ModuleBuilder builder(getOperation());
			mlir::rlc::SerializationContext ctx(builder);
			reinjectMemberFunctions(getOperation(), builder);

			for (auto& op : getOperation().getOps())
			{
				if (mlir::rlc::isSynthetic(&op))
					continue;
				auto loc = mlir::cast<mlir::FileLineColLoc>(op.getLoc());
				if (not file_to_serialize.empty() and
						loc.getFilename() != file_to_serialize)
					continue;
				if (auto casted = mlir::dyn_cast<mlir::rlc::Serializable>(op))
				{
					casted.serialize(*OS, ctx);
					if (op.getNextNode() == nullptr or
							not areConsecutive(casted.getLoc(), op.getNextNode()->getLoc()))
						*OS << "\n";
				}
			}
		}
	};
}	 // namespace mlir::rlc
