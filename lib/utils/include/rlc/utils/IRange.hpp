#pragma once
#include <iterator>

#include "llvm/ADT/iterator_range.h"

namespace rlc
{
	template<typename IType>
	class IRangeIterator
	{
		public:
		using __iterator_category = std::input_iterator_tag;
		using value_type = IType;
		using difference_type = IType;
		using pointer = IType*;
		using reference = IType&;

		explicit IRangeIterator(IType current = 0): current(current) {}
		[[nodiscard]] bool operator==(const IRangeIterator& other) const
		{
			return current == other.current;
		}
		[[nodiscard]] bool operator!=(const IRangeIterator& other) const
		{
			return current != other.current;
		}
		[[nodiscard]] reference operator*() const { return current; }
		[[nodiscard]] reference operator*() { return current; }
		[[nodiscard]] pointer operator->() const { return &current; }
		[[nodiscard]] pointer operator->() { return &current; }
		IRangeIterator operator++(int)	// NOLINT
		{
			auto copy = *this;
			++(*this);
			return copy;
		}
		IRangeIterator& operator++()	// NOLINT
		{
			current++;
			return *this;
		}

		private:
		IType current;
	};

	template<typename IType>
	llvm::iterator_range<IRangeIterator<IType>> irange(IType begin, IType end)
	{
		return llvm::make_range(IRangeIterator(begin), IRangeIterator(end));
	}

	template<typename IType>
	llvm::iterator_range<IRangeIterator<IType>> irange(IType end)
	{
		return llvm::make_range(IRangeIterator<IType>(0), IRangeIterator(end));
	}
}	 // namespace rlc
