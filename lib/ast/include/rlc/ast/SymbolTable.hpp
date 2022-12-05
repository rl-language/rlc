#pragma once

#include <iterator>
#include <map>
#include <utility>
#include <variant>

#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/iterator.h"
#include "llvm/ADT/iterator_range.h"
#include "rlc/ast/BuiltinFunctions.hpp"
#include "rlc/ast/Type.hpp"
namespace rlc
{
	class Entity;
	class System;
	class FunctionDeclaration;
	class DeclarationStatement;
	class ArgumentDeclaration;
	class Symbol
	{
		public:
		template<typename T>
		explicit Symbol(const T& symbol): content(&symbol)
		{
		}

		template<typename T>
		[[nodiscard]] bool isA() const
		{
			return std::holds_alternative<const T*>(content);
		}

		[[nodiscard]] bool isEntity() const { return isA<Entity>(); }

		template<typename T>
		[[nodiscard]] const T& get() const
		{
			assert(isA<T>());
			return *std::get<const T*>(content);
		}

		[[nodiscard]] std::string getName() const;

		template<typename Visitor>
		auto visit(Visitor&& visitor) const
				-> decltype(visitor(get<DeclarationStatement>()))
		{
			return std::visit(
					[&](const auto* ptr) { return visitor(*ptr); }, content);
		}
		[[nodiscard]] Type* getType() const;

		[[nodiscard]] bool hasType() const
		{
			return isA<FunctionDeclaration>() or isA<DeclarationStatement>() or
						 isA<ArgumentDeclaration>();
		}

		bool operator<(const Symbol& other) const
		{
			return content < other.content;
		}

		// returns true if when this symbol is lowerer, its value can be referenced
		[[nodiscard]] bool hasAddress() const
		{
			return isA<FunctionDeclaration>() or isA<DeclarationStatement>() or
						 isA<ArgumentDeclaration>();
		}

		void print(llvm::raw_ostream& OS, size_t indents = 0) const;
		void dump() const;

		private:
		std::variant<
				const Entity*,
				const System*,
				const FunctionDeclaration*,
				const DeclarationStatement*,
				const ArgumentDeclaration*>
				content;
	};

	template<typename Iterator, typename Selected>
	class SymbolFilteringIterator
			: public llvm::iterator_facade_base<
						SymbolFilteringIterator<Iterator, Selected>,
						std::bidirectional_iterator_tag,
						const Selected,
						size_t>
	{
		public:
		SymbolFilteringIterator(Iterator i, size_t maxAdv)
				: content(i), maxAdances(maxAdv)
		{
			while (!isSelected() and maxAdances != 0)
			{
				content++;
				maxAdances--;
			}
		}
		[[nodiscard]] bool operator==(const SymbolFilteringIterator& other) const
		{
			return content == other.content;
		}
		[[nodiscard]] bool operator!=(const SymbolFilteringIterator& other) const
		{
			return content != other.content;
		}
		const Selected& operator*() const { return getSymbol(); }
		SymbolFilteringIterator operator++()
		{
			auto t = *this;
			if (maxAdances == 0)
				return t;

			do
			{
				content++;
				maxAdances--;
			} while (!isSelected() and maxAdances != 0);

			return t;
		}

		const Selected& getSymbol() const
		{
			assert(isSelected());
			return content->second.template get<Selected>();
		}

		[[nodiscard]] bool isSelected() const
		{
			return content->second.template isA<Selected>();
		}

		SymbolFilteringIterator operator--()
		{
			auto t = *this;

			do
			{
				content--;
				maxAdances++;
			} while (!isSelected());

			return t;
		}

		private:
		Iterator content;
		size_t maxAdances;
	};

	class SymbolTable
	{
		public:
		using Map = std::multimap<llvm::StringRef, Symbol>;
		using const_iterator = Map::const_iterator;
		using iterator_range = Map::iterator;
		using const_iterator_range = llvm::iterator_range<const_iterator>;
		using iterator = Map::iterator;
		template<typename Filt>
		using filt_const_iterator = SymbolFilteringIterator<const_iterator, Filt>;

		template<typename Filt>
		using filt_const_iterator_range =
				llvm::iterator_range<filt_const_iterator<Filt>>;

		explicit SymbolTable(const SymbolTable* parent = nullptr): parent(parent) {}
		[[nodiscard]] bool contains(llvm::StringRef name) const;
		[[nodiscard]] const_iterator_range range(llvm::StringRef name) const;

		[[nodiscard]] auto begin() const { return symbols.begin(); }
		[[nodiscard]] auto end() const { return symbols.end(); }
		void insert(Symbol s) { symbols.emplace(s.getName(), s); }

		template<typename T>
		void insert(const T& s)
		{
			symbols.emplace(s.getName(), Symbol(s));
		}
		template<typename Selected>
		[[nodiscard]] filt_const_iterator_range<Selected> range(
				llvm::StringRef name) const
		{
			using Iterator = filt_const_iterator<Selected>;
			auto r = range(name);
			auto d = std::distance(r.begin(), r.end());
			auto b = Iterator(r.begin(), d);
			auto e = Iterator(r.end(), 0);
			return llvm::make_range(b, e);
		}

		const FunctionDeclaration* findOverload(BuiltinFunctions f, Type* t) const
		{
			return findOverload(builtinFunctionsToString(f), t);
		}

		const FunctionDeclaration* findOverload(
				llvm::StringRef mame, Type* t) const;

		template<typename Selected>
		[[nodiscard]] const Selected& getUnique(llvm::StringRef name) const
		{
			auto r = range<Selected>(name);
			assert(std::distance(r.begin(), r.end()) == 1);
			return *r.begin();
		}

		void print(llvm::raw_ostream& OS, size_t indents = 0) const;
		void dump() const;

		[[nodiscard]] bool directContain(llvm::StringRef name) const
		{
			return symbols.find(name) != symbols.end();
		}

		private:
		Map symbols;
		const SymbolTable* parent;
	};
}	 // namespace rlc
