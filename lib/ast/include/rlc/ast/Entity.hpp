#pragma once

#include "llvm/ADT/SmallVector.h"
#include "llvm/Support/raw_ostream.h"
#include "rlc/ast/Type.hpp"
namespace rlc
{
	class EntityField
	{
		public:
		EntityField(std::string typeName, std::string name)
				: fieldType(nullptr),
					fieldTypeName(move(typeName)),
					fieldName(move(name))
		{
		}

		[[nodiscard]] Type* getType() const { return fieldType; }
		[[nodiscard]] const std::string& getName() const { return fieldName; }
		[[nodiscard]] const std::string& getTypeName() const
		{
			return fieldTypeName;
		}

		[[nodiscard]] bool operator==(const EntityField& other) const
		{
			return fieldName == other.fieldName and
						 fieldTypeName == other.fieldTypeName;
		}

		[[nodiscard]] bool operator!=(const EntityField& other) const
		{
			return !(*this == other);
		}

		void print(llvm::raw_ostream& OS) const;
		void dump() const;

		private:
		Type* fieldType;
		std::string fieldTypeName;
		std::string fieldName;
	};

	class Entity
	{
		public:
		explicit Entity(
				std::string name, llvm::SmallVector<EntityField, 3> fields = {})
				: nm(move(name)), type(nullptr), fields(std::move(fields))
		{
		}

		[[nodiscard]] const std::string& getName() const { return nm; }
		[[nodiscard]] auto begin() const { return fields.begin(); }
		[[nodiscard]] auto end() const { return fields.end(); }
		[[nodiscard]] auto begin() { return fields.begin(); }
		[[nodiscard]] auto end() { return fields.end(); }
		[[nodiscard]] size_t fieldsCount() const { return fields.size(); }

		[[nodiscard]] bool operator==(const Entity& other) const
		{
			return nm == other.nm and fields == other.fields;
		}

		[[nodiscard]] bool operator!=(const Entity& other) const
		{
			return !(*this == other);
		}

		[[nodiscard]] const EntityField& operator[](size_t index) const
		{
			return fields[index];
		}
		[[nodiscard]] EntityField& operator[](size_t index)
		{
			return fields[index];
		}
		[[nodiscard]] Type* getType() const { return type; }
		void print(llvm::raw_ostream& OS, size_t indents = 0) const;
		void dump() const;

		private:
		std::string nm;
		Type* type;
		llvm::SmallVector<EntityField, 3> fields;
	};
}	 // namespace rlc
