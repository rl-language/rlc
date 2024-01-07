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
			llvm::SmallVector<mlir::rlc::EntityDeclaration, 2> decls;
			llvm::DenseMap<mlir::Type, mlir::rlc::EntityDeclaration>
					declaredTypeToDeclaration;
			for (auto decl : getOperation().getOps<mlir::rlc::EntityDeclaration>())
			{
				decls.push_back(decl);
				declaredTypeToDeclaration[decl.getResult().getType()] = decl;
			}

			std::map<
					mlir::rlc::EntityDeclaration,
					llvm::SmallVector<mlir::rlc::EntityDeclaration, 2>>
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

			llvm::DenseSet<mlir::rlc::EntityDeclaration> alredyEmitted;
			llvm::SmallVector<mlir::rlc::EntityDeclaration, 2> inOrderDeclaration;
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
