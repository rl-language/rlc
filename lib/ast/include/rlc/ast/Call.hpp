#pragma once

#include <memory>

#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/Support/raw_ostream.h"
#include "rlc/utils/SimpleIterator.hpp"
namespace rlc
{
	class Expression;
	class Call
	{
		public:
		friend class Expression;
		using iterator = SimpleIterator<Call&, Expression>;
		using const_iterator = SimpleIterator<const Call&, const Expression>;

		[[nodiscard]] const_iterator begin() const { return const_iterator(*this); }
		[[nodiscard]] iterator begin() { return iterator(*this); }
		[[nodiscard]] const_iterator end() const
		{
			return const_iterator(*this, subExpCount());
		}
		[[nodiscard]] iterator end() { return iterator(*this, subExpCount()); }

		[[nodiscard]] Expression& getFunctionExpression() { return *args.back(); }
		[[nodiscard]] const Expression& getFunctionExpression() const
		{
			return *args.back();
		}
		[[nodiscard]] size_t subExpCount() const { return args.size(); }
		[[nodiscard]] size_t argsCount() const { return args.size() - 1; }

		[[nodiscard]] Expression& getArg(size_t index)
		{
			assert(index < argsCount());
			return *args[index];
		}
		[[nodiscard]] const Expression& getArg(size_t index) const
		{
			assert(index < argsCount());
			return *args[index];
		}

		[[nodiscard]] Expression& getSubExpression(size_t index)
		{
			assert(index < subExpCount());
			return *args[index];
		}
		[[nodiscard]] const Expression& getSubExpression(size_t index) const
		{
			assert(index < subExpCount());
			return *args[index];
		}

		[[nodiscard]] auto argsRange() const
		{
			return llvm::make_range(begin(), end() - 1);
		}

		[[nodiscard]] auto argsRange()
		{
			return llvm::make_range(begin(), end() - 1);
		}

		void print(llvm::raw_ostream& OS = llvm::outs(), size_t indents = 0) const;
		void dump() const;

		Call(const Call& other);
		Call(Call&& other) = default;
		Call& operator=(const Call& other);
		Call& operator=(Call&& other) = default;
		~Call() = default;

		[[nodiscard]] bool operator==(const Call& other) const;

		[[nodiscard]] bool operator!=(const Call& other) const
		{
			return !(*this == other);
		}
		using Container = llvm::SmallVector<std::unique_ptr<Expression>, 3>;

		private:
		explicit Call(std::unique_ptr<Expression> f, Container args = {});
		Container args;
	};

}	 // namespace rlc
