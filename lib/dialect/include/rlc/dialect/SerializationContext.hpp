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

#pragma once

#include "llvm/Support/raw_ostream.h"
namespace mlir::rlc
{
	class ModuleBuilder;
	class SerializationContext
	{
		public:
		SerializationContext(ModuleBuilder& builder): builder(&builder) {}
		SerializationContext(): builder(nullptr) {}
		class IndentGuard
		{
			public:
			IndentGuard(SerializationContext& ctx): ctx(&ctx) { ctx.indentLevel++; }
			~IndentGuard() { ctx->indentLevel--; }

			private:
			SerializationContext* ctx;
		};

		class PreconditionGuard
		{
			public:
			PreconditionGuard(SerializationContext& ctx, int value = 1): ctx(&ctx)
			{
				ctx.emittingPrecondition += value;
			}
			~PreconditionGuard() { ctx->emittingPrecondition--; }

			private:
			SerializationContext* ctx;
		};

		void indent(llvm::raw_ostream& OS) { OS.indent(indentLevel * 2); }
		IndentGuard increaseIndent() { return IndentGuard(*this); }
		PreconditionGuard startEmittingPrecondition()
		{
			return PreconditionGuard(*this);
		}

		ModuleBuilder& getBuilder() { return *builder; }
		bool isInPrecondition() { return emittingPrecondition == 1; }

		private:
		int indentLevel = 0;
		int emittingPrecondition = 0;
		ModuleBuilder* builder;
	};
}	 // namespace mlir::rlc
