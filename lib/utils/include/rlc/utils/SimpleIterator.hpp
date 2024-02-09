/*
Copyright 2024 Massimo Fioravanti

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
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
