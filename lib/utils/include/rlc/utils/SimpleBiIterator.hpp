#pragma once

#include <cstddef>
#include <iterator>

#include "llvm/ADT/iterator.h"
namespace rlc
{
	template<typename T, typename E, typename RetType = E&>
	class SimpleBiIterator: public llvm::iterator_facade_base<
															SimpleBiIterator<T, E, RetType>,
															std::random_access_iterator_tag,
															E>
	{
		public:
		using DiffTp = typename SimpleBiIterator<T, E, E&>::difference_type;
		SimpleBiIterator(T t, size_t index = 0): index(index), type(t) {}
		[[nodiscard]] bool operator==(const SimpleBiIterator& other) const
		{
			return index == other.index;
		}

		RetType operator*() const;
		SimpleBiIterator& operator++()
		{
			index++;
			return *this;
		}

		SimpleBiIterator& operator--()
		{
			index--;
			return *this;
		}

		bool operator<(const SimpleBiIterator& RHS) const
		{
			return index < RHS.index;
		}

		SimpleBiIterator operator-(size_t R) const
		{
			SimpleBiIterator cp = *this;
			cp.index -= R;
			return cp;
		}

		DiffTp operator-(const SimpleBiIterator& R) const
		{
			return index - R.index;
		}

		SimpleBiIterator& operator+=(DiffTp N)
		{
			index += N;
			return *this;
		}
		SimpleBiIterator& operator-=(DiffTp N)
		{
			index -= N;
			return *this;
		}

		private:
		size_t index;
		T type;
	};
}	 // namespace rlc
