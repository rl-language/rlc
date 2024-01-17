/*
This file is part of the RLC project.

RLC is free software: you can redistribute it and/or modify it under the terms
of the GNU General Public License version 2 as published by the Free Software
Foundation.

RLC is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
RLC. If not, see <https://www.gnu.org/licenses/>.
*/
#pragma once

#include <string>

#include "llvm/Support/Error.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/IR/Diagnostics.h"
#include "rlc/dialect/Operations.hpp"
#include "rlc/parser/Lexer.hpp"

namespace rlc
{
	class Parser
	{
		public:
		struct FunctionDeclarationResult
		{
			mlir::rlc::FunctionOp op;
			llvm::SmallVector<mlir::Location> argLocs;
		};

		Parser(mlir::MLIRContext* ctx, std::string source, std::string sourceName)
				: ctx(ctx),
					builder(ctx),
					pos(mlir::FileLineColLoc::get(ctx, sourceName, 1, 1)),
					toParse(std::move(source)),
					fileName(sourceName),
					lexer(toParse.data())

		{
			next();
		}

		[[nodiscard]] mlir::Location getCurrentSourcePos() const;

		llvm::Expected<mlir::Value> primaryExpression();
		llvm::Expected<mlir::Value> postFixExpression();
		llvm::Expected<mlir::Value> builtinMalloc();
		llvm::Expected<mlir::Value> builtinFromArray();
		llvm::Expected<mlir::Value> builtinToArray();
		llvm::Expected<mlir::rlc::FreeOp> builtinFree();
		llvm::Expected<mlir::Operation*> builtinDestroy();
		llvm::Expected<mlir::Value> expression();
		llvm::Expected<mlir::Value> unaryExpression();
		llvm::Expected<mlir::rlc::EnumDeclarationOp> enumDeclaration();
		llvm::Expected<mlir::Value> initializerList();
		llvm::Expected<mlir::Value> enumUse();
		llvm::Expected<mlir::Value> multyplicativeExpression();
		llvm::Expected<mlir::Value> additiveExpression();
		llvm::Expected<mlir::Value> relationalExpression();
		llvm::Expected<mlir::Value> equalityExpression();
		llvm::Expected<mlir::Value> andExpression();
		llvm::Expected<mlir::Value> orExpression();
		llvm::Expected<mlir::rlc::UsingTypeOp> usingTypeStatement();
		llvm::Expected<mlir::rlc::EntityDeclaration> entityDeclaration();
		llvm::Expected<std::pair<std::string, mlir::Type>> entityField();
		llvm::Expected<llvm::SmallVector<mlir::Value, 3>> argumentExpressionList();

		llvm::Expected<mlir::rlc::ExpressionStatement> expressionStatement();
		llvm::Expected<mlir::rlc::IfStatement> ifStatement();
		llvm::Expected<mlir::Operation*> statement();
		llvm::Expected<mlir::rlc::ActionStatement> actionStatement();
		llvm::Expected<mlir::rlc::ActionsStatement> actionsStatement();
		llvm::Expected<mlir::rlc::DeclarationStatement> declarationStatement();
		llvm::Expected<bool> statementList();
		llvm::Expected<bool> requirementList();
		llvm::Expected<mlir::rlc::WhileStatement> whileStatement();
		llvm::Expected<mlir::rlc::ForFieldStatement> forFieldStatement();
		llvm::Expected<mlir::rlc::ReturnStatement> returnStatement();
		llvm::Expected<mlir::rlc::SubActionStatement> subActionStatement();

		llvm::Expected<llvm::SmallVector<std::pair<std::string, mlir::Type>, 3>>
		functionArguments();
		llvm::Expected<std::pair<std::string, mlir::Type>> argDeclaration();

		llvm::Expected<mlir::Type> singleTypeUse();
		llvm::Expected<mlir::rlc::ScalarUseType> singleNonArrayTypeUse();
		llvm::Expected<mlir::rlc::FunctionUseType> functionTypeUse();
		llvm::Expected<mlir::rlc::FunctionOp> functionDefinition(
				bool isMemberFunction = false);
		llvm::Expected<
				llvm::SmallVector<mlir::rlc::UncheckedTemplateParameterType, 2>>
		templateArguments();
		llvm::Expected<FunctionDeclarationResult> functionDeclaration(
				bool templateFunction = true, bool isMemberFunction = false);
		llvm::Expected<FunctionDeclarationResult> externFunctionDeclaration();
		llvm::Expected<mlir::rlc::ActionFunction> actionDeclaration(
				bool needsReturnType);
		llvm::Expected<mlir::rlc::ActionFunction> actionDefinition();
		llvm::Expected<mlir::rlc::UncheckedTraitDefinition> traitDefinition();

		llvm::Expected<mlir::ModuleOp> system(mlir::ModuleOp module = nullptr);

		void emitYieldIfNeeded(mlir::Location loc);
		llvm::ArrayRef<std::string> getImportedFiles() const
		{
			return importedFiles;
		}

		private:
		mlir::Type unkType();
		mlir::MLIRContext* ctx;
		mlir::OpBuilder builder;
		void next();
		bool accept(Token t);
		template<Token T>
		bool accept()
		{
			return accept(T);
		}
		llvm::Expected<Token> expect(Token t);

		Token current;
		mlir::Location pos;
		std::string toParse;
		std::string fileName;
		Lexer lexer;
		int64_t lInt64{ 0 };
		int64_t currentTemplateTypeIndex{ 0 };
		double lDouble{ 0 };
		std::string lIdent;

		llvm::SmallVector<std::string, 4> importedFiles;
	};
}	 // namespace rlc
