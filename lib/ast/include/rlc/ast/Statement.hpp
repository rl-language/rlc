#pragma once

#include <array>
#include <memory>
#include <utility>
#include <variant>

#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/Support/raw_ostream.h"
#include "rlc/ast/ActionDeclaration.hpp"
#include "rlc/ast/Expression.hpp"
#include "rlc/ast/Type.hpp"
#include "rlc/utils/SimpleIterator.hpp"
#include "rlc/utils/SourcePosition.hpp"
namespace rlc
{
	class Statement;
	class SymbolTable;
	class ExpressionStatement
	{
		public:
		friend llvm::yaml::MappingTraits<rlc::ExpressionStatement>;
		using iterator = std::array<Expression, 1>::iterator;
		using const_iterator = std::array<Expression, 1>::const_iterator;
		static constexpr const char* name = "expression_statement";

		explicit ExpressionStatement(
				Expression exp, SourcePosition position = SourcePosition())
				: expression({ std::move(exp) }), position(std::move(position))
		{
		}

		[[nodiscard]] const Expression& getExpression() const
		{
			return expression[0];
		}
		[[nodiscard]] Expression& getExpression() { return expression[0]; }

		[[nodiscard]] const_iterator begin() const { return expression.begin(); }
		[[nodiscard]] iterator begin() { return expression.begin(); }
		[[nodiscard]] const_iterator end() const { return expression.end(); }
		[[nodiscard]] iterator end() { return expression.end(); }

		[[nodiscard]] size_t subExpCount() const { return expression.size(); }
		[[nodiscard]] const Expression& getSubExp(size_t index) const
		{
			return expression.at(index);
		}
		[[nodiscard]] Expression& getSubExp(size_t index)
		{
			return expression.at(index);
		}

		void print(
				llvm::raw_ostream& OS,
				size_t indents = 0,
				bool printPosition = false) const;
		void dump() const;
		llvm::Error deduceTypes(const SymbolTable& tb, TypeDB& db);
		[[nodiscard]] const SourcePosition& getPosition() const { return position; }

		private:
		std::array<Expression, 1> expression;
		SourcePosition position;
	};

	class ActionStatement
	{
		public:
		friend llvm::yaml::MappingTraits<rlc::ActionStatement>;
		static constexpr const char* name = "action_statement";

		ActionStatement(ActionDeclaration declaration): decl(std::move(declaration))
		{
		}

		void print(
				llvm::raw_ostream& OS,
				size_t indents = 0,
				bool printPosition = false) const;
		void dump() const;
		llvm::Error deduceTypes(SymbolTable& tb, TypeDB& db);

		[[nodiscard]] const SourcePosition& getPosition() const
		{
			return decl.getSourcePosition();
		}

		ActionDeclaration& getDeclaration() { return decl; }
		[[nodiscard]] const ActionDeclaration& getDeclaration() const
		{
			return decl;
		}

		void setResumePoint(size_t i) { remumePointIndex = i; }

		[[nodiscard]] size_t getResumePointIndex() const
		{
			return remumePointIndex;
		}

		private:
		ActionDeclaration decl;
		size_t remumePointIndex = 0;
	};

	class DeclarationStatement
	{
		public:
		friend llvm::yaml::MappingTraits<rlc::DeclarationStatement>;
		using iterator = std::array<Expression, 1>::iterator;
		using const_iterator = std::array<Expression, 1>::const_iterator;
		static constexpr const char* name = "declaration_statement";

		DeclarationStatement(
				std::string name,
				Expression exp,
				SourcePosition position = SourcePosition())
				: varName(std::move(name)),
					expression({ std::move(exp) }),
					position(std::move(position))
		{
		}

		[[nodiscard]] bool isPrivate() const { return getName().starts_with("_"); }

		[[nodiscard]] const Expression& getExpression() const
		{
			return expression[0];
		}
		[[nodiscard]] Expression& getExpression() { return expression[0]; }

		[[nodiscard]] const_iterator begin() const { return expression.begin(); }
		[[nodiscard]] iterator begin() { return expression.begin(); }
		[[nodiscard]] const_iterator end() const { return expression.end(); }
		[[nodiscard]] iterator end() { return expression.end(); }

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

		void print(
				llvm::raw_ostream& OS,
				size_t indents = 0,
				bool printPosition = false) const;
		void dump() const;
		llvm::Error deduceTypes(SymbolTable& tb, TypeDB& db);

		[[nodiscard]] const SourcePosition& getPosition() const { return position; }

		private:
		std::string varName;
		std::array<Expression, 1> expression;
		SourcePosition position;
	};

	class ReturnStatement
	{
		public:
		static constexpr const char* name = "return_statement";
		friend llvm::yaml::MappingTraits<rlc::ReturnStatement>;
		ReturnStatement(Expression exp, SourcePosition position)
				: expression({ std::move(exp) }), position(std::move(position))
		{
		}
		explicit ReturnStatement(SourcePosition position)
				: position(std::move(position))
		{
		}

