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

namespace mlir::rlc
{
#define GEN_PASS_DEF_SERIALIZERLPASS
#include "rlc/dialect/Passes.inc"

	struct SerializeRLPass: impl::SerializeRLPassBase<SerializeRLPass>
	{
		using impl::SerializeRLPassBase<SerializeRLPass>::SerializeRLPassBase;

		void runOnOperation() override
		{
			for (auto op :
					 getOperation().getBody()->getOps<mlir::rlc::ClassDeclaration>())
			{
				auto type = op.getResult().getType().cast<mlir::rlc::ClassType>();
				*OS << "cls";
				if (not type.getExplicitTemplateParameters().empty())
				{
					*OS << "<";
					for (auto parameter :
							 llvm::drop_end(type.getExplicitTemplateParameters()))
					{
						parameter.cast<mlir::rlc::RLCSerializable>().rlc_serialize(
								*OS, SerializationContext());
						*OS << ", ";
					}
					type.getExplicitTemplateParameters()
							.back()
							.cast<mlir::rlc::RLCSerializable>()
							.rlc_serialize(*OS, SerializationContext());
					*OS << ">";
				}

				*OS << " " << type.getName() << ":\n";
				for (auto [name, type] :
						 llvm::zip(type.getFieldNames(), type.getBody()))
				{
					(*OS).indent(2);
					type.cast<mlir::rlc::RLCSerializable>().rlc_serialize(
							*OS, SerializationContext());
					*OS << " " << name << "\n";
				}
				if (type.getFieldNames().empty())
				{
					(*OS).indent(2);
					*OS << "pass\n";
				}
				*OS << "\n";
			}
		}
	};
}	 // namespace mlir::rlc
