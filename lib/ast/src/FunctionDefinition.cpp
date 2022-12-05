#include "rlc/ast/FunctionDefinition.hpp"

#include "llvm/ADT/DepthFirstIterator.h"
#include "llvm/Support/raw_ostream.h"
#include "rlc/ast/SymbolTable.hpp"
#include "rlc/utils/Error.hpp"

using namespace llvm;
using namespace std;
using namespace rlc;

void FunctionDefinition::print(
		raw_ostream& OS, size_t indents, bool dumpPosition) const
{
	llvm::yaml::Output output(OS);
	output << *const_cast<FunctionDefinition*>(this);
}
void FunctionDefinition::dump() const { print(outs()); }

llvm::Error FunctionDefinition::checkReturnedExpressionTypesAreCorrect() const
{
	for (const auto& node : llvm::depth_first(*this))
	{
		const auto* statement = node.statement;
		if (not statement->isReturnStatement())
			continue;

		auto* type = statement->get<ReturnStatement>().getReturnedType();
		if (type == nullptr and getDeclaration().getReturnType()->isVoid())
			continue;

		if (type == nullptr and not getDeclaration().getReturnType()->isVoid())
		{
			return llvm::make_error<rlc::RlcError>(
					"return statement of non-void function needs a expression",
					RlcErrorCategory::errorCode(RlcErrorCode::argumentTypeMissmatch),
					statement->getPosition());
		}

		if (type != getDeclaration().getReturnType())
		{
			return llvm::make_error<rlc::RlcError>(
					"return statement type missmatch",
					RlcErrorCategory::errorCode(RlcErrorCode::argumentTypeMissmatch),
					statement->getPosition());
		}
	}

	return llvm::Error::success();
}

Error FunctionDefinition::deduceTypes(const SymbolTable& tb, TypeDB& db)
{
	SymbolTable t(&tb);
	for (const auto& arg : *this)
		t.insert(arg);
	if (auto error = getBody().deduceTypes(t, db); error)
		return error;

	return checkReturnedExpressionTypesAreCorrect();
}

bool FunctionDefinition::isAction() const
{
	return llvm::any_of(llvm::depth_first(*this), [](const auto& def) {
		return def.statement->isActionStatement();
	});
}
