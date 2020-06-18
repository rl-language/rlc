#pragma once

#include <array>
#include <memory>
#include <utility>
#include <variant>

#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/Support/raw_ostream.h"
#include "rlc/ast/Expression.hpp"
#include "rlc/utils/SimpleIterator.hpp"
#include "rlc/utils/SourcePosition.hpp"
namespace rlc
{
	class Statement;
	class ExpressionStatement
	{
		public:
		explicit ExpressionStatement(Expression exp): expression({ std::move(exp) })
		{
		}

		[[nodiscard]] const Expression& getExpression() const
		{
			return expression[0];
		}
		[[nodiscard]] Expression& getExpression() { return expression[0]; }

		[[nodiscard]] auto begin() const { return expression.begin(); }
		[[nodiscard]] auto begin() { return expression.begin(); }
		[[nodiscard]] auto end() const { return expression.end(); }
		[[nodiscard]] auto end() { return expression.end(); }

		[[nodiscard]] size_t subExpCount() const { return expression.size(); }
		[[nodiscard]] const Expression& getSubExp(size_t index) const
		{
			return expression.at(index);
		}
		[[nodiscard]] Expression& getSubExp(size_t index)
		{
			return expression.at(index);
		}

		void print(llvm::raw_ostream& OS, size_t indents = 0) const;
		void dump() const;

		private:
		std::array<Expression, 1> expression;
	};

	class DeclarationStatement
	{
		public:
		explicit DeclarationStatement(std::string name, Expression exp)
				: varName(std::move(name)), expression({ std::move(exp) })
		{
		}

		[[nodiscard]] const Expression& getExpression() const
		{
			return expression[0];
		}
		[[nodiscard]] Expression& getExpression() { return expression[0]; }

		[[nodiscard]] auto begin() const { return expression.begin(); }
		[[nodiscard]] auto begin() { return expression.begin(); }
		[[nodiscard]] auto end() const { return expression.end(); }
		[[nodiscard]] auto end() { return expression.end(); }

		[[nodiscard]] size_t subExpCount() const { return expression.size(); }
		[[nodiscard]] const Expression& getSubExp(size_t index) const
		{
			return expression.at(index);
		}
		[[nodiscard]] Expression& getSubExp(size_t index)
		{
			return expression.at(index);
		}

		[[nodiscard]] const std::string& getName() const { return varName; }
		[[nodiscard]] Type* getType() const { return getExpression().getType(); }

		void print(llvm::raw_ostream& OS, size_t indents = 0) const;
		void dump() const;

		private:
		std::string varName;
		std::array<Expression, 1> expression;
	};

	class ReturnStatement
	{
		public:
		explicit ReturnStatement(Expression exp): expression({ std::move(exp) }) {}
		explicit ReturnStatement() = default;

		[[nodiscard]] bool isVoid() const { return expression.empty(); }
		[[nodiscard]] const Expression& getExpression() const
		{
			return expression[0];
		}
		[[nodiscard]] Expression& getExpression() { return expression[0]; }

		[[nodiscard]] auto begin() const { return expression.begin(); }
		[[nodiscard]] auto begin() { return expression.begin(); }
		[[nodiscard]] auto end() const { return expression.end(); }
		[[nodiscard]] auto end() { return expression.end(); }

		[[nodiscard]] size_t subExpCount() const { return expression.size(); }
		[[nodiscard]] const Expression& getSubExp(size_t index) const
		{
			return expression[index];
		}
		[[nodiscard]] Expression& getSubExp(size_t index)
		{
			return expression[index];
		}

		void print(llvm::raw_ostream& OS, size_t indents = 0) const;
		void dump() const;

		private:
		llvm::SmallVector<Expression, 1> expression;
	};

	class StatementList
	{
		public:
		friend Statement;
		using iterator = SimpleIterator<StatementList&, Statement>;
		using const_iterator =
				SimpleIterator<const StatementList&, const Statement>;

