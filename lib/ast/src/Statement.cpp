#include "rlc/ast/Statement.hpp"

#include <cstdlib>
#include <memory>

#include "llvm/ADT/STLExtras.h"
#include "rlc/ast/Expression.hpp"
#include "rlc/ast/SymbolTable.hpp"
#include "rlc/utils/Error.hpp"
#include "rlc/utils/SourcePosition.hpp"

using namespace rlc;
using namespace llvm;
using namespace std;

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

StatementList::StatementList(const StatementList& other)
		: position(other.position)
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
	position = other.position;
	return *this;
}

IfStatement::IfStatement(const IfStatement& other)
		: condition(other.condition), position(other.position)
{
	for (const auto& s : other.branches)
		branches.emplace_back(std::make_unique<Statement>(*s));
}

IfStatement& IfStatement::operator=(const IfStatement& other)
{
	if (this == &other)
		return *this;
	condition = other.condition;
	position = other.position;

	branches.clear();
	for (const auto& s : other.branches)
		branches.emplace_back(std::make_unique<Statement>(*s));

	return *this;
}

WhileStatement::WhileStatement(const WhileStatement& other)
		: condition(other.condition), position(other.position)
{
	body[0] = std::make_unique<Statement>(other.getBody());
}

WhileStatement& WhileStatement::operator=(const WhileStatement& other)
{
	if (this == &other)
		return *this;
	condition = other.condition;
	body[0] = std::make_unique<Statement>(other.getBody());
	position = other.position;

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
			std::move(exp),
			std::make_unique<Statement>(std::move(trueB)),
			std::make_unique<Statement>(std::move(falseB)),
			std::move(pos));
	return Statement(std::move(stat));
}

Statement Statement::statmentList(
		SmallVector<Statement, 3> statements, SourcePosition pos)
{
	StatementList::Container container;
	for (auto& s : statements)
		container.emplace_back(std::make_unique<Statement>(std::move(s)));

	StatementList ls(std::move(container), std::move(pos));
	return Statement(std::move(ls));
}

Statement Statement::ifStatment(
		Expression exp, Statement trueB, SourcePosition pos)
{
	IfStatement stat(
			std::move(exp),
			std::make_unique<Statement>(std::move(trueB)),
			std::move(pos));
	return Statement(std::move(stat));
}

Statement Statement::expStatement(Expression exp, SourcePosition pos)
{
	return Statement(ExpressionStatement(std::move(exp), std::move(pos)));
}

Statement Statement::declarationStatement(
		std::string name, Expression exp, SourcePosition pos)
{
	return Statement(
			DeclarationStatement(std::move(name), std::move(exp), std::move(pos)));
}

template<typename T>
size_t subExpCountImp(const T& stat)
{
	return stat.subExpCount();
}

