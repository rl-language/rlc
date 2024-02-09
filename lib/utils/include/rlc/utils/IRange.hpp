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