		[[nodiscard]] const_iterator begin() const
		{
			return const_iterator(*this, 0);
		}
		[[nodiscard]] iterator begin() { return iterator(*this, 0); }
		[[nodiscard]] const_iterator end() const
		{
			return const_iterator(*this, subStatCount());
		}
		[[nodiscard]] iterator end() { return iterator(*this, subStatCount()); }

		[[nodiscard]] size_t subStatCount() const { return list.size(); }
		[[nodiscard]] const Statement& getSubStatement(size_t index) const
		{
			return *list[index];
		}
		[[nodiscard]] Statement& getSubStatement(size_t index)
		{
			return *list[index];
		}
		void print(llvm::raw_ostream& OS, size_t indents = 0) const;
		void dump() const;

		StatementList(StatementList&& other) = default;
		StatementList& operator=(StatementList&& other) = default;
		StatementList(const StatementList& other);
		StatementList& operator=(const StatementList& other);
		~StatementList() = default;

		using Container = llvm::SmallVector<std::unique_ptr<Statement>, 2>;

		private:
		explicit StatementList(Container c): list(std::move(c)) {}
		Container list;
	};

	class IfStatement
	{
		public:
		friend Statement;
		using iterator = SimpleIterator<IfStatement&, Statement>;
		using const_iterator = SimpleIterator<const IfStatement&, const Statement>;

		[[nodiscard]] bool hasFalseBranch() const { return branches.size() == 2; }
		[[nodiscard]] const Expression& getCondition() const
		{
			return condition[0];
		}
		[[nodiscard]] Expression& getCondition() { return condition[0]; }
		[[nodiscard]] const_iterator begin() const
		{
			return const_iterator(*this, 0);
		}
		[[nodiscard]] iterator begin() { return iterator(*this, 0); }
		[[nodiscard]] const_iterator end() const
		{
			return const_iterator(*this, subStatCount());
		}
		[[nodiscard]] iterator end() { return iterator(*this, subStatCount()); }

		[[nodiscard]] auto expBegin() const { return condition.begin(); }
		[[nodiscard]] auto expBegin() { return condition.begin(); }
		[[nodiscard]] auto expEnd() const { return condition.end(); }
		[[nodiscard]] auto expEnd() { return condition.end(); }
		[[nodiscard]] auto expRange() const
		{
			return llvm::make_range(expBegin(), expEnd());
		}
		[[nodiscard]] auto expRange()
		{
			return llvm::make_range(expBegin(), expEnd());
		}

		[[nodiscard]] const Statement& getTrueBranch() const
		{
			return *branches[0];
		}
		[[nodiscard]] Statement& getTrueBranch() { return *branches[0]; }
		[[nodiscard]] const Statement& getFalseBranch() const
		{
			assert(hasFalseBranch());
			return *branches[1];
		}
		[[nodiscard]] Statement& getFalseBranch()
		{
			assert(hasFalseBranch());
			return *branches[1];
		}

		[[nodiscard]] size_t subExpCount() const { return condition.size(); }
		[[nodiscard]] const Expression& getSubExp(size_t index) const
		{
			return condition.at(index);
		}
		[[nodiscard]] Expression& getSubExp(size_t index)
		{
			return condition.at(index);
		}

		[[nodiscard]] size_t subStatCount() const { return branches.size(); }
		[[nodiscard]] const Statement& getSubStatement(size_t index) const
		{
			return *branches[index];
		}
		[[nodiscard]] Statement& getSubStatement(size_t index)
		{
			return *branches[index];
		}
		void print(llvm::raw_ostream& OS, size_t indents = 0) const;
		void dump() const;

		IfStatement(IfStatement&& other) = default;
		IfStatement& operator=(IfStatement&& other) = default;
		IfStatement(const IfStatement& other);
		IfStatement& operator=(const IfStatement& other);
		~IfStatement() = default;

