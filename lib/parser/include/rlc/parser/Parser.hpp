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
		llvm::Expected<mlir::Value> expression();
		llvm::Expected<mlir::Value> unaryExpression();
		llvm::Expected<mlir::Value> assignmentExpression();
		llvm::Expected<mlir::Value> multyplicativeExpression();
		llvm::Expected<mlir::Value> additiveExpression();
		llvm::Expected<mlir::Value> relationalExpression();
		llvm::Expected<mlir::Value> equalityExpression();
		llvm::Expected<mlir::Value> andExpression();
		llvm::Expected<mlir::Value> orExpression();
		llvm::Expected<mlir::rlc::EntityDeclaration> entityDeclaration();
		llvm::Expected<std::pair<std::string, mlir::rlc::ScalarUseType>>
		entityField();
		llvm::Expected<llvm::SmallVector<mlir::Value, 3>> argumentExpressionList();

		llvm::Expected<mlir::rlc::ExpressionStatement> expressionStatement();
		llvm::Expected<mlir::rlc::IfStatement> ifStatement();
		llvm::Expected<mlir::Operation*> statement();
		llvm::Expected<mlir::rlc::ActionStatement> actionStatement();
		llvm::Expected<mlir::rlc::DeclarationStatement> declarationStatement();
		llvm::Expected<bool> statementList();
		llvm::Expected<bool> requirementList();
		llvm::Expected<mlir::rlc::WhileStatement> whileStatement();
		llvm::Expected<mlir::rlc::ReturnStatement> returnStatement();

		llvm::Expected<
				llvm::SmallVector<std::pair<std::string, mlir::rlc::ScalarUseType>, 3>>
		functionArguments();
		llvm::Expected<std::pair<std::string, mlir::rlc::ScalarUseType>>
		argDeclaration();

		llvm::Expected<mlir::rlc::ScalarUseType> singleTypeUse();
		llvm::Expected<mlir::rlc::FunctionUseType> functionTypeUse();
		llvm::Expected<mlir::rlc::FunctionOp> functionDefinition();
		llvm::Expected<FunctionDeclarationResult> functionDeclaration();
		llvm::Expected<mlir::rlc::ActionFunction> actionDeclaration();
		llvm::Expected<mlir::rlc::ActionFunction> actionDefinition();
		llvm::Expected<mlir::rlc::UncheckedTraitDefinition> traitDefinition();

		llvm::Expected<mlir::ModuleOp> system();

		void emitYieldIfNeeded(mlir::Location loc);

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
		double lDouble{ 0 };
		std::string lIdent;
	};
}	 // namespace rlc
