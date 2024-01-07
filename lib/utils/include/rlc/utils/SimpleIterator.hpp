/*
This file is part of the RLC project.

RLC is free software: you can redistribute it and/or modify it under the terms
of the GNU General Public License version 2 as published by the Free Software
Foundation.

RLC is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
RLC. If not, see <https://www.gnu.org/licenses/>.
*/
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
