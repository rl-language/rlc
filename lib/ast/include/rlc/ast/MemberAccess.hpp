#pragma once

#include <array>
#include <memory>

#include "llvm/Support/raw_ostream.h"
#include "rlc/utils/SimpleIterator.hpp"
namespace rlc
{
	class Expression;
	class MemberAccess
	{
		public:
		using iterator = SimpleIterator<MemberAccess&, Expression>;
		using const_iterator =
				SimpleIterator<const MemberAccess&, const Expression>;
		MemberAccess(const Expression& exp, std::string accessedField);

		[[nodiscard]] const std::string& getFieldName() const
		{
			return accessedField;
		}

		[[nodiscard]] std::string& getFieldName() { return accessedField; }

		[[nodiscard]] const Expression& getExp() const { return *exp[0]; }
		[[nodiscard]] Expression& getExp() { return *exp[0]; }

		[[nodiscard]] const_iterator begin() const { return const_iterator(*this); }
		[[nodiscard]] iterator begin() { return iterator(*this); }
		[[nodiscard]] const_iterator end() const
		{
			return const_iterator(*this, 1);
		}
		[[nodiscard]] iterator end() { return iterator(*this, 1); }

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
