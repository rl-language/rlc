#include "rlc/ast/Statement.hpp"

#include <memory>

#include "llvm/ADT/STLExtras.h"
#include "rlc/ast/Expression.hpp"
#include "rlc/utils/SourcePosition.hpp"

using namespace rlc;
using namespace llvm;
using namespace std;

StatementList::StatementList(const StatementList& other)
{
	for (const auto& s : other.list)
		list.emplace_back(std::make_unique<Statement>(*s));
}

StatementList& StatementList::operator=(const StatementList& other)
{
	if (this == &other)
		return *this;

	list.clear();
	for (const auto& s : other.list)
		list.emplace_back(std::make_unique<Statement>(*s));
	return *this;
}

IfStatement::IfStatement(const IfStatement& other): condition(other.condition)
{
	for (const auto& s : other.branches)
		branches.emplace_back(std::make_unique<Statement>(*s));
}

IfStatement& IfStatement::operator=(const IfStatement& other)
{
	if (this == &other)
		return *this;
	condition = other.condition;

	branches.clear();
	for (const auto& s : other.branches)
		branches.emplace_back(std::make_unique<Statement>(*s));

	return *this;
}

WhileStatement::WhileStatement(const WhileStatement& other)
		: condition(other.condition)
{
	body[0] = std::make_unique<Statement>(other.getBody());
}

WhileStatement& WhileStatement::operator=(const WhileStatement& other)
{
	if (this == &other)
		return *this;
	condition = other.condition;
	body[0] = std::make_unique<Statement>(other.getBody());

	return *this;
}

template<>
const Statement&
		SimpleIterator<const StatementList&, const Statement>::operator*() const
{
	return type.getSubStatement(index);
}

template<>
Statement& SimpleIterator<StatementList&, Statement>::operator*() const
{
	return type.getSubStatement(index);
}

template<>
const Statement&
		SimpleIterator<const IfStatement&, const Statement>::operator*() const
{
	return type.getSubStatement(index);
}

template<>
Statement& SimpleIterator<IfStatement&, Statement>::operator*() const
{
	return type.getSubStatement(index);
}

template<>
const Statement&
		SimpleIterator<const WhileStatement&, const Statement>::operator*() const
{
	return type.getSubStatement(index);
}

template<>
Statement& SimpleIterator<WhileStatement&, Statement>::operator*() const
{
	return type.getSubStatement(index);
}

Statement Statement::ifStatment(
		Expression exp, Statement trueB, Statement falseB, SourcePosition pos)
{
	IfStatement stat(
			move(exp),
			std::make_unique<Statement>(move(trueB)),
			std::make_unique<Statement>(move(falseB)));
	return Statement(move(stat), move(pos));
}

Statement Statement::statmentList(
		SmallVector<Statement, 3> statements, SourcePosition pos)
{
	StatementList::Container container;
	for (auto& s : statements)
		container.emplace_back(std::make_unique<Statement>(move(s)));

	StatementList ls(move(container));
	return Statement(move(ls), move(pos));
}

Statement Statement::ifStatment(
		Expression exp, Statement trueB, SourcePosition pos)
{
	IfStatement stat(move(exp), std::make_unique<Statement>(move(trueB)));
	return Statement(move(stat), move(pos));
}

Statement Statement::expStatement(Expression exp, SourcePosition pos)
{
	return Statement(ExpressionStatement(move(exp)), move(pos));
}

Statement Statement::declarationStatement(
		std::string name, Expression exp, SourcePosition pos)
{
	return Statement(DeclarationStatement(move(name), move(exp)), move(pos));
}

template<typename T>
size_t subExpCountImp(const T& stat)
{
	return stat.subExpCount();
}

template<>
size_t subExpCountImp<StatementList>(const StatementList&)
{
	return 0;
}

template<typename T>
size_t subStatCountImp(const T& stat)
{
	return stat.subStatCount();
}

template<>
size_t subStatCountImp<ExpressionStatement>(const ExpressionStatement&)
{
	return 0;
}

template<>
size_t subStatCountImp<ReturnStatement>(const ReturnStatement&)
{
	return 0;
}

template<>
size_t subStatCountImp<DeclarationStatement>(const DeclarationStatement&)
{
	return 0;
}

size_t Statement::subExpCount() const
{
	return visit([](const auto& stat) { return subExpCountImp(stat); });
}

size_t Statement::subStatmentCount() const
{
	return visit([](const auto& stat) { return subStatCountImp(stat); });
}

template<typename T>
auto& getSubStatImp(T& stat, size_t index)
{
	return stat.getSubStatement(index);
}

template<>
auto& getSubStatImp<ExpressionStatement>(
		ExpressionStatement& stat, size_t index)
{
	assert(false && "unrechable");
	return *static_cast<Statement*>(nullptr);
}

template<>
auto& getSubStatImp<const ExpressionStatement>(
		const ExpressionStatement& stat, size_t index)
{
	assert(false && "unrechable");
	return *static_cast<Statement*>(nullptr);
}

