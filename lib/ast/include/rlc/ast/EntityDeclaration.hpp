#pragma once

#include "llvm/Support/raw_ostream.h"
#include "rlc/ast/Entity.hpp"
#include "rlc/utils/SourcePosition.hpp"
namespace rlc
{
	class EntityDeclaration
	{
		public:
		EntityDeclaration(Entity entity, SourcePosition pos = SourcePosition())
				: entity(std::move(entity)), pos(std::move(pos))
		{
		}

		[[nodiscard]] const SourcePosition& getSourcePosition() const
		{
			return pos;
		}

		[[nodiscard]] const Entity& getEntity() const { return entity; }
		[[nodiscard]] Entity& getEntity() { return entity; }
		void print(
				llvm::raw_ostream& OS,
				size_t indents = 0,
				bool printLocation = false) const;
		void dump() const;

		private:
		Entity entity;
		SourcePosition pos;
	};
}	 // namespace rlc
