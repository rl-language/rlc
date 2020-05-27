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
