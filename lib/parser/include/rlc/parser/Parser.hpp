#pragma once

#include <string>

#include "llvm/Support/Error.h"
#include "rlc/ast/Entity.hpp"
#include "rlc/ast/EntityDeclaration.hpp"
#include "rlc/ast/Expression.hpp"
#include "rlc/ast/Statement.hpp"
#include "rlc/parser/Lexer.hpp"
#include "rlc/utils/SourcePosition.hpp"

namespace rlc
{
	class Parser
	{
		public:
		Parser(std::string source, std::string sourceName)
				: pos(sourceName), toParse(std::move(source)), lexer(toParse.data())
		{
			next();
		}

		[[nodiscard]] SourcePosition getCurrentSourcePos() const { return pos; }

		llvm::Expected<Expression> primaryExpression();
		llvm::Expected<Expression> postFixExpression();
		llvm::Expected<Expression> expression();
		llvm::Expected<Expression> unaryExpression();
		llvm::Expected<Expression> assignmentExpression();
		llvm::Expected<Expression> multyplicativeExpression();
		llvm::Expected<Expression> additiveExpression();
		llvm::Expected<Expression> relationalExpression();
		llvm::Expected<Expression> equalityExpression();
		llvm::Expected<Expression> andExpression();
		llvm::Expected<Expression> orExpression();
		llvm::Expected<EntityDeclaration> entityDeclaration();
		llvm::Expected<EntityField> entityField();
		llvm::Expected<llvm::SmallVector<Expression, 3>> argumentExpressionList();

		llvm::Expected<Statement> expressionStatement();
		llvm::Expected<Statement> ifStatement();
		llvm::Expected<Statement> statement();
		llvm::Expected<Statement> declarationStatement();
		llvm::Expected<Statement> statementList();
		llvm::Expected<Statement> whileStatement();
		llvm::Expected<Statement> returnStatement();

		private:
		void next();
		bool accept(Token t);
		template<Token T>
		bool accept()
		{
			return accept(T);
		}
		llvm::Expected<Token> expect(Token t);

		Token current;
		SourcePosition pos;
		std::string toParse;
		Lexer lexer;
		int64_t lInt64{ 0 };
		double lDouble{ 0 };
		std::string lIdent{ "" };
	};
}	 // namespace rlc
