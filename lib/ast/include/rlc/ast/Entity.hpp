#pragma once

#include "llvm/ADT/SmallVector.h"
#include "llvm/Support/Error.h"
#include "llvm/Support/raw_ostream.h"
#include "rlc/ast/SymbolTable.hpp"
#include "rlc/ast/Type.hpp"
#include "rlc/ast/TypeUse.hpp"
#include "rlc/utils/SourcePosition.hpp"
namespace rlc
{
	class EntityField
	{
		public:
		friend llvm::yaml::MappingTraits<rlc::EntityField>;
		EntityField(
				SingleTypeUse typeName,
				std::string name,
				SourcePosition position = SourcePosition())
				: fieldTypeName(std::move(typeName)),
					fieldName(std::move(name)),
					position(std::move(position))
		{
		}

		[[nodiscard]] Type* getType() const { return fieldTypeName.getType(); }
		[[nodiscard]] const std::string& getName() const { return fieldName; }
		[[nodiscard]] const SingleTypeUse& getTypeUse() const
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
		llvm::Error deduceType(const SymbolTable& tb, TypeDB& db);

		[[nodiscard]] const SourcePosition& getPosition() const { return position; }

		private:
		SingleTypeUse fieldTypeName;
		std::string fieldName;
		SourcePosition position;
	};

	class Entity
	{
		public:
		friend llvm::yaml::MappingTraits<rlc::Entity>;
		explicit Entity(
				std::string name, llvm::SmallVector<EntityField, 3> fields = {})
				: name(std::move(name)), fields(std::move(fields))
		{
		}

		[[nodiscard]] const std::string& getName() const { return name; }
		[[nodiscard]] auto begin() const { return fields.begin(); }
		[[nodiscard]] auto end() const { return fields.end(); }
		[[nodiscard]] auto begin() { return fields.begin(); }
		[[nodiscard]] auto end() { return fields.end(); }
		[[nodiscard]] size_t fieldsCount() const { return fields.size(); }

		void addField(llvm::ArrayRef<EntityField> fields)
		{
			for (auto& field : fields)
				this->fields.emplace_back(field);
		}

		EntityField& addField(EntityField field)
		{
			fields.emplace_back(std::move(field));
			return fields.back();
		}

		[[nodiscard]] bool operator==(const Entity& other) const
		{
			return name == other.name and fields == other.fields;
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

		[[nodiscard]] size_t indexOfField(llvm::StringRef name) const;
		llvm::Error deduceType(const SymbolTable& tb, TypeDB& db);
		llvm::Error createType(SymbolTable& tb, TypeDB& db);

		private:
		std::string name;
		Type* type = nullptr;
		llvm::SmallVector<EntityField, 3> fields;
	};
}	 // namespace rlc

template<>
struct llvm::yaml::MappingTraits<rlc::EntityField>
{
	static void mapping(IO& io, rlc::EntityField& value)
	{
		assert(io.outputting());
		io.mapRequired("name", value.fieldName);
		io.mapRequired("type", value.fieldTypeName);
		if (not value.position.isMissing())
			io.mapRequired("position", value.position);
	}
};

template<>
struct llvm::yaml::SequenceTraits<llvm::SmallVector<rlc::EntityField, 3>>
{
	static size_t size(IO& io, llvm::SmallVector<rlc::EntityField, 3>& list)
	{
		return list.size();
	}
	static rlc::EntityField& element(
			IO& io, llvm::SmallVector<rlc::EntityField, 3>& list, size_t index)
	{
		assert(io.outputting());
		return list[index];
	}
};

template<>
struct llvm::yaml::MappingTraits<rlc::Entity>
{
	static void mapping(IO& io, rlc::Entity& value)
	{
		assert(io.outputting());
		io.mapRequired("name", value.name);
		if (value.type != nullptr)
			io.mapRequired("type", *value.type);
		io.mapRequired("fields", value.fields);
	}
};