		[[nodiscard]] rlc::Type* getReturnedType() const
		{
			if (expression.empty())
				return nullptr;
			return expression[0].getType();
		}

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

		void print(
				llvm::raw_ostream& OS,
				size_t indents = 0,
				bool printPosition = false) const;
		void dump() const;
		llvm::Error deduceTypes(const SymbolTable& tb, TypeDB& db);
		[[nodiscard]] const SourcePosition& getPosition() const { return position; }

		private:
		llvm::SmallVector<Expression, 1> expression;
		SourcePosition position;
	};

	class StatementList
	{
		public:
		static constexpr const char* name = "statement_list";
		friend llvm::yaml::MappingTraits<rlc::StatementList>;
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
		void print(
				llvm::raw_ostream& OS,
				size_t indents = 0,
				bool printPosition = false) const;
		void dump() const;

		StatementList(StatementList&& other) = default;
		StatementList& operator=(StatementList&& other) = default;
		StatementList(const StatementList& other);
		StatementList& operator=(const StatementList& other);
		~StatementList() = default;

		llvm::Error deduceTypes(SymbolTable& tb, TypeDB& db);

		using Container = llvm::SmallVector<std::unique_ptr<Statement>, 2>;
		[[nodiscard]] const SourcePosition& getPosition() const { return position; }

		private:
		explicit StatementList(Container c, SourcePosition position)
				: list(std::move(c)), position(std::move(position))
		{
		}
		Container list;
		SourcePosition position;
	};

	class IfStatement
	{
		public:
		static constexpr const char* name = "if_statement";
		friend llvm::yaml::MappingTraits<rlc::IfStatement>;
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
		void print(
				llvm::raw_ostream& OS,
				size_t indents = 0,
				bool printPosition = false) const;
		void dump() const;

		IfStatement(IfStatement&& other) = default;
		IfStatement& operator=(IfStatement&& other) = default;
		IfStatement(const IfStatement& other);
		IfStatement& operator=(const IfStatement& other);
		~IfStatement() = default;
		llvm::Error deduceTypes(SymbolTable& tb, TypeDB& db);
		[[nodiscard]] const SourcePosition& getPosition() const { return position; }

		private:
		IfStatement(
				Expression cond,
				std::unique_ptr<Statement> trueBranch,
				SourcePosition position)
				: condition({ std::move(cond) }), position(std::move(position))
		{
			branches.emplace_back(std::move(trueBranch));
		}
		IfStatement(
				Expression cond,
				std::unique_ptr<Statement> trueBranch,
				std::unique_ptr<Statement> falseBranch,
				SourcePosition position)
				: condition({ std::move(cond) }), position(std::move(position))
		{
			branches.emplace_back(std::move(trueBranch));
			branches.emplace_back(std::move(falseBranch));
		}
		std::array<Expression, 1> condition;
		llvm::SmallVector<std::unique_ptr<Statement>, 2> branches;
		SourcePosition position;
	};

	class WhileStatement
	{
		public:
		friend Statement;
		friend llvm::yaml::MappingTraits<rlc::WhileStatement>;
		static constexpr const char* name = "while_statement";
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
		void print(
				llvm::raw_ostream& OS,
				size_t indents = 0,
				bool printPosition = false) const;
		void dump() const;

		WhileStatement(WhileStatement&& other) = default;
		WhileStatement& operator=(WhileStatement&& other) = default;
		WhileStatement(const WhileStatement& other);
		WhileStatement& operator=(const WhileStatement& other);
		~WhileStatement() = default;
		llvm::Error deduceTypes(SymbolTable& tb, TypeDB& db);
		[[nodiscard]] const SourcePosition& getPosition() const { return position; }

		private:
		WhileStatement(
				Expression cond,
				std::unique_ptr<Statement> bod,
				SourcePosition position)
				: condition({ std::move(cond) }), position(std::move(position))
		{
			body[0] = std::move(bod);
		}
		std::array<Expression, 1> condition;
		std::array<std::unique_ptr<Statement>, 1> body;
		SourcePosition position;
	};

	class Statement
	{
		public:
		using ExpIterator = SimpleIterator<Statement&, Expression>;
		using ConstExpIterator = SimpleIterator<const Statement&, const Expression>;

		using StatIterator = SimpleIterator<Statement&, Statement>;
		using ConstStatIterator = SimpleIterator<const Statement&, const Statement>;

		template<typename Stat>
		explicit Statement(Stat&& cont)
			requires(not std::is_same_v<std::decay_t<Stat>, Statement>)
				: content(std::forward<Stat>(cont))
		{
		}

		Statement(Statement&& other) = default;
		Statement(const Statement& other) = default;
		Statement& operator=(const Statement& other) = default;
		Statement& operator=(Statement&& other) = default;
		~Statement() = default;

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
		[[nodiscard]] bool isActionStatement() const
		{
			return isA<ActionStatement>();
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
			return ConstStatIterator(*this, subStatmentCount());
		}
		[[nodiscard]] StatIterator end()
		{
			return StatIterator(*this, subStatmentCount());
		}
		llvm::Error deduceTypes(SymbolTable& tb, TypeDB& db);

