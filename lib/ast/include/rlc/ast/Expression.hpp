#pragma once

#include <initializer_list>
#include <utility>
#include <variant>

#include "llvm/ADT/GraphTraits.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Support/Error.h"
#include "llvm/Support/raw_ostream.h"
#include "rlc/ast/ArrayAccess.hpp"
#include "rlc/ast/Call.hpp"
#include "rlc/ast/Constant.hpp"
#include "rlc/ast/MemberAccess.hpp"
#include "rlc/ast/Reference.hpp"
#include "rlc/ast/SymbolTable.hpp"
#include "rlc/ast/Type.hpp"
#include "rlc/ast/ZeroInitializer.hpp"
#include "rlc/utils/SimpleIterator.hpp"
#include "rlc/utils/SourcePosition.hpp"

namespace rlc
{
	class Expression
	{
		public:
		friend llvm::yaml::MappingTraits<rlc::Expression>;
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
		void print(
				llvm::raw_ostream& OS = llvm::outs(),
				size_t indents = 0,
				bool printLocation = false) const;
		void dump() const;

		template<typename Visitor>
		[[nodiscard]] auto visit(Visitor&& visitor)
				-> decltype(visitor(std::declval<Call&>()))
		{
			return std::visit(std::forward<Visitor>(visitor), content);
		}

		template<typename Visitor>
		[[nodiscard]] auto visit(Visitor&& visitor) const
				-> decltype(visitor(std::declval<const Call&>()))
		{
			return std::visit(std::forward<Visitor>(visitor), content);
		}

		[[nodiscard]] Expression& subExpression(size_t index)
		{
			if (isCall())
				return getCall().getSubExpression(index);
			if (isA<ArrayAccess>())
				return get<ArrayAccess>().getSubExpression(index);
			if (isA<MemberAccess>())
				return get<MemberAccess>().getExp();

			assert(false && "urechable");
		}

		[[nodiscard]] const Expression& subExpression(size_t index) const
		{
			if (isCall())
				return getCall().getSubExpression(index);
			if (isA<ArrayAccess>())
				return get<ArrayAccess>().getSubExpression(index);
			if (isA<MemberAccess>())
				return get<MemberAccess>().getExp();
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
		static Expression scalarConstant(
				T constant, SourcePosition position = SourcePosition())
		{
			return Expression(ScalarConstant(constant, std::move(position)), nullptr);
		}

		static Expression zeroInitializer(
				SingleTypeUse use, SourcePosition position = SourcePosition())
		{
			return Expression(
					ZeroInitializer(std::move(use), std::move(position)), nullptr);
		}

		static Expression call(
				Expression call,
				llvm::SmallVector<Expression, 3> args = {},
				SourcePosition position = SourcePosition());

		static Expression reference(
				std::string refName, SourcePosition position = SourcePosition())
		{
			return Expression(
					Reference(std::move(refName), std::move(position)), nullptr);
		}

		static Expression reference(
				BuiltinFunctions fun, SourcePosition position = SourcePosition())
		{
			return Expression(Reference(fun, std::move(position)), nullptr);
		}

		template<typename T>
		explicit Expression(T&& c, Type* t): content(std::forward<T>(c)), type(t)
		{
		}

		static Expression doubleConstant(
				double d, SourcePosition position = SourcePosition())
		{
			return scalarConstant<double>(d, std::move(position));
		}

		static Expression int64Constant(
				int64_t d, SourcePosition position = SourcePosition())
		{
			return scalarConstant<int64_t>(d, std::move(position));
		}

		static Expression boolConstant(
				bool d, SourcePosition position = SourcePosition())
		{
			return scalarConstant<bool>(d, std::move(position));
		}

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
			auto position = getPosition();
			return call(
					reference(rlc::BuiltinFunctions::Add, position),
					{ std::move(*this), std::move(other) },
					position);
		}

		[[nodiscard]] Expression operator||(Expression&& other)
		{
			auto position = getPosition();
			return call(
					reference(BuiltinFunctions::Or, position),
					{ std::move(*this), std::move(other) },
					position);
		}

		[[nodiscard]] Expression operator+(Expression&& other)
		{
			auto position = getPosition();
			return call(
					reference(BuiltinFunctions::Add, position),
					{ std::move(*this), std::move(other) },
					position);
		}

		[[nodiscard]] Expression operator-(Expression&& other)
		{
			auto position = getPosition();
			return call(
					reference(BuiltinFunctions::Subtract, position),
					{ std::move(*this), std::move(other) },
					position);
		}

		[[nodiscard]] Expression operator/(Expression&& other)
		{
			auto position = getPosition();
			return call(
					reference(BuiltinFunctions::Divide, position),
					{ std::move(*this), std::move(other) },
					position);
		}

		[[nodiscard]] Expression operator*(Expression&& other)
		{
			auto position = getPosition();
			return call(
					reference(BuiltinFunctions::Multiply, position),
					{ std::move(*this), std::move(other) },
					position);
		}

		[[nodiscard]] Expression& operator+=(Expression&& other)
		{
			auto position = getPosition();
			*this = call(
					reference(BuiltinFunctions::Add, position),
					{ std::move(*this), std::move(other) },
					position);
			return *this;
		}

