#pragma once

#include <memory>

#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/Support/raw_ostream.h"
#include "rlc/utils/SimpleIterator.hpp"
#include "rlc/utils/SourcePosition.hpp"

namespace rlc
{
	class Expression;
	class Call
	{
		public:
		friend class Expression;
		friend llvm::yaml::MappingTraits<rlc::Call>;
		static constexpr const char* name = "call";
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

		void print(
				llvm::raw_ostream& OS = llvm::outs(),
				size_t indents = 0,
				bool printLocation = false) const;
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

		[[nodiscard]] const SourcePosition& getPosition() const { return position; }
		void setPosition(const SourcePosition& newPoisition)
		{
			position = newPoisition;
		}

		private:
		explicit Call(
				std::unique_ptr<Expression> f,
				Container args = {},
				SourcePosition position = SourcePosition());
		Container args;
		SourcePosition position;
	};

}	 // namespace rlc

template<>
struct llvm::yaml::SequenceTraits<rlc::Call::Container>
{
	static size_t size(IO& io, rlc::Call::Container& list) { return list.size(); }
	static rlc::Expression& element(
			IO& io, rlc::Call::Container& list, size_t index)
	{
		assert(io.outputting());
		return *list[index];
	}
};

template<>
struct llvm::yaml::MappingTraits<rlc::Call>
{
	static void mapping(IO& io, rlc::Call& value)
	{
		assert(io.outputting());
		io.mapRequired("args", value.args);
		if (not value.getPosition().isMissing())
			io.mapRequired("position", value.position);
	}
};
