#pragma once

#include "mlir/Analysis/Liveness.h"
#include "rlc/dialect/Operations.hpp"

namespace mlir::rlc
{
	namespace detail
	{
		class ActionValueLivenessImpl;
	};

	class ActionValueLiveness
	{
		public:
		ActionValueLiveness(mlir::rlc::ActionFunction action);
		~ActionValueLiveness();

		bool isDeadAfter(mlir::Value value, mlir::rlc::ActionStatement statements);

		private:
		detail::ActionValueLivenessImpl* impl;
	};

	class ActionLiveness
	{
		public:
		template<typename T>
		struct Partition
		{
			llvm::SmallVector<T, 4> usedAcrossActions;
			llvm::SmallVector<T, 4> notUsedAcrossActions;
		};

		explicit ActionLiveness(mlir::rlc::ActionFunction action)
				: fun(action), liveness(action)
		{
			valueToSurvivingFrameIndex[fun.getResult()] = 0;
			getFrameTypes();
		}
		bool isUsedAcrossActions(mlir::Value val);

		const Partition<mlir::rlc::DeclarationStatement>&
		getDeclarationsUsedAcrossActions();

		const Partition<mlir::OpResult>& getStatementsArgsUsedAcrossActions();
		const Partition<mlir::BlockArgument>& getArgumentsUsedAcrossActions();

		private:
		struct ActionFrames
		{
			llvm::SmallVector<std::pair<std::string, mlir::Type>> usedAcrossActions;
			llvm::SmallVector<std::pair<std::string, mlir::Type>>
					notUsedAcrossActions;
		};

		ActionFrames getFrames();

		public:
		struct FrameTypes
		{
			EntityType frameUsedAcrossSections;
			EntityType frameNotUsedAcrossSections;
		};

		FrameTypes getFrameTypes()
		{
			if (types.frameUsedAcrossSections != nullptr)
				return types;
			ActionFrames frame = getFrames();

			{
				llvm::SmallVector<mlir::Type, 4> memberTypes;
				llvm::SmallVector<std::string, 4> memberNames;

				// add the implicit local variable "resume_index" to members
				memberTypes.push_back(
						mlir::rlc::IntegerType::getInt64(fun.getContext()));
				memberNames.push_back("resume_index");

				for (auto& info : frame.usedAcrossActions)
				{
					memberNames.push_back(info.first);
					memberTypes.push_back(info.second);
				}

				// add the types of all named members to the action's type.
				auto res =
						fun.getEntityType().setBody(memberTypes, memberNames).succeeded();
				assert(res);

				types.frameUsedAcrossSections = fun.getEntityType();
			}

			{
				llvm::SmallVector<mlir::Type, 4> memberTypes;
				llvm::SmallVector<std::string, 4> memberNames;

				for (auto& info : frame.notUsedAcrossActions)
				{
					memberNames.push_back(info.first);
					memberTypes.push_back(mlir::rlc::ReferenceType::get(
							info.second.getContext(), info.second));
				}

				auto res = EntityType::getIdentified(
						fun.getContext(),
						(types.frameUsedAcrossSections.getName() + "_shadow").str(),
						{});
				auto isOk = res.setBody(memberTypes, memberNames).succeeded();
				assert(isOk);

				types.frameNotUsedAcrossSections = res;
			}

			return types;
		}

		std::optional<size_t> indexInExternalFrame(mlir::Value val)
		{
			if (not valueToSurvivingFrameIndex.contains(val))
				return std::nullopt;
			return valueToSurvivingFrameIndex[val];
		}

		std::optional<size_t> indexInHiddenFrame(mlir::Value val)
		{
			if (not valueToHiddenFrameIndex.contains(val))
				return std::nullopt;
			return valueToHiddenFrameIndex[val];
		}

		bool variableIsInHiddenFrame(mlir::Value val)
		{
			return valueToHiddenFrameIndex.contains(val);
		}

		bool variableIsInExternalFrame(mlir::Value val)
		{
			return valueToSurvivingFrameIndex.contains(val);
		}

		bool isMainFunctionArgInExternalFrame(size_t index)
		{
			return mainFunctionArgToFrameIndex[index].first;
		}

		size_t mainFunctionArgIndexInFrame(size_t index)
		{
			return mainFunctionArgToFrameIndex[index].second;
		}

		bool isMainFunctionArgInHiddenFrame(size_t index)
		{
			return not mainFunctionArgToFrameIndex[index].first;
		}

		private:
		llvm::DenseMap<mlir::Value, bool> cache;
		ActionFunction fun;
		ActionValueLiveness liveness;

		std::optional<Partition<mlir::rlc::DeclarationStatement>> declsStatements =
				std::nullopt;

		std::optional<Partition<mlir::OpResult>> operands = std::nullopt;

		std::optional<Partition<mlir::BlockArgument>> mainFunArgs = std::nullopt;

		llvm::DenseMap<size_t, std::pair<bool, size_t>> mainFunctionArgToFrameIndex;
		llvm::DenseMap<mlir::Value, size_t> valueToSurvivingFrameIndex;
		llvm::DenseMap<mlir::Value, size_t> valueToHiddenFrameIndex;
		FrameTypes types = {};
	};

}	 // namespace mlir::rlc