		[[nodiscard]] Expression& operator-=(Expression&& other)
		{
			auto position = getPosition();
			*this = call(
					reference(BuiltinFunctions::Subtract, position),
					{ std::move(*this), std::move(other) },
					position);
			return *this;
		}

		[[nodiscard]] Expression& operator/=(Expression&& other)
		{
			auto position = getPosition();
			*this = call(
					reference(BuiltinFunctions::Divide, position),
					{ std::move(*this), std::move(other) },
					position);
			return *this;
		}

		[[nodiscard]] Expression& operator*=(Expression&& other)
		{
			auto position = getPosition();
			*this = call(
					reference(BuiltinFunctions::Multiply, position),
					{ std::move(*this), std::move(other) },
					position);
			return *this;
		}

		[[nodiscard]] Expression operator<(Expression&& other)
		{
			auto position = getPosition();
			return call(
					reference(BuiltinFunctions::Less, position),
					{ std::move(*this), std::move(other) },
					position);
		}

		[[nodiscard]] Expression operator<=(Expression&& other)
		{
			auto position = getPosition();
			return call(
					reference(BuiltinFunctions::LessEqual, position),
					{ std::move(*this), std::move(other) },
					position);
		}

		[[nodiscard]] Expression operator>(Expression&& other)
		{
			auto position = getPosition();
			return call(
					reference(BuiltinFunctions::Greater, position),
					{ std::move(*this), std::move(other) },
					position);
		}

		[[nodiscard]] Expression operator>=(Expression&& other)
		{
			auto position = getPosition();
			return call(
					reference(BuiltinFunctions::GrearEqual, position),
					{ std::move(*this), std::move(other) },
					position);
		}

		[[nodiscard]] static Expression arrayAccess(
				Expression lhs, Expression rhs, SourcePosition pos)
		{
			return Expression(
					ArrayAccess(
							std::make_unique<Expression>(std::move(lhs)),
							std::make_unique<Expression>(std::move(rhs)),
							std::move(pos)),
					nullptr);
		}

		[[nodiscard]] Expression operator[](Expression&& other)
		{
			auto position = getPosition();
			return arrayAccess(std::move(*this), std::move(other), position);
		}

		static Expression memberAccess(
				Expression&& leftHand,
				std::string memberName,
				SourcePosition position = SourcePosition())
		{
			return Expression(
					MemberAccess(
							std::move(leftHand), std::move(memberName), std::move(position)),
					nullptr);
		}

		void setType(Type* tp) { type = tp; }

		static Expression assign(
				Expression&& leftHand,
				Expression&& rightHand,
				SourcePosition position = SourcePosition())
		{
			return call(
					reference(BuiltinFunctions::Assign, position),
					{ std::move(leftHand), std::move(rightHand) },
					position);
		}

		llvm::Error deduceType(const SymbolTable& tb, TypeDB& db);

		[[nodiscard]] const SourcePosition& getPosition() const
		{
			return visit([](const auto& Entry) -> const SourcePosition& {
				return Entry.getPosition();
			});
		}

		void setPosition(const SourcePosition& newPoisition)
		{
			visit([&newPoisition](auto& Entry) -> void {
				Entry.setPosition(newPoisition);
			});
		}

		private:
		std::variant<
				ZeroInitializer,
				ScalarConstant,
				Call,
				Reference,
				MemberAccess,
				ArrayAccess>
				content;
		Type* type;
	};

	class ExpressionGraph
	{
		public:
		rlc::Expression* graph;
	};

}	 // namespace rlc

template<>
struct llvm::yaml::MappingTraits<rlc::Expression>
{
	static void mapping(IO& io, rlc::Expression& value)
	{
		assert(io.outputting());
		value.visit([&]<typename T>(T& Value) -> void {
			io.mapTag(T::name, true);
			llvm::yaml::MappingTraits<T>::mapping(io, Value);
		});
		if (value.type != nullptr)
			io.mapRequired("type", *value.type);
	}
};

template<>
struct llvm::GraphTraits<rlc::ExpressionGraph>
{
	class Node
	{
		public:
		rlc::Expression* exp;
		bool operator==(const Node& other) const = default;
		Node(rlc::Expression& exp): exp(&exp) {}
	};
	using NodeRef = Node;
	using ChildIteratorType = rlc::Expression::iterator;

	static NodeRef getEntryNode(const rlc::ExpressionGraph& exp)
	{
		return *exp.graph;
	}

	static ChildIteratorType child_begin(NodeRef current)
	{
		return current.exp->begin();
	}
	static ChildIteratorType child_end(NodeRef current)
	{
		return current.exp->end();
	}
};

template<>
struct llvm::PointerLikeTypeTraits<
		llvm::GraphTraits<rlc::ExpressionGraph>::Node>
{
	static void* getAsVoidPointer(
			llvm::GraphTraits<rlc::ExpressionGraph>::Node node)
	{
		return (void*) node.exp;
	}

	static llvm::GraphTraits<rlc::ExpressionGraph>::Node getFromVoidPtr(
			void* node)
	{
		auto* stm = static_cast<rlc::Expression*>(node);
		return *stm;
	}
};
