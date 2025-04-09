
/*
Copyright 2025 Massimo Fioravanti

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
#pragma once

#include "llvm/Support/raw_ostream.h"
#include "mlir/IR/BuiltinOps.h"

namespace mlir::rlc
{

	class StreamWriter
	{
		public:
		StreamWriter(llvm::raw_ostream& out): OS(&out) {}
		void write(llvm::StringRef to_print) { *OS << to_print; }

		private:
		llvm::raw_ostream* OS;
	};

	class PatternImpl
	{
		public:
		virtual ~PatternImpl() = default;
		virtual bool matches(mlir::Operation* op) = 0;
		virtual void apply(mlir::Operation* op, StreamWriter& OS) = 0;
	};

	template<typename ObjectType, typename OperationType>
	OperationType getOpTypeImpl(
			void (ObjectType::*)(OperationType, StreamWriter&))
	{
		return OperationType(nullptr);
	}

	template<typename Matcher>
	class Pattern: public PatternImpl
	{
		public:
		using OperationType = decltype(getOpTypeImpl(&Matcher::apply));

		Pattern(Matcher&& matcher): matcher(std::forward<Matcher>(matcher)) {}

		virtual bool matches(mlir::Operation* op) final
		{
			return mlir::isa<OperationType>(op);
		}

		virtual void apply(mlir::Operation* op, StreamWriter& OS) final
		{
			matcher.apply(mlir::cast<OperationType>(op), OS);
		}

		private:
		Matcher matcher;
	};

	class PatternMatcher
	{
		public:
		PatternMatcher(llvm::raw_ostream& out): OS(out) {}

		void apply(mlir::ModuleOp op)
		{
			op.walk([this](mlir::Operation* current) {
				for (auto& pattern : patterns)
				{
					if (pattern->matches(current))
					{
						pattern->apply(current, OS);
						break;
					}
				}
			});
		}

		template<typename Matcher>
		void add(Matcher&& to_add)
		{
			patterns.push_back(
					std::make_unique<Pattern<Matcher>>(std::forward<Matcher>(to_add)));
		}

		template<typename Matcher>
		void add()
		{
			patterns.push_back(std::make_unique<Pattern<Matcher>>(Matcher()));
		}

		private:
		std::vector<std::unique_ptr<PatternImpl>> patterns;
		StreamWriter OS;
	};
}	 // namespace mlir::rlc
