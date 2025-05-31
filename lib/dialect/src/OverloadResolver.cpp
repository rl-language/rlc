/*
Copyright 2024 Massimo Fioravanti

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	 http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/
#include "rlc/dialect/OverloadResolver.hpp"

#include "rlc/dialect/Dialect.h"
#include "rlc/dialect/Operations.hpp"

mlir::LogicalResult mlir::rlc::OverloadResolver::deduceSubstitutions(
		mlir::Location callPoint,
		llvm::DenseMap<mlir::rlc::TemplateParameterType, mlir::Type>& substitutions,
		mlir::Type calleeArgument,
		mlir::Type callSiteArgument)
{
	if (auto casted = calleeArgument.dyn_cast<mlir::rlc::FrameType>())
		return deduceSubstitutions(
				callPoint, substitutions, casted.getUnderlying(), callSiteArgument);
	if (auto casted = calleeArgument.dyn_cast<mlir::rlc::ContextType>())
		return deduceSubstitutions(
				callPoint, substitutions, casted.getUnderlying(), callSiteArgument);
	// if the called function argument is not a template, you must provide exactly
	// that argumen of course.
	if (mlir::rlc::isTemplateType(calleeArgument, isTemplate).failed())
		return mlir::success(callSiteArgument == calleeArgument);

	if (auto templateParameter =
					calleeArgument.dyn_cast<mlir::rlc::TemplateParameterType>())
	{
		if (templateParameter.getTrait() != nullptr and
				templateParameter.getTrait()
						.typeRespectsTrait(callPoint, callSiteArgument, *symbolTable)
						.failed())
			return mlir::failure();

		if (auto iter = substitutions.find(templateParameter);
				iter != substitutions.end())
			if (iter->second != callSiteArgument)
				return mlir::failure();

		substitutions[templateParameter] = callSiteArgument;
		return mlir::success();
	}

	if (auto pair =
					std::pair{ calleeArgument.dyn_cast<mlir::rlc::ClassType>(),
										 callSiteArgument.dyn_cast<mlir::rlc::ClassType>() };
			pair.first and pair.second)
	{
		if (pair.first.getName() != pair.second.getName())
			return mlir::failure();

		for (auto [callee, callsite] : llvm::zip(
						 pair.first.getExplicitTemplateParameters(),
						 pair.second.getExplicitTemplateParameters()))
		{
			if (deduceSubstitutions(callPoint, substitutions, callee, callsite)
							.failed())
				return mlir::failure();
		}

		for (auto [callee, callsite] :
				 llvm::zip(pair.first.getMembers(), pair.second.getMembers()))
		{
			if (deduceSubstitutions(
							callPoint, substitutions, callee.getType(), callsite.getType())
							.failed())
				return mlir::failure();
		}
		return mlir::success();
	}

	if (auto pair =
					std::pair{ calleeArgument.dyn_cast<mlir::rlc::ArrayType>(),
										 callSiteArgument.dyn_cast<mlir::rlc::ArrayType>() };
			pair.first and pair.second)
	{
		if (pair.first.getSize() != pair.second.getSize())
		{
			if (auto templateParemterCallee =
							pair.first.getSize().dyn_cast<mlir::rlc::TemplateParameterType>())
			{
				assert(templateParemterCallee.getIsIntLiteral());
				substitutions[templateParemterCallee] = pair.second.getSize();
			}
			else
				return mlir::failure();
		}
		return deduceSubstitutions(
				callPoint,
				substitutions,
				pair.first.getUnderlying(),
				pair.second.getUnderlying());
	}

	if (auto pair =
					std::pair{ calleeArgument.dyn_cast<mlir::rlc::OwningPtrType>(),
										 callSiteArgument.dyn_cast<mlir::rlc::OwningPtrType>() };
			pair.first and pair.second)
	{
		return deduceSubstitutions(
				callPoint,
				substitutions,
				pair.first.getUnderlying(),
				pair.second.getUnderlying());
	}

	if (auto pair = std::pair{ calleeArgument.dyn_cast<mlir::FunctionType>(),
														 callSiteArgument.dyn_cast<mlir::FunctionType>() };
			pair.first and pair.second)
	{
		for (auto [callee, callsite] :
				 llvm::zip(pair.first.getInputs(), pair.second.getInputs()))
		{
			if (deduceSubstitutions(callPoint, substitutions, callee, callsite)
							.failed())
				return mlir::failure();
		}
		for (auto [callee, callsite] :
				 llvm::zip(pair.first.getResults(), pair.second.getResults()))
		{
			if (deduceSubstitutions(callPoint, substitutions, callee, callsite)
							.failed())
				return mlir::failure();
		}
		return mlir::success();
	}

	return mlir::failure();
}

mlir::Type mlir::rlc::OverloadResolver::deduceTemplateCallSiteType(
		mlir::Location callPoint,
		mlir::TypeRange callSiteArgumentTypes,
		mlir::FunctionType possibleCallee,
		mlir::TypeRange caleeTemplateArguments)
{
	llvm::DenseMap<mlir::rlc::TemplateParameterType, mlir::Type> substitutions;
	for (auto [callSiteArgument, calleeArgument] :
			 llvm::zip(callSiteArgumentTypes, possibleCallee.getInputs()))
	{
		if (deduceSubstitutions(
						callPoint, substitutions, calleeArgument, callSiteArgument)
						.failed())
			return nullptr;
	}

	for (auto templateParameter : caleeTemplateArguments)
	{
		auto templateArg =
				mlir::dyn_cast<mlir::rlc::TemplateParameterType>(templateParameter);
		if (not templateArg)
			continue;
		if (not substitutions.contains(templateArg))
		{
			return nullptr;
		}
	}

	if (possibleCallee.getNumResults() == 0)
		return mlir::FunctionType::get(
				possibleCallee.getContext(), callSiteArgumentTypes, mlir::TypeRange());

	auto resultType = possibleCallee.getResult(0);
	if (auto casted = resultType.dyn_cast<mlir::rlc::TemplateParameterType>())
	{
		assert(substitutions.find(casted) != substitutions.end());
		resultType = substitutions[casted];
	}
	else if (isTemplateType(resultType).succeeded())
	{
		for (auto sobstitution : substitutions)
		{
			resultType =
					resultType.replace([sobstitution](mlir::Type type) -> mlir::Type {
						auto t = type.dyn_cast<mlir::rlc::TemplateParameterType>();
						if (not t or t != sobstitution.getFirst())
							return type;
						return sobstitution.second;
					});
		}
	}

	return mlir::FunctionType::get(
			possibleCallee.getContext(),
			callSiteArgumentTypes,
			mlir::TypeRange(resultType));
}

static mlir::Value findBestMatch(
		mlir::ValueRange candidates,
		mlir::TypeRange arguments,
		mlir::DenseMap<mlir::Type, bool>& isTemplateTypeCache)
{
	llvm::SmallVector<mlir::Value> nonTemplates;
	llvm::SmallVector<mlir::Value> templates;

	for (auto candidate : candidates)
	{
		if (mlir::rlc::isTemplateType(candidate.getType(), isTemplateTypeCache)
						.succeeded())
			templates.push_back(candidate);
		else
			nonTemplates.push_back(candidate);
	}

	if (nonTemplates.size() > 1)
		return nullptr;

	if (nonTemplates.size() == 1)
		return nonTemplates[0];

	return templates[0];
}

mlir::Value mlir::rlc::OverloadResolver::findOverload(
		mlir::Location callPoint,
		bool isMemberCall,
		llvm::StringRef name,
		mlir::TypeRange arguments)
{
	llvm::SmallVector<mlir::Value> matching =
			findOverloads(callPoint, isMemberCall, name, arguments);

	if (not matching.empty())
		if (auto value = findBestMatch(matching, arguments, isTemplate))
			return value;

	if (errorEmitter)
	{
		std::string error;
		llvm::raw_string_ostream stream(error);
		stream << "Could not find matching function " << name << "(";
		size_t index = 1;
		for (auto argument : arguments)
		{
			stream << prettyType(argument);
			if (index != arguments.size())
				stream << ",";
			index++;
		}
		stream << ")";
		stream.flush();

		auto _ = logError(errorEmitter, error);
	}

	for (auto candidate : symbolTable->get(name))
	{
		if (not candidate.getType().isa<mlir::FunctionType>())
			continue;
		if (errorEmitter)
			auto _ = logRemark(
					candidate.getDefiningOp(),
					("Candidate: " + prettyType(candidate.getType())));
	}
	return nullptr;
}

static bool locsAreInSameFile(mlir::Location l, mlir::Location r)
{
	auto casted1 = l.cast<mlir::FileLineColLoc>();
	auto catsed2 = r.cast<mlir::FileLineColLoc>();
	return casted1.getFilename() == catsed2.getFilename();
}

static llvm::SmallVector<mlir::Type, 2> templateArguments(mlir::Operation* op)
{
	llvm::SmallVector<mlir::Type, 2> toReturn;
	auto function = llvm::dyn_cast<mlir::rlc::FunctionOp>(op);
	if (!function)
		return toReturn;
	for (auto parameter : function.getTemplateParameters())
	{
		toReturn.push_back(mlir::cast<mlir::TypeAttr>(parameter).getValue());
	}
	return toReturn;
}

llvm::SmallVector<mlir::Value, 2> mlir::rlc::OverloadResolver::findOverloads(
		mlir::Location callPoint,
		bool isMemberCall,
		llvm::StringRef name,
		mlir::TypeRange arguments)
{
	llvm::SmallVector<mlir::Value, 2> matching;
	for (auto candidate : symbolTable->get(name))
	{
		auto casted = candidate.getType().dyn_cast<mlir::FunctionType>();
		if (not casted)
			continue;

		if (casted.getNumInputs() != arguments.size())
			continue;

		if (name.starts_with("_") and
				not locsAreInSameFile(candidate.getDefiningOp()->getLoc(), callPoint))
			continue;

		if (auto casted = candidate.getDefiningOp<mlir::rlc::FunctionOp>())
		{
			if (casted.getIsMemberFunction() != isMemberCall)
				continue;
		}

		if (deduceTemplateCallSiteType(
						callPoint,
						arguments,
						candidate.getType().cast<mlir::FunctionType>(),
						templateArguments(candidate.getDefiningOp())) != nullptr)
		{
			matching.push_back(candidate);
		}
	}
	return matching;
}

mlir::Value mlir::rlc::OverloadResolver::instantiateOverload(
		mlir::IRRewriter& rewriter,
		bool isMemberCall,
		mlir::Location loc,
		llvm::StringRef name,
		mlir::TypeRange arguments)
{
	auto overload = findOverload(loc, isMemberCall, name, arguments);
	if (not overload)
		return overload;

	auto instantiated = deduceTemplateCallSiteType(
			loc,
			arguments,
			overload.getType().cast<mlir::FunctionType>(),
			templateArguments(overload.getDefiningOp()));
	if (isTemplateType(overload.getType(), isTemplate).failed())
		return overload;

	return rewriter.create<mlir::rlc::TemplateInstantiationOp>(
			loc, instantiated, overload);
}
