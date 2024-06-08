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
#include "rlc/dialect/Operations.hpp"
#include "rlc/dialect/Passes.hpp"

namespace mlir::rlc
{

#define GEN_PASS_DEF_SORTTYPEDECLARATIONSPASS
#include "rlc/dialect/Passes.inc"

	struct SortTypeDeclarationsPass
			: impl::SortTypeDeclarationsPassBase<SortTypeDeclarationsPass>
	{
		using impl::SortTypeDeclarationsPassBase<
				SortTypeDeclarationsPass>::SortTypeDeclarationsPassBase;

		void runOnOperation() override
		{
			llvm::SmallVector<mlir::rlc::ClassDeclaration, 2> decls;
			llvm::DenseMap<mlir::Type, mlir::rlc::ClassDeclaration>
					declaredTypeToDeclaration;
			for (auto decl : getOperation().getOps<mlir::rlc::ClassDeclaration>())
			{
				decls.push_back(decl);
				declaredTypeToDeclaration[decl.getResult().getType()] = decl;
			}

			std::map<
					mlir::rlc::ClassDeclaration,
					llvm::SmallVector<mlir::rlc::ClassDeclaration, 2>>
					dependencies;

			;
			for (auto decl : decls)
			{
				decl.getResult().getType().walk([&](mlir::Type t) {
					if (declaredTypeToDeclaration.count(t) == 0)
						return;

					dependencies[decl].push_back(declaredTypeToDeclaration[t]);
				});
			}

			llvm::DenseSet<mlir::rlc::ClassDeclaration> alredyEmitted;
			llvm::SmallVector<mlir::rlc::ClassDeclaration, 2> inOrderDeclaration;
			for (auto& pair : dependencies)
			{
				if (alredyEmitted.contains(pair.first))
					continue;

				for (auto type : pair.second)
				{
					if (alredyEmitted.contains(type))
						continue;
					inOrderDeclaration.push_back(type);
					alredyEmitted.insert(type);
				}
				inOrderDeclaration.push_back(pair.first);
				alredyEmitted.insert(pair.first);
			}

			mlir::IRRewriter rewriter(getOperation().getContext());
			for (auto toEmit : llvm::make_range(
							 inOrderDeclaration.rbegin(), inOrderDeclaration.rend()))
			{
				toEmit->moveBefore(
						&getOperation().getBodyRegion().front(),
						getOperation().getBodyRegion().front().begin());
			}
		}
	};
}	 // namespace mlir::rlc
