#pragma once

#include <string>

#include "llvm/Support/YAMLTraits.h"
#include "llvm/Support/raw_ostream.h"
#include "rlc/ast/BuiltinFunctions.hpp"
#include "rlc/ast/TypeUse.hpp"
#include "rlc/utils/SourcePosition.hpp"

namespace rlc
{
	class Expression;
	class ArrayAccess
	{
		public:
		friend class Expression;
		friend llvm::yaml::MappingTraits<rlc::ArrayAccess>;
		using Container = llvm::SmallVector<std::unique_ptr<Expression>, 3>;
		static constexpr const char* name = "array_access";

		void print(llvm::raw_ostream& OS) const;
		void dump() const;

		ArrayAccess(const ArrayAccess& other);
		ArrayAccess(ArrayAccess&& other) = default;
		ArrayAccess& operator=(const ArrayAccess& other);
		ArrayAccess& operator=(ArrayAccess&& other) = default;
		~ArrayAccess() = default;

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

		[[nodiscard]] bool operator==(const ArrayAccess& other) const;

		[[nodiscard]] bool operator!=(const ArrayAccess& other) const
		{
			return !(*this == other);
		}

		[[nodiscard]] const SourcePosition& getPosition() const { return position; }
		void setPosition(const SourcePosition& newPoisition)
		{
			position = newPoisition;
		}
		[[nodiscard]] size_t subExpCount() const { return args.size(); }

		using iterator = SimpleIterator<ArrayAccess&, Expression>;
		using const_iterator = SimpleIterator<const ArrayAccess&, const Expression>;

		[[nodiscard]] const_iterator begin() const { return const_iterator(*this); }
		[[nodiscard]] iterator begin() { return iterator(*this); }
		[[nodiscard]] const_iterator end() const
		{
			return const_iterator(*this, subExpCount());
		}
		[[nodiscard]] iterator end() { return iterator(*this, subExpCount()); }

		[[nodiscard]] const Expression& getArray() const { return *args[0]; }

		[[nodiscard]] const Expression& getIndexing() const { return *args[1]; }

		Expression& getArray() { return *args[0]; }

		Expression& getIndexing() { return *args[1]; }

		private:
		ArrayAccess(
				std::unique_ptr<Expression> lhs,
				std::unique_ptr<Expression> rhs,
				SourcePosition position);
		Container args;
		SourcePosition position;
	};
}	 // namespace rlc

template<>
struct llvm::yaml::MappingTraits<rlc::ArrayAccess>
{
	static void mapping(IO& io, rlc::ArrayAccess& value)
	{
		assert(io.outputting());
		io.mapRequired("lhs", *value.args[0]);
		io.mapRequired("rhs", *value.args[1]);
		if (not value.getPosition().isMissing())
			io.mapRequired("position", value.position);
	}
};
