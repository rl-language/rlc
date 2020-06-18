#pragma once

#include <utility>
#include <variant>

#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"
#include "rlc/ast/Type.hpp"
namespace rlc
{
	class Entity;
	class System;
	class FunctionDefinition;
	class DeclarationStatement;
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

		template<typename T>
		[[nodiscard]] const T& get() const
		{
			assert(isA<T>());
			return *std::get<const T*>(content);
		}

		[[nodiscard]] llvm::StringRef getName() const;

		template<typename Visitor>
		auto visit(Visitor&& visitor) const
				-> decltype(visitor(get<DeclarationStatement>()))
		{
			return std::visit(
					[&](const auto* ptr) { return visitor(*ptr); }, content);
		}
		[[nodiscard]] Type* getType() const;

		private:
		std::variant<
				const Entity*,
				const System*,
				const FunctionDefinition*,
				const DeclarationStatement*>
				content;
	};

	class SymbolTable
	{
		public:
		explicit SymbolTable(const SymbolTable* parent): parent(parent) {}
		[[nodiscard]] bool contains(llvm::StringRef name) const;
		[[nodiscard]] const Symbol& get(llvm::StringRef name) const;

		[[nodiscard]] auto begin() const { return symbols.begin(); }
		[[nodiscard]] auto end() const { return symbols.end(); }
		void insert(Symbol s) { symbols.try_emplace(s.getName(), s); }

		template<typename T>
		void insert(const T& s)
		{
			symbols.try_emplace(s.getName(), Symbol(s));
		}

		private:
		llvm::StringMap<Symbol> symbols;
		const SymbolTable* parent;
	};
}	 // namespace rlc