		void print(
				llvm::raw_ostream& OS,
				size_t indents = 0,
				bool printPosition = false) const;
		void dump() const;

		static Statement actionStatement(ActionDeclaration declaration);

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
					std::move(cond),
					std::make_unique<Statement>(std::move(body)),
					std::move(pos));
			return Statement(std::move(s));
		}

		static Statement statmentList(
				llvm::SmallVector<Statement, 3> statement,
				SourcePosition pos = SourcePosition());

		static Statement returnStatement(
				Expression returnExpression, SourcePosition pos = SourcePosition())
		{
			return Statement(
					ReturnStatement(std::move(returnExpression), std::move(pos)));
		}

		static Statement returnStatement(SourcePosition pos = SourcePosition())
		{
			return Statement(ReturnStatement(std::move(pos)));
		}

		[[nodiscard]] const SourcePosition& getPosition() const
		{
			return visit([](const auto& Entry) -> const SourcePosition& {
				return Entry.getPosition();
			});
		}

		private:
		std::variant<
				ExpressionStatement,
				IfStatement,
				WhileStatement,
				StatementList,
				ReturnStatement,
				DeclarationStatement,
				ActionStatement>
				content;
	};

	template<>
	Statement& SimpleIterator<Statement&, Statement>::operator*() const;

	template<>
	const Statement&
	SimpleIterator<const Statement&, const Statement>::operator*() const;

	template<>
	Expression& SimpleIterator<Statement&, Expression>::operator*() const;

	template<>
	const Expression&
	SimpleIterator<const Statement&, const Expression>::operator*() const;
}	 // namespace rlc

template<>
struct llvm::yaml::SequenceTraits<rlc::StatementList::Container>
{
	static size_t size(IO& io, rlc::StatementList::Container& list)
	{
		return list.size();
	}
	static rlc::Statement& element(
			IO& io, rlc::StatementList::Container& list, size_t index)
	{
		assert(io.outputting());
		return *list[index];
	}
};

template<>
struct llvm::yaml::MappingTraits<rlc::StatementList>
{
	static void mapping(IO& io, rlc::StatementList& value)
	{
		assert(io.outputting());
		io.mapRequired("statements", value.list);
		if (not value.getPosition().isMissing())
			io.mapRequired("position", value.position);
	}
};

template<>
struct llvm::yaml::MappingTraits<rlc::IfStatement>
{
	static void mapping(IO& io, rlc::IfStatement& value)
	{
		assert(io.outputting());
		io.mapRequired("condition", value.condition.front());
		io.mapRequired("true_branch", *value.branches.front());
		if (value.hasFalseBranch())
			io.mapRequired("false_branch", *value.branches.back());
		if (not value.getPosition().isMissing())
			io.mapRequired("position", value.position);
	}
};

template<>
struct llvm::yaml::MappingTraits<rlc::WhileStatement>
{
	static void mapping(IO& io, rlc::WhileStatement& value)
	{
		assert(io.outputting());
		io.mapRequired("condition", value.condition.front());
		io.mapRequired("body", *value.body.front());
		if (not value.getPosition().isMissing())
			io.mapRequired("position", value.position);
	}
};

template<>
struct llvm::yaml::MappingTraits<rlc::ExpressionStatement>
{
	static void mapping(IO& io, rlc::ExpressionStatement& value)
	{
		assert(io.outputting());
		io.mapRequired("expression", value.expression.front());
		if (not value.getPosition().isMissing())
			io.mapRequired("position", value.position);
	}
};

template<>
struct llvm::yaml::MappingTraits<rlc::ReturnStatement>
{
	static void mapping(IO& io, rlc::ReturnStatement& value)
	{
		assert(io.outputting());
		if (not value.expression.empty())
			io.mapRequired("expression", value.expression.front());
		if (not value.getPosition().isMissing())
			io.mapRequired("position", value.position);
	}
};

template<>
struct llvm::yaml::MappingTraits<rlc::ActionStatement>
{
	static void mapping(IO& io, rlc::ActionStatement& value)
	{
		assert(io.outputting());
		io.mapRequired("resume_point", value.remumePointIndex);
		llvm::yaml::MappingTraits<rlc::ActionDeclaration>::mapping(io, value.decl);
	}
};

template<>
struct llvm::yaml::MappingTraits<rlc::DeclarationStatement>
{
	static void mapping(IO& io, rlc::DeclarationStatement& value)
	{
		assert(io.outputting());
		io.mapRequired("name", value.varName);
		io.mapRequired("expression", value.expression.front());
		if (not value.getPosition().isMissing())
			io.mapRequired("position", value.position);
	}
};

template<>
struct llvm::yaml::MappingTraits<rlc::Statement>
{
	static void mapping(IO& io, rlc::Statement& value)
	{
		assert(io.outputting());
		value.visit([&]<typename T>(T& inner) -> void {
			io.mapTag(T::name, true);
			llvm::yaml::MappingTraits<T>::mapping(io, inner);
		});
	}
};
