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

#include <type_traits>
#include <utility>
namespace rlc
{
	template<typename Lambda>
	class ScopeGuard
	{
		bool committed;	 // not mutable
		Lambda rollbackLambda;

		public:
		explicit ScopeGuard(Lambda&& _l)
				: committed(false), rollbackLambda(std::forward<Lambda>(_l))
		{
		}

		ScopeGuard(const ScopeGuard& other) = delete;
		ScopeGuard(ScopeGuard&& other)
				: committed(other.committed),
					rollbackLambda(std::move(other.rollbackLambda))
		{
			other.commit();
		}
		ScopeGuard& operator=(const ScopeGuard& other) = delete;
		ScopeGuard& operator=(ScopeGuard&& other) = delete;

		~ScopeGuard()
		{
			if (!committed)
				rollbackLambda();	 // what if this throws?
		}
		void commit() { committed = true; }	 // no need for const
	};

	template<typename Lambda>
	[[nodiscard]] auto makeGuard(Lambda&& lambda)
	{
		return ScopeGuard<Lambda>(std::forward<Lambda>(lambda));
	}

}	 // namespace rlc
