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

#include <string>

#include "llvm/Support/Error.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/IR/Diagnostics.h"
#include "rlc/dialect/IRBuilder.hpp"
#include "rlc/dialect/Operations.hpp"
#include "rlc/parser/Lexer.hpp"

namespace rlc
{
	class Parser
	{
		public:
		Parser(
				mlir::MLIRContext* ctx,
				std::string source,
				std::string sourceName,
				bool attachComments = false)
				: ctx(ctx),
					builder(ctx),
					pos(mlir::FileLineColLoc::get(ctx, sourceName, 1, 1)),
					lastTokenEndpos(mlir::FileLineColLoc::get(ctx, sourceName, 1, 1)),
					toParse(std::move(source)),
					fileName(sourceName),
					lexer(toParse.data()),
					attachComments(attachComments)

		{
			next();
		}

		[[nodiscard]] mlir::Location getCurrentSourcePos() const;
		[[nodiscard]] mlir::Location getLastTokenEndPos() const;

		llvm::Expected<mlir::Value> primaryExpression();
		llvm::Expected<mlir::rlc::Comment> comment();
		llvm::Expected<mlir::Value> postFixExpression();
		llvm::Expected<mlir::rlc::AssertOp> assertStatement();
		llvm::Expected<mlir::Value> canCallExpression();
		llvm::Expected<mlir::Value> builtinMalloc();
		llvm::Expected<mlir::Value> builtinFromArray();
		llvm::Expected<mlir::Value> builtinToArray();
		llvm::Expected<mlir::rlc::FreeOp> builtinFree();
		llvm::Expected<mlir::Operation*> builtinDestroy();
		llvm::Expected<mlir::Value> builtinMangledName();
		llvm::Expected<mlir::Value> builtinAsPtr();
		llvm::Expected<mlir::Operation*> builtinConstruct();
		llvm::Expected<mlir::Value> expression();
		llvm::Expected<mlir::Value> unaryExpression();
		llvm::Expected<mlir::rlc::EnumFieldDeclarationOp> enumFieldDeclaration();
		llvm::Expected<mlir::rlc::EnumDeclarationOp> enumDeclaration();
		llvm::Expected<mlir::Value> initializerList();
		llvm::Expected<mlir::Value> enumUse();
		llvm::Expected<mlir::Value> multyplicativeExpression();
		llvm::Expected<mlir::Value> additiveExpression();
		llvm::Expected<mlir::Value> relationalExpression();
		llvm::Expected<mlir::Value> equalityExpression();
		llvm::Expected<mlir::Value> andExpression();
		llvm::Expected<mlir::Value> orExpression();
		llvm::Expected<mlir::Value> bitWiseAndExpression();
		llvm::Expected<mlir::Value> bitWiseXorExpression();
		llvm::Expected<mlir::Value> bitWiseOrExpression();
		llvm::Expected<mlir::Value> stringExpression();
		llvm::Expected<mlir::Operation*> usingTypeStatement();
		llvm::Expected<mlir::rlc::ClassDeclaration> classDeclaration();
		llvm::Expected<mlir::rlc::ClassFieldDeclarationAttr> classField();
		llvm::Expected<llvm::SmallVector<mlir::Value, 3>> argumentExpressionList();
		llvm::Expected<mlir::rlc::TypeAliasOp> usingStatement();

		llvm::Expected<mlir::rlc::ExpressionStatement> expressionStatement();
		llvm::Expected<mlir::rlc::IfStatement> ifStatement();
		llvm::Expected<mlir::Operation*> statement();
		llvm::Expected<mlir::rlc::ActionStatement> actionStatement();
		llvm::Expected<mlir::rlc::ActionsStatement> actionsStatement();
		llvm::Expected<mlir::rlc::DeclarationStatement> declarationStatement();
		llvm::Expected<bool> statementList();
		llvm::Expected<bool> requirementList();
		llvm::Expected<mlir::rlc::WhileStatement> whileStatement();
		llvm::Expected<mlir::rlc::ForLoopStatement> forLoopStatement(
				llvm::StringRef varName);
		llvm::Expected<mlir::Operation*> forFieldStatement();
		llvm::Expected<mlir::rlc::ReturnStatement> returnStatement();
		llvm::Expected<mlir::rlc::BreakStatement> breakStatement();
		llvm::Expected<mlir::rlc::ContinueStatement> continueStatement();
		llvm::Expected<mlir::rlc::SubActionStatement> subActionStatement();

		llvm::Expected<llvm::SmallVector<mlir::rlc::FunctionArgumentAttr, 3>>
		functionArguments();
		llvm::Expected<mlir::rlc::FunctionArgumentAttr> argDeclaration();

		llvm::Expected<mlir::rlc::ShugarizedTypeAttr> singleTypeUse();
		llvm::Expected<mlir::rlc::ScalarUseType> singleNonArrayTypeUse();
		llvm::Expected<mlir::rlc::FunctionUseType> functionTypeUse();
		llvm::Expected<mlir::rlc::FunctionOp> functionDefinition(
				bool isMemberFunction = false);
		llvm::Expected<
				llvm::SmallVector<mlir::rlc::UncheckedTemplateParameterType, 2>>
		templateArguments();
		llvm::Expected<mlir::rlc::FunctionOp> functionDeclaration(
				bool templateFunction = true, bool isMemberFunction = false);
		llvm::Expected<mlir::rlc::FunctionOp> externFunctionDeclaration();
		llvm::Expected<mlir::Operation*> actionDeclaration(bool actionFunction);
		llvm::Expected<mlir::rlc::ConstantGlobalOp> globalConstant();
		llvm::Expected<mlir::rlc::ActionFunction> actionDefinition();
		llvm::Expected<mlir::rlc::UncheckedTraitDefinition> traitDefinition();

		llvm::Expected<mlir::ModuleOp> system(mlir::ModuleOp module = nullptr);

		void emitYieldIfNeeded(mlir::Location loc);
		void removeYieldIfNotNeeded();
		llvm::ArrayRef<std::string> getImportedFiles() const
		{
			return importedFiles;
		}

		mlir::Builder& getBuilder() { return builder; }

		private:
		mlir::Type unkType();
		mlir::MLIRContext* ctx;
		mlir::rlc::IRBuilder builder;
		void next();
		bool accept(Token t);
		template<Token T>
		bool accept()
		{
			return accept(T);
		}
		llvm::Expected<Token> expect(Token t);
		llvm::Expected<Token> expectEndOfLine();
		bool acceptEndOfLine();

		Token current;
		mlir::Location pos;
		mlir::Location lastTokenEndpos;
		std::string toParse;
		std::string fileName;
		Lexer lexer;
		int64_t lInt64{ 0 };
		int64_t currentTemplateTypeIndex{ 0 };
		double lDouble{ 0 };
		std::string lIdent;
		std::string lString;
		bool attachComments{ false };

		llvm::SmallVector<std::string, 4> importedFiles;
	};
}	 // namespace rlc
