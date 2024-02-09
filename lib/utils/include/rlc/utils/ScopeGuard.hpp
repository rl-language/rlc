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
