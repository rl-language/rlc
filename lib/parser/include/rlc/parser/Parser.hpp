#pragma once

#include <string>

#include "llvm/Support/Error.h"
#include "rlc/ast/Expression.hpp"
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
		llvm::Expected<Expression> multyplicativeExpression();
		llvm::Expected<Expression> additiveExpression();
		llvm::Expected<Expression> relationalExpression();
		llvm::Expected<Expression> equalityExpression();
		llvm::Expected<Expression> andExpression();
		llvm::Expected<Expression> orExpression();
		llvm::Expected<llvm::SmallVector<Expression, 3>> argumentExpressionList();

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
	};
}	 // namespace rlc
