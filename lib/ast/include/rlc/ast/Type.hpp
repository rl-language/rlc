#pragma once

#include <cstddef>
#include <iterator>
#include <map>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <variant>

#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/iterator.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/Support/Allocator.h"
#include "llvm/Support/raw_ostream.h"

namespace rlc
{
	enum class BuiltinType
	{
		VOID,
		LONG,
		DOUBLE,
		BOOL
	};

	llvm::StringRef builtinTypeToString(BuiltinType t);

	class Type;
	class ArrayType
	{
		public:
		ArrayType(Type* baseType, size_t size): innerType({ baseType }), siz(size)
		{
		}
		[[nodiscard]] size_t size() const { return siz; }
		[[nodiscard]] Type* getBaseType() const { return innerType[0]; }
		[[nodiscard]] std::string getName() const;
		[[nodiscard]] auto begin() const { return innerType.begin(); }
		[[nodiscard]] auto end() const { return innerType.end(); }

		[[nodiscard]] bool operator==(const ArrayType& other) const
		{
			return siz == other.siz and innerType == other.innerType;
		}

		[[nodiscard]] bool operator!=(const ArrayType& other) const
		{
			return !(*this == other);
		}

		private:
		const std::array<Type*, 1> innerType;
		const size_t siz;
	};

	class UserDefinedType
	{
		public:
		UserDefinedType(llvm::StringRef name, llvm::ArrayRef<Type*> memberTypes)
				: name(name), memberTypes(memberTypes.begin(), memberTypes.end())
		{
		}
		[[nodiscard]] llvm::StringRef getName() const { return name; }

		[[nodiscard]] auto begin() const { return memberTypes.begin(); }
		[[nodiscard]] auto end() const { return memberTypes.end(); }
		[[nodiscard]] size_t childrenCount() const { return memberTypes.size(); }
		[[nodiscard]] Type* getContainedType(size_t index) const
		{
			return memberTypes[index];
		}

		[[nodiscard]] bool operator==(const UserDefinedType& other) const
		{
			return name == other.name && memberTypes == other.memberTypes;
		}

		[[nodiscard]] bool operator!=(const UserDefinedType& other) const
		{
			return !(*this == other);
		}

		private:
		llvm::StringRef name;
		const llvm::SmallVector<Type*, 3> memberTypes;
	};

	class FunctionType
	{
		public:
		FunctionType(Type* returnType, llvm::ArrayRef<Type*> argTypes)
				: types(argTypes.begin(), argTypes.end())
		{
			types.push_back(returnType);
		}

		template<typename... Args>
		FunctionType(Type* type, Args... args): types({ type, args... })
		{
		}

		[[nodiscard]] auto begin() const { return types.begin(); }
		[[nodiscard]] auto end() const { return types.end(); }
		[[nodiscard]] Type* getContainedType(size_t index) const
		{
			assert(index < types.size());
			return types[index];
		}

		[[nodiscard]] Type* getArgumentType(size_t index) const
		{
			assert(index < argumentsCount());
			return types[index];
		}

		[[nodiscard]] auto argumentsRange() const
		{
			return llvm::make_range(begin(), end() - 1);
		}

		[[nodiscard]] bool operator==(const FunctionType& other) const
		{
			return types == other.types;
		}

		[[nodiscard]] bool operator!=(const FunctionType& other) const
		{
			return !(*this == other);
		}

		[[nodiscard]] std::string getName() const;

		[[nodiscard]] size_t argumentsCount() const { return types.size() - 1; }
		[[nodiscard]] size_t containedTypesCount() const { return types.size(); }

		[[nodiscard]] Type* getReturnType() const { return types.back(); }

		private:
		llvm::SmallVector<Type*, 4> types;
	};

	class TypeIterator: public llvm::iterator_facade_base<
													TypeIterator,
													std::bidirectional_iterator_tag,
													Type*>
	{
		public:
		TypeIterator(const Type* t, size_t index = 0): index(index), type(t) {}
		[[nodiscard]] bool operator==(const TypeIterator& other) const
		{
			return index == other.index;
		}

		Type* operator*() const;
		TypeIterator& operator++()
		{
			index++;
			return *this;
		}

		TypeIterator& operator--()
		{
			index--;
			return *this;
		}

		private:
		size_t index;
		const Type* type;
	};

	class Type
	{
		public:
		friend class TypeDB;
		template<typename T>
		[[nodiscard]] bool isA() const
		{
			return std::holds_alternative<T>(content);
		}

		[[nodiscard]] size_t arrayLenght() const { return get<ArrayType>().size(); }

