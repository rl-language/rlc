#pragma once

#include <utility>
#include <variant>

#include "llvm/ADT/STLExtras.h"
#include "llvm/Support/raw_ostream.h"
#include "rlc/ast/Call.hpp"
#include "rlc/ast/Constant.hpp"
#include "rlc/ast/Reference.hpp"
#include "rlc/utils/SimpleBiIterator.hpp"

namespace rlc
{
	class Expression;

	class Expression
	{
		public:
		using iterator = SimpleBiIterator<Expression&, Expression>;
		using const_iterator =
				SimpleBiIterator<const Expression&, const Expression>;
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
		{
			return std::visit(std::forward<Visitor>(visitor), content);
		}

		template<typename Visitor>
		[[nodiscard]] auto visit(Visitor&& visitor)
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

		template<typename... Args>
		static Expression call(Expression call, Args&&... args)
		{
			return Expression(
					Call(
							llvm::make_unique<Expression>(std::move(call)),
							{ llvm::make_unique<Expression>(std::forward<Args>(args))... }),
					nullptr);
		}

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

		private:
		std::variant<ScalarConstant, Call, Reference> content;
		Type* type;
	};
}	 // namespace rlc
