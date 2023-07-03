#include "rlc/dialect/OverloadResolver.hpp"

#include "rlc/dialect/Dialect.h"
#include "rlc/dialect/Operations.hpp"

mlir::Type mlir::rlc::OverloadResolver::deduceTemplateCallSiteType(
		mlir::TypeRange callSiteArgumentTypes, mlir::FunctionType possibleCallee)
{
	llvm::DenseMap<mlir::rlc::TemplateParameterType, mlir::Type> substitutions;
	for (auto [callSiteArgument, calleeArgument] :
			 llvm::zip(callSiteArgumentTypes, possibleCallee.getInputs()))
	{
		if (not calleeArgument.isa<mlir::rlc::TemplateParameterType>())
		{
			if (callSiteArgument != calleeArgument)
				return nullptr;
			continue;
		}

		auto templateParameter =
				calleeArgument.cast<mlir::rlc::TemplateParameterType>();
		if (templateParameter.getTrait() != nullptr and
				templateParameter.getTrait()
						.typeRespectsTrait(callSiteArgument, *symbolTable)
						.failed())
		{
			return nullptr;
		}

		if (auto iter = substitutions.find(templateParameter);
				iter != substitutions.end())
			if (iter->second != callSiteArgument)
				return nullptr;

		substitutions[templateParameter] = callSiteArgument;
	}

	if (possibleCallee.getNumResults() == 0)
		return mlir::FunctionType::get(
				possibleCallee.getContext(), callSiteArgumentTypes, mlir::TypeRange());

	auto resultType = possibleCallee.getResult(0);
	if (auto casted = resultType.dyn_cast<mlir::rlc::TemplateParameterType>())
		resultType = substitutions[casted];

	return mlir::FunctionType::get(
			possibleCallee.getContext(),
			callSiteArgumentTypes,
			mlir::TypeRange(resultType));
}

mlir::Value mlir::rlc::OverloadResolver::findOverload(
		llvm::StringRef name, mlir::TypeRange arguments)
{
	llvm::SmallVector<mlir::Value> matching = findOverloads(name, arguments);

	assert(matching.size() <= 1);
	if (not matching.empty())
		return matching.front();

	std::string error;
	llvm::raw_string_ostream stream(error);
	stream << "could not find matching function " << name << "(";
	for (auto argument : arguments)
	{
		argument.print(stream);
		stream << ",";
	}
	stream << ")";
	stream.flush();

	if (errorEmitter)
		errorEmitter->emitError(error);

	for (auto candidate : symbolTable->get(name))
	{
		if (not candidate.getType().isa<mlir::FunctionType>())
			continue;
		candidate.getDefiningOp()->emitRemark("candidate");
	}
	return nullptr;
}

llvm::SmallVector<mlir::Value, 2> mlir::rlc::OverloadResolver::findOverloads(
		llvm::StringRef name, mlir::TypeRange arguments)
{
	llvm::SmallVector<mlir::Value> matching;
	for (auto candidate : symbolTable->get(name))
	{
		if (not candidate.getType().isa<mlir::FunctionType>())
			continue;

		if (deduceTemplateCallSiteType(
						arguments, candidate.getType().cast<mlir::FunctionType>()) !=
				nullptr)
			matching.push_back(candidate);
	}
	return matching;
}

mlir::Value mlir::rlc::OverloadResolver::instantiateOverload(
		mlir::IRRewriter& rewriter,
		mlir::Location loc,
		llvm::StringRef name,
		mlir::TypeRange arguments)
{
	auto overload = findOverload(name, arguments);
	if (not overload)
		return overload;

	auto instantiated = deduceTemplateCallSiteType(
			arguments, overload.getType().cast<mlir::FunctionType>());
	if (isTemplateType(overload.getType()).failed())
		return overload;

	return rewriter.create<mlir::rlc::TemplateInstantiationOp>(
			loc, instantiated, overload);
}