		private:
		IfStatement(Expression cond, std::unique_ptr<Statement> trueBranch)
				: condition({ std::move(cond) })
		{
			branches.emplace_back(move(trueBranch));
		}
		IfStatement(
				Expression cond,
				std::unique_ptr<Statement> trueBranch,
				std::unique_ptr<Statement> falseBranch)
				: condition({ std::move(cond) })
		{
			branches.emplace_back(move(trueBranch));
			branches.emplace_back(move(falseBranch));
		}
		std::array<Expression, 1> condition;
		llvm::SmallVector<std::unique_ptr<Statement>, 2> branches;
	};

	class WhileStatement
	{
		public:
		friend Statement;
		using iterator = SimpleIterator<WhileStatement&, Statement>;
		using const_iterator =
				SimpleIterator<const WhileStatement&, const Statement>;

		[[nodiscard]] const Expression& getCondition() const
		{
			return condition[0];
		}
		[[nodiscard]] Expression& getCondition() { return condition[0]; }
		[[nodiscard]] const_iterator begin() const
		{
			return const_iterator(*this, 0);
		}
		[[nodiscard]] iterator begin() { return iterator(*this, 0); }
		[[nodiscard]] const_iterator end() const
		{
			return const_iterator(*this, subStatCount());
		}
		[[nodiscard]] iterator end() { return iterator(*this, subStatCount()); }

		[[nodiscard]] auto expBegin() const { return condition.begin(); }
		[[nodiscard]] auto expBegin() { return condition.begin(); }
		[[nodiscard]] auto expEnd() const { return condition.end(); }
		[[nodiscard]] auto expEnd() { return condition.end(); }
		[[nodiscard]] auto expRange() const
		{
			return llvm::make_range(expBegin(), expEnd());
		}
		[[nodiscard]] auto expRange()
		{
			return llvm::make_range(expBegin(), expEnd());
		}

		[[nodiscard]] const Statement& getBody() const { return *body[0]; }
		[[nodiscard]] Statement& getBody() { return *body[0]; }

		[[nodiscard]] size_t subExpCount() const { return condition.size(); }
		[[nodiscard]] const Expression& getSubExp(size_t index) const
		{
			return condition.at(index);
		}
		[[nodiscard]] Expression& getSubExp(size_t index)
		{
			return condition.at(index);
		}

		[[nodiscard]] size_t subStatCount() const { return body.size(); }
		[[nodiscard]] const Statement& getSubStatement(size_t index) const
		{
			return *body.at(index);
		}
		[[nodiscard]] Statement& getSubStatement(size_t index)
		{
			return *body.at(index);
		}
		void print(llvm::raw_ostream& OS, size_t indents = 0) const;
		void dump() const;

		WhileStatement(WhileStatement&& other) = default;
		WhileStatement& operator=(WhileStatement&& other) = default;
		WhileStatement(const WhileStatement& other);
		WhileStatement& operator=(const WhileStatement& other);
		~WhileStatement() = default;

		private:
		WhileStatement(Expression cond, std::unique_ptr<Statement> bod)
				: condition({ std::move(cond) })
		{
			body[0] = std::move(bod);
		}
		std::array<Expression, 1> condition;
		std::array<std::unique_ptr<Statement>, 1> body;
	};

	class Statement
	{
		public:
		using ExpIterator = SimpleIterator<Statement&, Expression>;
		using ConstExpIterator = SimpleIterator<const Statement&, const Expression>;

		using StatIterator = SimpleIterator<Statement&, Statement>;
		using ConstStatIterator = SimpleIterator<const Statement&, const Statement>;

		template<typename Stat>
		Statement(Stat&& cont, SourcePosition position)
				: position(std::move(position)), content(std::forward<Stat>(cont))
		{
		}
		[[nodiscard]] const SourcePosition& getPosition() const { return position; }

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

		[[nodiscard]] bool isIfStat() const { return isA<IfStatement>(); }
		[[nodiscard]] bool isDeclarationStatement() const
		{
			return isA<DeclarationStatement>();
		}
		[[nodiscard]] bool isWhileStat() const { return isA<WhileStatement>(); }
		[[nodiscard]] bool isStatmentList() const { return isA<StatementList>(); }
		[[nodiscard]] bool isReturnStatement() const
		{
			return isA<ReturnStatement>();
		}

