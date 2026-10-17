/*
Copyright 2026 Massimo Fioravanti

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
#define GEN_PASS_DEF_WARNUNUSEDPASS
#include "rlc/dialect/Passes.inc"

	struct WarnUnusedPass: impl::WarnUnusedPassBase<WarnUnusedPass>
	{
		using impl::WarnUnusedPassBase<WarnUnusedPass>::WarnUnusedPassBase;

		void runOnOperation() override
		{
			for (auto& global : getOperation().getBodyRegion().front())
			{
				if (not llvm::isa<mlir::rlc::FunctionOp>(global))
					continue;
				if (isSynthetic(&global))
					continue;

				if (auto casted = llvm::dyn_cast<mlir::rlc::FunctionOp>(global);
						not casted or
						(casted.getUnmangledName() == "main" and
						 casted.getArgumentTypes().empty()) or
						casted.getUnmangledName() == "init" or
						casted.getUnmangledName() == "drop" or
						casted.getUnmangledName() == "assign" or
						casted.getUnmangledName() == "equal" or
						casted.getUnmangledName() == "score" or
						casted.getUnmangledName() == "not_equal" or
						casted.getUnmangledName() == "get_num_players" or
						casted.getUnmangledName() == "fuzz" or
						casted.getUnmangledName() == "get_current_player" or
						casted.getUnmangledName() == "pretty_print")
					continue;
				if (auto casted =
								mlir::dyn_cast<mlir::FileLineColLoc>((global.getLoc()));
						casted and casted.getFilename().getValue().contains("stdlib"))
					continue;

				if (llvm::none_of(
								global.getResults(), [](const mlir::OpResult& result) {
									return result.hasNUsesOrMore(1);
								}))
					auto _ = mlir::rlc::logRemark(&global, "Has no users");
			}
		}
	};
}	 // namespace mlir::rlc
