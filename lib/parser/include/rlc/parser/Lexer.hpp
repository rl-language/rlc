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
		KeywordBitAnd,
		KeywordAnd,
		KeywordBitXor,
		KeywordOr,
		KeywordRule,
		KeywordEnum,
		KeywordMalloc,
		KeywordAssert,
		KeywordDestroy,
		KeywordContstruct,
		KeywordCan,
		KeywordBreak,
		KeywordContinue,
		KeywordFree,
		KeywordRef,
		KeywordToArray,
		KeywordFromArray,
		KeywordEvent,
		KeywordUsing,
		KeywordAlternative,
		KeywordAction,
		KeywordSubAction,
		KeywordFrame,
		KeywordCtx,
		KeywordClass,
		KeywordIf,
		KeywordElse,
		KeywordWhile,
		KeywordOwningPtr,
		KeywordFor,
		KeywordOf,
		KeywordReturn,
		KeywordEnable,
		KeywordTrue,
		KeywordFalse,
		KeywordIn,
		KeywordLet,
		KeywordActions,
		KeywordFun,
		KeywordReq,
		KeywordType,
		KeywordExtern,
		KeywordImport,
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
		ColonsColons,
		Identifier,
		String,
		Character,
		Double,
		Int64,
		Bool,
		Dot,
		Tilde,
		Error,
		VerticalPipe
	};

	llvm::StringRef tokenToString(Token t);

	class Lexer
	{
		public:
		Lexer(const char* i): in(i), indentStack({ 0 })
		{
			if (*in == '\r')
				in++;
		}

		Token next();

		[[nodiscard]] int64_t lastInt64() const { return lInt64; }
		[[nodiscard]] double lastDouble() const { return lDouble; }
		[[nodiscard]] llvm::StringRef lastString() const { return lString; }
		[[nodiscard]] llvm::StringRef lastIndent() const { return lIdent; }
		[[nodiscard]] size_t getCurrentColumn() const { return currentColumn; }
		[[nodiscard]] size_t getCurrentLine() const { return currentLine; }
		// returns the comments before the last token but after the previous tokens.
		[[nodiscard]] llvm::StringRef getLastComment() const { return lComment; }

		void print(llvm::raw_ostream& OS);
		void dump();

		private:
		Token nextWithoutTrailingConsume();
		char eatChar();
		bool eatComment();
		void consumeWhiteSpaceUntilNextMeaningfullChar();
		std::optional<Token> eatSpaces();
		std::optional<Token> eatSymbol();
		Token eatString();
		char eatCharLiteral();
		std::optional<Token> twoSymbols(char current);
		Token eatIdent();
		Token eatNumber();

		const char* in;
		size_t currentLine{ 1 };
		size_t currentColumn{ 1 };
		std::vector<size_t> indentStack;
		bool newLine{ true };
		int64_t nestedParentesys{ 0 };

		int64_t lInt64{ 0 };
		double lDouble{ 0 };
		std::string lIdent{ "" };
		std::string lString{ "" };
		std::string lComment{ "" };
		size_t deindentToEmit{ 0 };
		bool parsingString = false;
		bool emittedExtraNewLine = false;
	};
}	 // namespace rlc
