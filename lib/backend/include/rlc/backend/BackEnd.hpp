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

		TargetInfoImpl* pimpl;
	};

}	 // namespace mlir::rlc
