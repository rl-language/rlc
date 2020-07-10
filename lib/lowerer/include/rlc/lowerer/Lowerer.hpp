#pragma once

#include <memory>

#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/Support/Error.h"
#include "llvm/Support/raw_ostream.h"
#include "rlc/ast/Entity.hpp"
#include "rlc/ast/FunctionDeclaration.hpp"
#include "rlc/ast/System.hpp"
#include "rlc/ast/Type.hpp"

namespace rlc
{
	class Lowerer
	{
		public:
		llvm::Error lowerSystem(const System& system);
		llvm::Error lowerEntity(const Entity& entity, llvm::Module& module);
		llvm::Error lowerDeclaration(
				const FunctionDeclaration& decl, llvm::Module& module);

		llvm::Error declareOpaqueStruct(const Entity& entity, llvm::Module& module);

		[[nodiscard]] llvm::Type* rlcToLlvmType(rlc::Type* t) const;

		[[nodiscard]] const llvm::Module& getModule(size_t index) const
		{
			return *modules[index];
		}

		[[nodiscard]] size_t modulesCount() const { return modules.size(); }

		/**
		 * returns true if there were no errors
		 */
		bool verify(llvm::raw_ostream& OS);

		private:
		[[nodiscard]] llvm::Type* uncahedRlcToLlvmType(rlc::Type* t) const;
		[[nodiscard]] llvm::Type* rlcBuiltinTollvmType(rlc::Type* t) const;
		[[nodiscard]] llvm::Type* rlcFunctionTypeToLlvmType(rlc::Type* t) const;
		mutable llvm::LLVMContext context;
		llvm::SmallVector<std::unique_ptr<llvm::Module>, 3> modules;
		mutable llvm::DenseMap<rlc::Type*, llvm::Type*> typeToTypeMap;
	};
}	 // namespace rlc