template<>
size_t subExpCountImp<ActionStatement>(const ActionStatement&)
{
	return 0;
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

template<>
size_t subStatCountImp<ActionStatement>(const ActionStatement&)
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
auto& getSubStatImp<ActionStatement>(ActionStatement& stat, size_t index)
{
	assert(false && "unrechable");
	abort();
	return *static_cast<Statement*>(nullptr);
}

template<>
auto& getSubStatImp<const ActionStatement>(
		const ActionStatement& stat, size_t index)
{
	assert(false && "unrechable");
	abort();
	return *static_cast<Statement*>(nullptr);
}

template<>
auto& getSubStatImp<ExpressionStatement>(
		ExpressionStatement& stat, size_t index)
{
	assert(false && "unrechable");
	abort();
	return *static_cast<Statement*>(nullptr);
}

template<>
auto& getSubStatImp<const ExpressionStatement>(
		const ExpressionStatement& stat, size_t index)
{
	assert(false && "unrechable");
	abort();
	return *static_cast<Statement*>(nullptr);
}

template<>
auto& getSubStatImp<ReturnStatement>(ReturnStatement& stat, size_t index)
{
	assert(false && "unrechable");
	abort();
	return *static_cast<Statement*>(nullptr);
}

template<>
auto& getSubStatImp<const ReturnStatement>(
		const ReturnStatement& stat, size_t index)
{
	assert(false && "unrechable");
	abort();
	return *static_cast<Statement*>(nullptr);
}

template<>
auto& getSubStatImp<DeclarationStatement>(
		DeclarationStatement& stat, size_t index)
{
	assert(false && "unrechable");
	abort();
	return *static_cast<Statement*>(nullptr);
}

template<>
auto& getSubStatImp<const DeclarationStatement>(
		const DeclarationStatement& stat, size_t index)
{
	assert(false && "unrechable");
	abort();
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
auto& getSubExpImp<ActionStatement>(ActionStatement& stat, size_t index)
{
	assert(false && "unrechable");
	abort();
	return *static_cast<Expression*>(nullptr);
}

template<>
auto& getSubExpImp<const ActionStatement>(
		const ActionStatement& stat, size_t index)
{
	assert(false && "unrechable");
	abort();
	return *static_cast<Expression*>(nullptr);
}

template<>
auto& getSubExpImp<StatementList>(StatementList& stat, size_t index)
{
	assert(false && "unrechable");
	abort();
	return *static_cast<Expression*>(nullptr);
}

template<>
auto& getSubExpImp<const StatementList>(const StatementList& stat, size_t index)
{
	assert(false && "unrechable");
	abort();
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

void ActionStatement::print(
		raw_ostream& OS, size_t indents, bool printPosition) const
{
	llvm::yaml::Output output(OS);
	output << *const_cast<ActionStatement*>(this);
}

void ActionStatement::dump() const { print(outs()); }

void Statement::print(raw_ostream& OS, size_t indents, bool printPosition) const
{
	llvm::yaml::Output output(OS);
	output << *const_cast<Statement*>(this);
}

void Statement::dump() const { print(outs()); }

void ExpressionStatement::print(
		raw_ostream& OS, size_t indents, bool printPosition) const
{
	getExpression().print(OS, indents, printPosition);
}

void ExpressionStatement::dump() const { print(outs()); }

void IfStatement::print(
		raw_ostream& OS, size_t indents, bool printPosition) const
{
	OS.indent(indents);
	OS << "if statement\n";
	getCondition().print(OS, indents + 1, printPosition);

	OS << "\n";
	OS.indent(indents);
	OS << "branches:";
	for (const auto& b : *this)
	{
		b.print(OS, indents + 1, printPosition);
		OS << "\n";
	}
}

void IfStatement::dump() const { print(outs()); }

void WhileStatement::print(
		raw_ostream& OS, size_t indents, bool printPosition) const
{
	OS.indent(indents);
	OS << "while statement\n";
	OS.indent(indents + 1);
	OS << "cond:\n";
	getCondition().print(OS, indents + 2, printPosition);

	OS << "\n";
	OS.indent(indents + 1);
	OS << "body:\n";
	for (const auto& b : *this)
	{
		b.print(OS, indents + 2, printPosition);
		OS << "\n";
	}
}

void WhileStatement::dump() const { print(outs()); }

void StatementList::print(
		raw_ostream& OS, size_t indents, bool printPosition) const
{
	OS.indent(indents);
	OS << "statement list\n";

	for (const auto& b : *this)
	{
		b.print(OS, indents + 1, printPosition);
		OS << "\n";
	}
}

void StatementList::dump() const { print(outs()); }

void DeclarationStatement::print(
		raw_ostream& OS, size_t indents, bool printPosition) const
{
	OS.indent(indents);
	OS << "declaration statement: " << getName() << "\n";

	getExpression().print(OS, indents + 1, printPosition);
}

void DeclarationStatement::dump() const { print(outs()); }

void ReturnStatement::print(
		raw_ostream& OS, size_t indents, bool printPosition) const
{
	OS.indent(indents);
	OS << "return statement \n";

	if (!isVoid())
	{
		getExpression().print(OS, indents + 1, printPosition);
		OS << "\n";
	}
}

void ReturnStatement::dump() const { print(outs()); }

Error ExpressionStatement::deduceTypes(const SymbolTable& tb, TypeDB& db)
{
	return getExpression().deduceType(tb, db);
}

Error ReturnStatement::deduceTypes(const SymbolTable& tb, TypeDB& db)
{
	if (isVoid())
		return Error::success();
	return getExpression().deduceType(tb, db);
}

Error StatementList::deduceTypes(SymbolTable& tb, TypeDB& db)
{
	SymbolTable t(&tb);
	for (auto& s : *this)
		if (auto e = s.deduceTypes(t, db); e)
			return e;

	return Error::success();
}

Error IfStatement::deduceTypes(SymbolTable& tb, TypeDB& db)
{
	if (auto error = getCondition().deduceType(tb, db); error)
		return error;

	for (auto& s : *this)
		if (auto e = s.deduceTypes(tb, db); e)
			return e;

	if (!getCondition().getType()->isBool())
		return make_error<RlcError>(
				"condition of if statement was not boolean",
				RlcErrorCategory::errorCode(RlcErrorCode::argumentTypeMissmatch),
				getPosition());

	return Error::success();
}

Error WhileStatement::deduceTypes(SymbolTable& tb, TypeDB& db)
{
	for (auto& s : *this)
		if (auto e = s.deduceTypes(tb, db); e)
			return e;

	for (auto& exp : expRange())
		if (auto e = exp.deduceType(tb, db); e)
			return e;

	if (!getCondition().getType()->isBool())
		return make_error<RlcError>(
				"condition of if statement must be a boolean expression",
				RlcErrorCategory::errorCode(RlcErrorCode::argumentTypeMissmatch),
				getPosition());

	return Error::success();
}

Error ActionStatement::deduceTypes(SymbolTable& tb, TypeDB& db)
{
	if (auto error = decl.deduceType(tb, db); error)
		return error;

	for (auto& arg : decl)
	{
		if (tb.directContain(arg.getName()))
			return make_error<RlcError>(
					"Symbol " + arg.getName() + "was already defined",
					RlcErrorCategory::errorCode(RlcErrorCode::alreadyDefininedVariable),
					arg.getSourcePosition());

		if (auto e = arg.deduceType(tb, db); e)
			return e;

		tb.insert(arg);
	}
	return llvm::Error::success();
}

Statement Statement::actionStatement(ActionDeclaration declaration)
{
	return Statement(ActionStatement(declaration));
}

Error DeclarationStatement::deduceTypes(SymbolTable& tb, TypeDB& db)
{
	if (!tb.directContain(getName()))
	{
		if (auto e = getExpression().deduceType(tb, db); e)
			return e;

		tb.insert(*this);
		return Error::success();
	}

	return make_error<RlcError>(
			"Symbol " + getName() + "was already defined",
			RlcErrorCategory::errorCode(RlcErrorCode::alreadyDefininedVariable),
			getPosition());
}

llvm::Error Statement::deduceTypes(SymbolTable& tb, TypeDB& db)
{
	return std::visit([&](auto& s) { return s.deduceTypes(tb, db); }, content);
}