template<>
auto& getSubStatImp<ReturnStatement>(ReturnStatement& stat, size_t index)
{
	assert(false && "unrechable");
	return *static_cast<Statement*>(nullptr);
}

template<>
auto& getSubStatImp<const ReturnStatement>(
		const ReturnStatement& stat, size_t index)
{
	assert(false && "unrechable");
	return *static_cast<Statement*>(nullptr);
}

template<>
auto& getSubStatImp<DeclarationStatement>(
		DeclarationStatement& stat, size_t index)
{
	assert(false && "unrechable");
	return *static_cast<Statement*>(nullptr);
}

template<>
auto& getSubStatImp<const DeclarationStatement>(
		const DeclarationStatement& stat, size_t index)
{
	assert(false && "unrechable");
	return *static_cast<Statement*>(nullptr);
}

Statement& Statement::getSubStatement(size_t index)
{
	return visit(
			[index](auto& stat) -> Statement& { return getSubStatImp(stat, index); });
}

template<typename T>
auto& getSubExpImp(T& stat, size_t index)
{
	return stat.getSubExp(index);
}

template<>
auto& getSubExpImp<StatementList>(StatementList& stat, size_t index)
{
	assert(false && "unrechable");
	return *static_cast<Expression*>(nullptr);
}

template<>
auto& getSubExpImp<const StatementList>(const StatementList& stat, size_t index)
{
	assert(false && "unrechable");
	return *static_cast<Expression*>(nullptr);
}

Expression& Statement::getSubExp(size_t index)
{
	return visit(
			[index](auto& stat) -> Expression& { return getSubExpImp(stat, index); });
}

const Statement& Statement::getSubStatement(size_t index) const
{
	return visit([index](const auto& stat) -> const Statement& {
		return getSubStatImp(stat, index);
	});
}

const Expression& Statement::getSubExp(size_t index) const
{
	return visit([index](const auto& stat) -> const Expression& {
		return getSubExpImp(stat, index);
	});
}

void Statement::print(raw_ostream& OS, size_t indents, bool printPosition) const
{
	if (printPosition)
	{
		OS.indent(indents);
		OS << position.toString();
		OS << "\n";
	}

	visit([&](const auto& cont) { cont.print(OS, indents); });
}

void Statement::dump() const { print(outs()); }

void ExpressionStatement::print(raw_ostream& OS, size_t indents) const
{
	getExpression().print(OS, indents);
}

void ExpressionStatement::dump() const { print(outs()); }

void IfStatement::print(raw_ostream& OS, size_t indents) const
{
	OS.indent(indents);
	OS << "if statement\n";
	getCondition().print(OS, indents + 1);

	OS << "\n";
	OS.indent(indents);
	OS << "branches:";
	for (const auto& b : *this)
	{
		b.print(OS, indents + 1);
		OS << "\n";
	}
}

void IfStatement::dump() const { print(outs()); }

void WhileStatement::print(raw_ostream& OS, size_t indents) const
{
	OS.indent(indents);
	OS << "while statement\n";
	OS.indent(indents + 1);
	OS << "cond:\n";
	getCondition().print(OS, indents + 2);

	OS << "\n";
	OS.indent(indents + 1);
	OS << "body:\n";
	for (const auto& b : *this)
	{
		b.print(OS, indents + 2);
		OS << "\n";
	}
}

void WhileStatement::dump() const { print(outs()); }

void StatementList::print(raw_ostream& OS, size_t indents) const
{
	OS.indent(indents);
	OS << "statement list\n";

	for (const auto& b : *this)
	{
		b.print(OS, indents + 1);
		if (&b != &(*(end() - 1)))
			OS << "\n";
	}
}

void StatementList::dump() const { print(outs()); }

void DeclarationStatement::print(raw_ostream& OS, size_t indents) const
{
	OS.indent(indents);
	OS << "declaration statement: " << getName() << " = ";

	getExpression().print(OS);
}

void DeclarationStatement::dump() const { print(outs()); }

void ReturnStatement::print(raw_ostream& OS, size_t indents) const
{
	OS.indent(indents);
	OS << "return statement \n";

	if (!isVoid())
	{
		getExpression().print(OS, indents + 1);
		OS << "\n";
	}
}

void ReturnStatement::dump() const { print(outs()); }

template<>
Statement& SimpleIterator<Statement&, Statement>::operator*() const
{
	return type.getSubStatement(index);
}

template<>
const Statement& SimpleIterator<const Statement&, const Statement>::operator*()
		const
{
	return type.getSubStatement(index);
}

template<>
Expression& SimpleIterator<Statement&, Expression>::operator*() const
{
	return type.getSubExp(index);
}

template<>
const Expression&
		SimpleIterator<const Statement&, const Expression>::operator*() const
{
	return type.getSubExp(index);
}