		[[nodiscard]] bool isExpStat() const { return isA<ExpressionStatement>(); }

		[[nodiscard]] const ExpressionStatement& expStatement() const
		{
			return get<ExpressionStatement>();
		}

		[[nodiscard]] ExpressionStatement& expStatement()
		{
			return get<ExpressionStatement>();
		}

		template<typename T>
		auto visit(T&& visitor) -> decltype(visitor(expStatement()))
		{
			return std::visit(std::forward<T>(visitor), content);
		}
		template<typename T>
		auto visit(T&& visitor) const -> decltype(visitor(expStatement()))
		{
			return std::visit(std::forward<T>(visitor), content);
		}

		[[nodiscard]] size_t subExpCount() const;
		[[nodiscard]] size_t subStatmentCount() const;
		[[nodiscard]] const Statement& getSubStatement(size_t index) const;
		[[nodiscard]] Statement& getSubStatement(size_t index);
		[[nodiscard]] const Expression& getSubExp(size_t index) const;
		[[nodiscard]] Expression& getSubExp(size_t index);

		[[nodiscard]] ExpIterator expBegin() { return ExpIterator(*this, 0); }
		[[nodiscard]] ConstExpIterator expBegin() const
		{
			return ConstExpIterator(*this, 0);
		}
		[[nodiscard]] ConstExpIterator expEnd() const
		{
			return ConstExpIterator(*this, subExpCount());
		}
		[[nodiscard]] ExpIterator expEnd()
		{
			return ExpIterator(*this, subExpCount());
		}
		[[nodiscard]] auto expRange() const
		{
			return llvm::make_range(expBegin(), expEnd());
		}

		[[nodiscard]] auto expRange()
		{
			return llvm::make_range(expBegin(), expEnd());
		}

		[[nodiscard]] StatIterator begin() { return StatIterator(*this, 0); }
		[[nodiscard]] ConstStatIterator begin() const
		{
			return ConstStatIterator(*this, 0);
		}
		[[nodiscard]] ConstStatIterator end() const
		{
			return ConstStatIterator(*this, subExpCount());
		}
		[[nodiscard]] StatIterator end()
		{
			return StatIterator(*this, subExpCount());
		}

		void print(
				llvm::raw_ostream& OS,
				size_t indents = 0,
				bool printPosition = false) const;
		void dump() const;

		static Statement ifStatment(
				Expression exp,
				Statement trueB,
				Statement falseB,
				SourcePosition pos = SourcePosition());
		static Statement ifStatment(
				Expression exp, Statement trueB, SourcePosition pos = SourcePosition());

		static Statement expStatement(
				Expression exp, SourcePosition = SourcePosition());

		static Statement declarationStatement(
				std::string name, Expression exp, SourcePosition = SourcePosition());

		static Statement whileStatement(
				Expression cond, Statement body, SourcePosition pos = SourcePosition())
		{
			WhileStatement s(
					std::move(cond), std::make_unique<Statement>(std::move(body)));
			return Statement(std::move(s), std::move(pos));
		}

		static Statement statmentList(
				llvm::SmallVector<Statement, 3> statement,
				SourcePosition pos = SourcePosition());

		static Statement returnStatement(
				Expression returnExpression, SourcePosition pos = SourcePosition())
		{
			return Statement(
					ReturnStatement(std::move(returnExpression)), std::move(pos));
		}

		static Statement returnStatement(SourcePosition pos = SourcePosition())
		{
			return Statement(ReturnStatement(), std::move(pos));
		}

		private:
		SourcePosition position;
		std::variant<
				ExpressionStatement,
				IfStatement,
				WhileStatement,
				StatementList,
				ReturnStatement,
				DeclarationStatement>
				content;
	};
}	 // namespace rlc
