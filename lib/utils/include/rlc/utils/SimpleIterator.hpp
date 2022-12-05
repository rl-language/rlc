#pragma once

#include <cstddef>
#include <iterator>

#include "llvm/ADT/iterator.h"
namespace rlc
{
	template<typename T, typename E, typename RetType = E&>
	class SimpleIterator: public llvm::iterator_facade_base<
														SimpleIterator<T, E, RetType>,
														std::random_access_iterator_tag,
														E>
	{
		public:
		using DiffTp = typename SimpleIterator<T, E, E&>::difference_type;
		SimpleIterator(T t, size_t index = 0): index(index), type(t) {}
		[[nodiscard]] bool operator==(const SimpleIterator& other) const
		{
			return index == other.index;
		}

		RetType operator*() const;
		SimpleIterator& operator++()
		{
			index++;
			return *this;
		}

		SimpleIterator operator++(int)
		{
			auto copy = *this;
			index++;
			return copy;
		}

		SimpleIterator& operator--()
		{
			index--;
			return *this;
		}

		bool operator<(const SimpleIterator& RHS) const
		{
			return index < RHS.index;
		}

		SimpleIterator operator-(size_t R) const
		{
			SimpleIterator cp = *this;
			cp.index -= R;
			return cp;
		}

		DiffTp operator-(const SimpleIterator& R) const { return index - R.index; }

		SimpleIterator& operator+=(DiffTp N)
		{
			index += N;
			return *this;
		}
		SimpleIterator& operator-=(DiffTp N)
		{
			index -= N;
			return *this;
		}

		private:
		size_t index;
		T type;
	};
}	 // namespace rlc
