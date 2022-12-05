#pragma once

#include <llvm/ADT/SmallVector.h>
#include <memory>
#include <string>

#include "llvm/ADT/iterator_range.h"
#include "llvm/Support/Error.h"
#include "llvm/Support/raw_ostream.h"
#include "rlc/ast/SymbolTable.hpp"
#include "rlc/ast/Type.hpp"
#include "rlc/utils/SourcePosition.hpp"
namespace rlc
{
	class FunctionTypeUse;
	class SingleTypeUse
	{
		public:
		friend class FunctionTypeUse;
		friend llvm::yaml::MappingTraits<rlc::SingleTypeUse>;
		[[nodiscard]] const std::string& getName() const { return name; }
		[[nodiscard]] bool isArray() const { return array; }
		[[nodiscard]] size_t getDimension() const { return dim; }
		[[nodiscard]] bool isScalar() const { return !array; }
		[[nodiscard]] bool isFunctionType() const { return fType != nullptr; }
		[[nodiscard]] const FunctionTypeUse& getFunctionType() const;
		[[nodiscard]] FunctionTypeUse& getFunctionType();

		static SingleTypeUse scalarType(
				std::string name, SourcePosition pos = SourcePosition())
		{
			return SingleTypeUse(std::move(name), false, 1, std::move(pos));
		}
		static SingleTypeUse arrayType(
				std::string name, size_t dim = 1, SourcePosition pos = SourcePosition())
		{
			return SingleTypeUse(std::move(name), true, dim, std::move(pos));
		}
		void print(llvm::raw_ostream& OS) const;
		void dump() const;

		explicit SingleTypeUse(Type* t): type(t) { assert(t != nullptr); }
		SingleTypeUse(SingleTypeUse&& other) = default;
		SingleTypeUse& operator=(SingleTypeUse&& other) = default;
		SingleTypeUse(const SingleTypeUse& other);
		SingleTypeUse& operator=(const SingleTypeUse& other);
		~SingleTypeUse() = default;
		[[nodiscard]] std::string toString() const;
		[[nodiscard]] Type* getType() const { return type; }
		llvm::Error deduceType(const SymbolTable& tb, TypeDB& db);

		[[nodiscard]] const SourcePosition& getPosition() const { return position; }

		[[nodiscard]] bool operator==(const SingleTypeUse& other) const
		{
			return std::tie(name, array, dim, *fType, type) ==
						 std::tie(
								 other.name, other.array, other.dim, *other.fType, other.type);
		}
		[[nodiscard]] bool operator!=(const SingleTypeUse& other) const
		{
			return not(*this == other);
		}

		private:
		explicit SingleTypeUse(
				std::string name,
				bool array,
				size_t dim = 1,
				SourcePosition position = SourcePosition())
				: name(std::move(name)),
					array(array),
					dim(dim),
					position(std::move(position))
		{
		}
		explicit SingleTypeUse(
				std::unique_ptr<FunctionTypeUse> f,
				SourcePosition position = SourcePosition());
		std::string name;
		bool array{ false };
		size_t dim{ 1 };
		std::unique_ptr<FunctionTypeUse> fType{ nullptr };
		Type* type{ nullptr };
		SourcePosition position;
	};

	/**
	 * A type use is a position in the source code where a type appears,
	 * such as in argument declarations.
	 */
	class FunctionTypeUse
	{
		public:
		friend class SingleTypeUse;
		friend llvm::yaml::MappingTraits<rlc::FunctionTypeUse>;
		[[nodiscard]] auto begin() const { return types.begin(); }
		[[nodiscard]] auto begin() { return types.begin(); }
		[[nodiscard]] auto end() const { return types.end(); }
		[[nodiscard]] auto end() { return types.end(); }
		[[nodiscard]] auto argsRange()
		{
			return llvm::make_range(begin(), end() - 1);
		}
		[[nodiscard]] auto argsRange() const
		{
			return llvm::make_range(begin(), end() - 1);
		}
		explicit FunctionTypeUse(Type* t): type(t) { assert(t->isFunctionType()); }
		[[nodiscard]] const SingleTypeUse& operator[](size_t index) const
		{
			return types[index];
		}

		[[nodiscard]] const SingleTypeUse& getArgType(size_t index) const
		{
			assert(index < argCount());
			return types[index];
		}

		[[nodiscard]] const SingleTypeUse& getReturnType() const
		{
			return types.back();
		}

		[[nodiscard]] size_t argCount() const { return types.size() - 1; }
		static SingleTypeUse functionType(
				llvm::SmallVector<SingleTypeUse, 2> types,
				SourcePosition position = SourcePosition())
		{
			return SingleTypeUse(
					std::make_unique<FunctionTypeUse>(std::move(types), position),
					position);
		}

		void print(llvm::raw_ostream& OS) const;
		void dump() const;

		explicit FunctionTypeUse(
				llvm::SmallVector<SingleTypeUse, 2> types, SourcePosition position)
				: types(std::move(types)), position(std::move(position))
		{
		}
		llvm::Error deduceType(const SymbolTable& tb, TypeDB& db);
		[[nodiscard]] Type* getType() const { return type; }
		[[nodiscard]] const SourcePosition& getPosition() const { return position; }

		[[nodiscard]] bool operator==(const FunctionTypeUse& other) const
		{
			return std::tie(types, type) == std::tie(other.types, other.type);
		}
		[[nodiscard]] bool operator!=(const FunctionTypeUse& other) const
		{
			return !(*this == other);
		}

		private:
		llvm::SmallVector<SingleTypeUse, 2> types;
		Type* type{ nullptr };
		SourcePosition position;
	};

}	 // namespace rlc

template<>
struct llvm::yaml::MappingTraits<rlc::SingleTypeUse>
{
	static void mapping(IO& io, rlc::SingleTypeUse& value)
	{
		assert(io.outputting());
		if (value.type != nullptr)
			io.mapRequired("type", *value.type);
		else
			io.mapRequired("type", value.name);
		if (value.fType != nullptr)
			io.mapRequired("function_type", *value.fType);

		if (value.isArray())
			io.mapRequired("dim", value.dim);

		if (not value.getPosition().isMissing())
			io.mapRequired("position", value.position);
	}
};

template<>
struct llvm::yaml::SequenceTraits<llvm::SmallVector<rlc::SingleTypeUse, 2>>
{
	static size_t size(IO& io, llvm::SmallVector<rlc::SingleTypeUse, 2>& list)
	{
		return list.size();
	}
	static rlc::SingleTypeUse& element(
			IO& io, llvm::SmallVector<rlc::SingleTypeUse, 2>& list, size_t index)
	{
		assert(io.outputting());
		return list[index];
	}
};

template<>
struct llvm::yaml::MappingTraits<rlc::FunctionTypeUse>
{
	static void mapping(IO& io, rlc::FunctionTypeUse& value)
	{
		assert(io.outputting());
		io.mapRequired("literal", value.types);
		if (value.type != nullptr)
			io.mapRequired("deduced", *value.type);
		if (not value.getPosition().isMissing())
			io.mapRequired("position", value.position);
	}
};
