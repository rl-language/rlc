#pragma once

#include <bits/stdint-intn.h>
#include <cstddef>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "llvm/ADT/StringRef.h"
#include "llvm/Support/raw_ostream.h"
namespace rlc
{
	enum class Token
	{
		End,
		Begin,
		KeywordSystem,
		KeywordTrait,
		KeywordIs,
		KeywordAnd,
		KeywordOr,
		KeywordRule,
		KeywordEvent,
		KeywordAction,
		KeywordEntity,
		KeywordIf,
		KeywordElse,
		KeywordWhile,
		KeywordFor,
		KeywordReturn,
		KeywordEnable,
		KeywordTrue,
		KeywordFalse,
		KeywordIn,
		KeywordLet,
		KeywordFun,
		KeywordReq,
		KeywordType,
		Indent,
		Deindent,
		Newline,
		Arrow,
		Plus,
		Minus,
		Mult,
		Divide,
		Module,
		LPar,
		RPar,
		RSquare,
		LSquare,
		LBracket,
		RBracket,
		LEqual,
		GEqual,
		EqualEqual,
		NEqual,
		ExMark,
		Comma,
		LAng,
		RAng,
		Equal,
		Colons,
		Identifier,
		Double,
		Int64,
		Bool,
		Dot,
		Error
	};

	llvm::StringRef tokenToString(Token t);

	class Lexer
	{
		public:
		Lexer(const char* i): in(i), indentStack({ 0 }) {}

		Token next();

		[[nodiscard]] int64_t lastInt64() const { return lInt64; }
		[[nodiscard]] double lastDouble() const { return lDouble; }
		[[nodiscard]] llvm::StringRef lastIndent() const { return lIdent; }
		[[nodiscard]] size_t getCurrentColumn() const { return currentColumn; }
		[[nodiscard]] size_t getCurrentLine() const { return currentLine; }

		void print(llvm::raw_ostream& OS);
		void dump();

		private:
		char eatChar();
		std::optional<Token> eatSpaces();
		std::optional<Token> eatSymbol();
		std::optional<Token> twoSymbols(char current);
		Token eatIdent();
		Token eatNumber();

		const char* in;
		size_t currentLine{ 1 };
		size_t currentColumn{ 1 };
		std::vector<size_t> indentStack;
		bool newLine{ true };

		int64_t lInt64{ 0 };
		double lDouble{ 0 };
		std::string lIdent{ "" };
		size_t deindentToEmit{ 0 };
	};
}	 // namespace rlc
