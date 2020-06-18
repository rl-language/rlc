#pragma once

#include <initializer_list>
#include <utility>
#include <variant>

#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Support/Error.h"
#include "llvm/Support/raw_ostream.h"
#include "rlc/ast/Call.hpp"
#include "rlc/ast/Constant.hpp"
#include "rlc/ast/Reference.hpp"
#include "rlc/ast/SymbolTable.hpp"
#include "rlc/ast/Type.hpp"
#include "rlc/utils/SimpleIterator.hpp"

namespace rlc
{
	class Expression
	{
		public:
		using iterator = SimpleIterator<Expression&, Expression>;
		using const_iterator = SimpleIterator<const Expression&, const Expression>;
		[[nodiscard]] Type* getType() const { return type; }

		template<typename T>
		[[nodiscard]] bool isA() const
		{
			return std::holds_alternative<T>(content);
		}

		template<typename T>
		[[nodiscard]] const T& get() const
		{
			assert(isA<T>());
			return std::get<T>(content);
		}

		template<typename T>
		[[nodiscard]] T& get()
		{
			assert(isA<T>());
			return std::get<T>(content);
		}

		[[nodiscard]] bool isCall() const { return isA<Call>(); }
		[[nodiscard]] const Call& getCall() const { return get<Call>(); }
		[[nodiscard]] Call& getCall() { return get<Call>(); }
		void print(llvm::raw_ostream& OS = llvm::outs(), size_t indents = 0) const;
		void dump() const;

		template<typename Visitor>
		[[nodiscard]] auto visit(Visitor&& visitor) const
				-> decltype(visitor(getCall()))
		{
			return std::visit(std::forward<Visitor>(visitor), content);
		}

		template<typename Visitor>
		[[nodiscard]] auto visit(Visitor&& visitor) -> decltype(visitor(getCall()))
		{
			return std::visit(std::forward<Visitor>(visitor), content);
		}

		[[nodiscard]] Expression& subExpression(size_t index)
		{
			assert(isCall());
			return getCall().getSubExpression(index);
		}

		[[nodiscard]] const Expression& subExpression(size_t index) const
		{
			assert(isCall());
			return getCall().getSubExpression(index);
		}

		[[nodiscard]] iterator begin() { return iterator(*this); }
		[[nodiscard]] const_iterator begin() const { return const_iterator(*this); }
		[[nodiscard]] iterator end()
		{
			return iterator(*this, subExpressionCount());
		}
		[[nodiscard]] const_iterator end() const
		{
			return const_iterator(*this, subExpressionCount());
		}
		[[nodiscard]] size_t subExpressionCount() const;

		template<typename T>
		static Expression scalarConstant(T constant)
		{
			return Expression(ScalarConstant(constant), nullptr);
		}

		static Expression call(
				Expression call, llvm::SmallVector<Expression, 3> args = {});

		static Expression reference(std::string refName)
		{
			return Expression(Reference(std::move(refName)), nullptr);
		}

		template<typename T>
		explicit Expression(T&& c, Type* t): content(std::forward<T>(c)), type(t)
		{
		}

		static Expression doubleConstant(double d)
		{
			return scalarConstant<double>(d);
		}

		static Expression int64Constant(int64_t d)
		{
			return scalarConstant<int64_t>(d);
		}

		static Expression boolConstant(bool d) { return scalarConstant<bool>(d); }

		[[nodiscard]] bool operator==(const Expression& other) const
		{
			return content == other.content and type == other.type;
		}

		[[nodiscard]] bool operator!=(const Expression& other) const
		{
			return !(*this == other);
		}

		[[nodiscard]] Expression operator&&(Expression&& other)
		{
			return call(reference("and"), { std::move(*this), std::move(other) });
		}

		[[nodiscard]] Expression operator||(Expression&& other)
		{
			return call(reference("or"), { std::move(*this), std::move(other) });
		}

		[[nodiscard]] Expression operator+(Expression&& other)
		{
			return call(reference("add"), { std::move(*this), std::move(other) });
		}

		[[nodiscard]] Expression operator-(Expression&& other)
		{
			return call(
					reference("subtract"), { std::move(*this), std::move(other) });
		}

		[[nodiscard]] Expression operator/(Expression&& other)
		{
			return call(reference("divide"), { std::move(*this), std::move(other) });
		}

		[[nodiscard]] Expression operator*(Expression&& other)
		{
			return call(
					reference("multiply"), { std::move(*this), std::move(other) });
		}

		[[nodiscard]] Expression& operator+=(Expression&& other)
		{
			*this = call(reference("add"), { std::move(*this), std::move(other) });
			return *this;
		}

		[[nodiscard]] Expression& operator-=(Expression&& other)
		{
			*this =
					call(reference("subtract"), { std::move(*this), std::move(other) });
			return *this;
		}

		[[nodiscard]] Expression& operator/=(Expression&& other)
		{
			*this = call(reference("divide"), { std::move(*this), std::move(other) });
			return *this;
		}

		[[nodiscard]] Expression& operator*=(Expression&& other)
		{
			*this =
					call(reference("multiply"), { std::move(*this), std::move(other) });
			return *this;
		}

		[[nodiscard]] Expression operator<(Expression&& other)
		{
			return call(reference("less"), { std::move(*this), std::move(other) });
		}

		[[nodiscard]] Expression operator<=(Expression&& other)
		{
			return call(
					reference("lessEqual"), { std::move(*this), std::move(other) });
		}

		[[nodiscard]] Expression operator>(Expression&& other)
		{
			return call(reference("greater"), { std::move(*this), std::move(other) });
		}

		[[nodiscard]] Expression operator>=(Expression&& other)
		{
			return call(
					reference("greatherEqual"), { std::move(*this), std::move(other) });
		}

		[[nodiscard]] Expression operator[](Expression&& other)
		{
			return call(
					reference("arrayAccess"), { std::move(*this), std::move(other) });
		}

		static Expression memberAccess(
				Expression&& leftHand, std::string memberName)
		{
			return call(
					reference("memberAccess"),
					{ std::move(leftHand), reference(std::move(memberName)) });
		}

		static Expression assign(Expression&& leftHand, Expression&& rightHand)
		{
			return call(
					reference("assign"), { std::move(leftHand), std::move(rightHand) });
		}

		llvm::Error deduceType(const SymbolTable& tb, TypeDB& db);

		private:
		std::variant<ScalarConstant, Call, Reference> content;
		Type* type;
	};
}	 // namespace rlc