		[[nodiscard]] Type* getBaseType() const
		{
			return get<ArrayType>().getBaseType();
		}
		[[nodiscard]] size_t getArgCount() const
		{
			return get<FunctionType>().argumentsCount();
		}

		template<typename T>
		[[nodiscard]] const T& get() const
		{
			assert(isA<T>());
			return std::get<T>(content);
		}

		[[nodiscard]] bool isBuiltin() const { return isA<BuiltinType>(); }

		[[nodiscard]] bool isArray() const { return isA<ArrayType>(); }

		[[nodiscard]] bool isUserDefined() const { return isA<UserDefinedType>(); }

		template<class Visitor>
		auto visit(Visitor&& vis) const
		{
			return std::visit(std::forward<Visitor>(vis), content);
		}

		[[nodiscard]] size_t containedTypesCount() const;
		[[nodiscard]] Type* getContainedType(size_t index) const;

		[[nodiscard]] std::string getName() const;

		[[nodiscard]] auto begin() const { return TypeIterator(this); }
		[[nodiscard]] auto end() const
		{
			return TypeIterator(this, containedTypesCount());
		}

		[[nodiscard]] bool isVoid() const
		{
			return isBuiltin() && get<BuiltinType>() == BuiltinType::VOID;
		}

		[[nodiscard]] bool isFunctionType() const { return isA<FunctionType>(); }

		[[nodiscard]] Type* getReturnType() const
		{
			assert(isFunctionType());
			return get<FunctionType>().getReturnType();
		}

		[[nodiscard]] Type* getArgumentType(size_t index) const
		{
			assert(isFunctionType());
			return get<FunctionType>().getArgumentType(index);
		}

		[[nodiscard]] bool operator==(const Type& other) const
		{
			return content == other.content;
		}

		[[nodiscard]] bool operator!=(const Type& other) const
		{
			return !(*this == other);
		}

		[[nodiscard]] BuiltinType getBuiltinType() const
		{
			assert(isBuiltin());
			return get<BuiltinType>();
		}

		[[nodiscard]] size_t getArraySize() const
		{
			assert(isArray());
			return get<ArrayType>().size();
		}

		void print(llvm::raw_ostream& out);
		void dump();

		private:
		Type(BuiltinType b): content(b) {}
		Type(Type* t, size_t s): content(ArrayType(t, s)) {}
		Type(Type* t, llvm::ArrayRef<Type*> args): content(FunctionType(t, args)) {}
		Type(llvm::StringRef name, llvm::ArrayRef<Type*> members)
				: content(UserDefinedType(name, members))
		{
		}
		const std::variant<BuiltinType, ArrayType, UserDefinedType, FunctionType>
				content;
	};

	class TypeDB
	{
		public:
		[[nodiscard]] Type* getLongType() { return getBuiltin(BuiltinType::LONG); }
		[[nodiscard]] Type* getBoolType() { return getBuiltin(BuiltinType::BOOL); }
		[[nodiscard]] Type* getDoubleType()
		{
			return getBuiltin(BuiltinType::DOUBLE);
		}
		[[nodiscard]] Type* getVoidType() { return getBuiltin(BuiltinType::VOID); }

		template<BuiltinType b>
		[[nodiscard]] Type* getBuiltin()
		{
			return getBuiltin(b);
		}

		[[nodiscard]] Type* getUserDefined(llvm::StringRef name) const;

		[[nodiscard]] Type* getBuiltin(BuiltinType b);

		[[nodiscard]] Type* createUserDefinedType(
				std::string name, llvm::ArrayRef<Type*> members);

		template<typename... Members>
		[[nodiscard]] Type* createUserDefinedType(
				std::string name, Members&&... members)
		{
			return createUserDefinedType(
					std::move(name), { std::forward<Members>(members)... });
		}

		[[nodiscard]] Type* getUserDefinedType(llvm::StringRef name)
		{
			if (auto f = userDefinedTypes.find(name); f != userDefinedTypes.end())
				return f->second;
			return nullptr;
		}

		[[nodiscard]] Type* getArrayType(Type* tp, size_t size);
		[[nodiscard]] Type* getFunctionType(
				Type* returnType, llvm::ArrayRef<Type*> args);

		template<typename... Args>
		[[nodiscard]] Type* getFunctionType(Type* retType, Args*... args)
		{
			return getFunctionType(retType, { args... });
		}

		private:
		llvm::BumpPtrAllocator allocator;
		llvm::StringMap<Type*> userDefinedTypes;
		std::map<BuiltinType, Type*> builtinTypes;
		std::map<std::pair<Type*, size_t>, Type*> arrayTypes;
		std::map<llvm::SmallVector<Type*, 3>, Type*> functionTypes;
	};
}	 // namespace rlc
