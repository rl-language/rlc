#pragma once

#include <array>
#include <memory>

#include "llvm/Support/raw_ostream.h"
namespace rlc
{
	class Expression;
	class MemberAccess
	{
		public:
		MemberAccess(const Expression& exp, std::string accessedField);

		[[nodiscard]] const std::string& getFieldName() const
		{
			return accessedField;
		}

		[[nodiscard]] std::string& getFieldName() { return accessedField; }

		[[nodiscard]] const Expression& getExp() const { return *exp[0]; }
		[[nodiscard]] Expression& getExp() { return *exp[0]; }

		[[nodiscard]] auto begin() const { return exp.begin(); }
		[[nodiscard]] auto begin() { return exp.begin(); }
		[[nodiscard]] auto end() const { return exp.end(); }
		[[nodiscard]] auto end() { return exp.end(); }

		void print(llvm::raw_ostream& OS = llvm::outs(), size_t indents = 0) const;
		void dump() const;
		[[nodiscard]] bool operator==(const MemberAccess& other) const;
		[[nodiscard]] bool operator!=(const MemberAccess& other) const
		{
			return !(*this == other);
		}

		MemberAccess(MemberAccess&& other) = default;
		~MemberAccess() = default;
		MemberAccess& operator=(const MemberAccess& other);
		MemberAccess(const MemberAccess& other);
		MemberAccess& operator=(MemberAccess&& other) = default;

		private:
		std::string accessedField;
		std::array<std::unique_ptr<Expression>, 1> exp;
	};
}	 // namespace rlc
