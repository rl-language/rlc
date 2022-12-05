#include "rlc/ast/ActionDefinition.hpp"

#include "llvm/ADT/DepthFirstIterator.h"
#include "llvm/Support/raw_ostream.h"
#include "rlc/ast/FunctionDefinition.hpp"
#include "rlc/ast/SymbolTable.hpp"
#include "rlc/utils/Error.hpp"

using namespace llvm;
using namespace std;
using namespace rlc;

static bool expressionMustBeReplaced(Expression& expression)
{
	if (not expression.isA<Reference>())
		return false;

	auto& reference = expression.get<Reference>();
	if (not reference.hasReference())
		return false;

	auto symbol = reference.getReferred();
	if (not symbol.isA<ArgumentDeclaration>() and
			not symbol.isA<DeclarationStatement>())
		return false;

	if (symbol.isA<DeclarationStatement>() and
			symbol.get<DeclarationStatement>().isPrivate())
		return false;
	return true;
}

[[nodiscard]] FunctionDefinition ActionDefinition::asFunction(
		SymbolTable& table) const
{
	auto def = FunctionDefinition(
			implementatioName(),
			getBody(),
			SingleTypeUse::scalarType(builtinTypeToString(BuiltinType::VOID).str()),
			{ ArgumentDeclaration(
					"arg0",
					SingleTypeUse::scalarType(declaration.getEntityName()),
					getSourcePosition()) },
			getSourcePosition());

	llvm::SmallVector<Expression*, 2> toBeReplaced;
	llvm::SmallVector<Statement*, 2> varDecls;
	for (auto& statement : llvm::depth_first(FunctionStatementGraph{ &def }))
	{
		for (Expression& e : statement.statement->expRange())
		{
			for (auto exp : llvm::depth_first(ExpressionGraph{ &e }))
			{
				Expression& expression = *exp.exp;
				if (not expressionMustBeReplaced(expression))
					continue;

				toBeReplaced.push_back(&expression);
			}
		}

		if (not statement.statement->isDeclarationStatement())
			continue;

		auto& decl = statement.statement->get<DeclarationStatement>();
		if (decl.isPrivate())
			continue;

		varDecls.push_back(statement.statement);
	}

	for (Expression* exp : toBeReplaced)
	{
		auto& reference = exp->get<Reference>();
		*exp = Expression::memberAccess(
				Expression::reference("arg0", exp->getPosition()),
				reference.getName(),
				exp->getPosition());
	}

	for (Statement* stat : varDecls)
	{
		auto& declaration = stat->get<DeclarationStatement>();
		auto lhs = Expression::memberAccess(
				Expression::reference("arg0", stat->getPosition()),
				declaration.getName(),
				stat->getPosition());
		auto expression = Expression::assign(
				std::move(lhs),
				std::move(declaration.getExpression()),
				declaration.getPosition());
		*stat = Statement::expStatement(std::move(expression));
	}

	return def;
}

void ActionDefinition::print(
		raw_ostream& OS, size_t indents, bool dumpPosition) const
{
	llvm::yaml::Output output(OS);
	output << *const_cast<ActionDefinition*>(this);
}
void ActionDefinition::dump() const { print(outs()); }

llvm::Error ActionDefinition::checkReturnedExpressionTypesAreCorrect() const
{
	for (const auto& node : llvm::depth_first(*this))
	{
		const auto* statement = node.statement;
		if (not statement->isReturnStatement())
			continue;

		auto* type = statement->get<ReturnStatement>().getReturnedType();
		if (type == nullptr)
			continue;

		return llvm::make_error<rlc::RlcError>(
				"action cannot return anything",
				RlcErrorCategory::errorCode(RlcErrorCode::argumentTypeMissmatch),
				statement->getPosition());
	}

	return llvm::Error::success();
}

Error ActionDefinition::deduceTypes(const SymbolTable& tb, TypeDB& db)
{
	SymbolTable t(&tb);
	for (const auto& arg : *this)
		t.insert(arg);

	size_t index = 1;
	for (const auto& statement : llvm::depth_first(ActionDefinitionGraph{ this }))
	{
		if (not statement.statement->isActionStatement())
			continue;

		statement.statement->get<ActionStatement>().setResumePoint(index);
		index++;
	}

	if (auto error = getBody().deduceTypes(t, db); error)
		return error;

	return checkReturnedExpressionTypesAreCorrect();
}

[[nodiscard]] llvm::SmallVector<const ActionDeclaration*, 4>
ActionDefinition::allInnerActions() const
{
	llvm::SmallVector<const ActionDeclaration*, 4> toReturn;
	for (const auto& statement : llvm::depth_first(*this))
	{
		if (not statement.statement->isActionStatement())
			continue;

		const auto& decl = statement.statement->get<ActionStatement>();
		toReturn.emplace_back(&decl.getDeclaration());
	}

	return toReturn;
}
