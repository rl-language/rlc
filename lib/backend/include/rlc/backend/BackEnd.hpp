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
