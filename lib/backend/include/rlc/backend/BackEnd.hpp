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
#pragma once

#include <memory>
#include <string>

namespace llvm
{
	class DataLayout;
}
namespace mlir::rlc
{
	void initLLVM();

	struct TargetInfoImpl;
	struct TargetInfo
	{
		public:
		TargetInfo(std::string triple, bool shared, bool optimize);
		~TargetInfo();
		bool optimize() const;
		bool isShared() const;
		const llvm::DataLayout& getDataLayout() const;

		TargetInfo(TargetInfo&& other): pimpl(other.pimpl)
		{
			other.pimpl = nullptr;
		}
		TargetInfo& operator==(TargetInfo&& other);

		TargetInfo(const TargetInfo& other) = delete;
		TargetInfo& operator=(const TargetInfo& other) = delete;

		TargetInfoImpl* pimpl;
	};

}	 // namespace mlir::rlc
