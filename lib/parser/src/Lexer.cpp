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
#include "rlc/parser/Lexer.hpp"

#include <cctype>
#include <optional>
#include <string>

#include "llvm/ADT/StringRef.h"

using namespace std;
using namespace rlc;

char Lexer::eatChar()
{
	const char c = *in;
	currentColumn++;
	newLine = c == '\n';
	if (c == '\n')
	{
		currentLine++;
		currentColumn = 1;
		lastNonWitheSpaceColumn = 1;
	}
	if (not isspace(c))
		lastNonWitheSpaceColumn = currentColumn;
	if ((c == '(' or c == '{' or c == '[') and not parsingString and
			not parsingComment)
	{
		nestedParentesys++;
	}
	if ((c == ')' or c == '}' or c == ']') and not parsingString and
			not parsingComment)
	{
		nestedParentesys--;
	}

	in++;
	if (c != '\0' and *in == '\r')
		in++;
	return c;
}

void Lexer::dump() { print(llvm::outs()); }

void Lexer::print(llvm::raw_ostream& OS)
{
	auto token = next();
	while (token != Token::End)
	{
		OS << tokenToString(token);
		OS << " start[" << tokenStartLine << ":" << tokenStartCol << "]";
		OS << " end[" << currentLine << ":" << currentColumn << "]";
		OS << "\n";
		token = next();
	}
}

llvm::StringRef rlc::tokenToString(Token t)
{
	switch (t)
	{
		case Token::End:
			return "End";
		case Token::Begin:
			return "Begin";
		case Token::KeywordBreak:
			return "KeywordBreak";
		case Token::KeywordSystem:
			return "KeywordSystem";
		case Token::KeywordAssert:
			return "KeywordAssert";
		case Token::KeywordMangledName:
			return "KeywordMangledName";
		case Token::KeywordAsPtr:
			return "KeywordAsPtr";
		case Token::KeywordTrait:
			return "KeywordTrait";
		case Token::KeywordIs:
			return "KeywordIs";
		case Token::KeywordBitXor:
			return "KeywordBitXor";
		case Token::KeywordActionClass:
			return "KeywordActionClass";
		case Token::KeywordBitAnd:
			return "KeywordBitAnd";
		case Token::KeywordAnd:
			return "KeywordAnd";
		case Token::KeywordOr:
			return "KeywordOr";
		case Token::KeywordConst:
			return "KeywordConst";
		case Token::KeywordRule:
			return "KeywordRule";
		case Token::KeywordContinue:
			return "KeywordContinue";
		case Token::KeywordEvent:
			return "KeywordEvent";
		case Token::KeywordCan:
			return "KeywordCan";
		case Token::KeywordAction:
			return "KeywordAction";
		case Token::KeywordSubAction:
			return "KeywordSubAction";
		case Token::KeywordFrame:
			return "KeywordFrame";
		case Token::KeywordCtx:
			return "KeywordCtx";
		case Token::KeywordLet:
			return "KeywordLet";
		case Token::KeywordClass:
			return "KeywordClass";
		case Token::KeywordPass:
			return "KeywordPass";
		case Token::KeywordDestroy:
			return "KeywordDestroy";
		case Token::KeywordContstruct:
			return "KeywordContstruct";
		case Token::KeywordEnum:
			return "KeywordEnum";
		case Token::KeywordExtern:
			return "KeywordExtern";
		case Token::KeywordImport:
			return "KeywordImport";
		case Token::KeywordMalloc:
			return "KeywordMalloc";
		case Token::KeywordUsing:
			return "KeywordUsing";
		case Token::KeywordAlternative:
			return "KeywordAlternative";
		case Token::KeywordOwningPtr:
			return "KeywordOwningPtr";
		case Token::KeywordFree:
			return "KeywordFree";
		case Token::KeywordFromArray:
			return "KeywordFromArray";
		case Token::KeywordToArray:
			return "KeywordToArray";
		case Token::KeywordIf:
			return "KeywordIf";
		case Token::KeywordElse:
			return "KeywordElse";
		case Token::KeywordWhile:
			return "KeywordWhile";
		case Token::KeywordFor:
			return "KeywordFor";
		case Token::KeywordOf:
			return "KeywordOf";
		case Token::KeywordReturn:
			return "KeywordReturn";
		case Token::KeywordRef:
			return "KeywordRef";
		case Token::KeywordEnable:
			return "KeywordEnable";
		case Token::KeywordTrue:
			return "KeywordTrue";
		case Token::KeywordActions:
			return "KeywordActions";
		case Token::KeywordFalse:
			return "KeywordFalse";
		case Token::KeywordIn:
			return "KeywordIn";
		case Token::KeywordType:
			return "KeywordType";
		case Token::Indent:
			return "Indent";
		case Token::Deindent:
			return "Deindent";
		case Token::Newline:
			return "Newline";
		case Token::Plus:
			return "Plus";
		case Token::Minus:
			return "Minus";
		case Token::Mult:
			return "Mult";
		case Token::Divide:
			return "Divide";
		case Token::LPar:
			return "LPar";
		case Token::RPar:
			return "RPar";
		case Token::LAng:
			return "LAng";
		case Token::RAng:
			return "RAng";
		case Token::RSquare:
			return "RSquare";
		case Token::LSquare:
			return "LSquare";
		case Token::LBracket:
			return "LBracket";
		case Token::RBracket:
			return "RBracket";
		case Token::Colons:
			return "Colons";
		case Token::ColonsColons:
			return "ColonsColons";
		case Token::Tilde:
			return "Tilde";
		case Token::Equal:
			return "Equal";
		case Token::AnnotationIntroducer:
			return "AnnotationIntroducer";
		case Token::Identifier:
			return "Identifier";
		case Token::String:
			return "String";
		case Token::Character:
			return "Character";
		case Token::Double:
			return "Double";
		case Token::Int64:
			return "Int64";
		case Token::Bool:
			return "Bool";
		case Token::Error:
			return "Error";
		case Token::GEqual:
			return "GEqual";
		case Token::LEqual:
			return "LEqual";
		case Token::EqualEqual:
			return "EqualEqual";
		case Token::NEqual:
			return "NEqual";
		case Token::ExMark:
			return "ExMark";
		case Token::Comma:
			return "Comma";
		case Token::VerticalPipe:
			return "VerticalPipe";
		case Token::Dot:
			return "Dot";
		case Token::Module:
			return "Module";
		case Token::Arrow:
			return "Arrow";
		case Token::KeywordFun:
			return "KeywordFun";
		case Token::KeywordReq:
			return "KeywordReq";
	}
	return "";
}

void Lexer::consumeWhiteSpaceUntilNextMeaningfullChar()
{
	while (isspace(*in) != 0 and *in != '\n' and *in != '\0' and *in != '\f')
		eatChar();
	eatComment();
}

optional<Token> Lexer::eatSpaces()
{
	bool beginOfLine = newLine;
	size_t indent = 0;
	while (isspace(*in) != 0 and *in != '\n' and *in != '\f')
	{
		eatChar();
		indent++;

		// if we were eating spaces from the start of the line, and it is a line of
		// spaces, ignore it
		if (*in == '\n' and beginOfLine)
		{
			eatChar();
			indent = 0;
		}
	}

	if (!beginOfLine)
		return nullopt;

	if (indent > indentStack.back())
	{
		indentStack.push_back(indent);
		return Token::Indent;
	}

	if (indent == indentStack.back())
		return nullopt;

	if (find(indentStack.begin(), indentStack.end(), indent) == indentStack.end())
	{
		return Token::Error;
	}

	while (indentStack.back() != indent)
	{
		indentStack.erase(indentStack.end() - 1);
		deindentToEmit++;
	}

	return nullopt;
}

Token Lexer::eatNumber()
{
	string number;
	bool isHex = false;
	number += eatChar();
	if (number[0] == '0' and *in == 'x')
	{
		isHex = true;
		number += eatChar();
	}

	while (isdigit(*in) != 0 or (isHex and isxdigit(*in)))
		number += eatChar();

	if (isHex)
	{
		std::uint64_t value = 0;
		if (llvm::StringRef(number).getAsInteger(0, value))
			return Token::Error;
		memcpy(&lInt64, &value, sizeof(value));
		return Token::Int64;
	}

	if (*in != '.')
	{
		if (llvm::StringRef(number).consumeInteger(10, lInt64))
			return Token::Error;
		return Token::Int64;
	}

	number += eatChar();
	while (isdigit(*in) != 0)
		number += eatChar();

	lDouble = stod(number);
	return Token::Double;
}

optional<Token> Lexer::eatSymbol()
{
	switch (*in)
	{
		case '+':
			return Token::Plus;
		case '-':
			return Token::Minus;
		case '*':
			return Token::Mult;
		case '/':
			return Token::Divide;
		case '(':
			return Token::LPar;
		case ')':
			return Token::RPar;
		case '<':
			return Token::LAng;
		case '>':
			return Token::RAng;
		case '[':
			return Token::LSquare;
		case ']':
			return Token::RSquare;
		case '{':
			return Token::LBracket;
		case '}':
			return Token::RBracket;
		case '=':
			return Token::Equal;
		case '!':
			return Token::ExMark;
		case ':':
			return Token::Colons;
		case '%':
			return Token::Module;
		case ',':
			return Token::Comma;
		case '.':
			return Token::Dot;
		case '~':
			return Token::Tilde;
		case '&':
			return Token::KeywordBitAnd;
		case '^':
			return Token::KeywordBitXor;
		case '\n':
			return Token::Newline;
		case '|':
			return Token::VerticalPipe;
		case '@':
			return Token::AnnotationIntroducer;
	}
	return nullopt;
}

optional<Token> Lexer::twoSymbols(char current)
{
	if (current == '\n')
		while (*in == '\n')
			eatChar();

	switch (current)
	{
		case '<':
			if (*in == '=')
				return Token::LEqual;
			return nullopt;
		case '>':
			if (*in == '=')
				return Token::GEqual;
			return nullopt;
		case '=':
			if (*in == '=')
				return Token::EqualEqual;
			return nullopt;
		case '!':
			if (*in == '=')
				return Token::NEqual;
			return nullopt;
		case '-':
			if (*in == '>')
				return Token::Arrow;
			return nullopt;
		case ':':
			if (*in == ':')
				return Token::ColonsColons;
			return nullopt;
	}
	return nullopt;
}

Token Lexer::eatIdent()
{
	string name;
	name += eatChar();
	while ((isspace(*in) == 0 or *in == '_') and not eatSymbol() and *in != '\0')
		name += eatChar();

	if (name == "if")
		return Token::KeywordIf;

	if (name == "else")
		return Token::KeywordElse;

	if (name == "enum")
		return Token::KeywordEnum;

	if (name == "cls")
		return Token::KeywordClass;

	if (name == "evn")
		return Token::KeywordEvent;

	if (name == "const")
		return Token::KeywordConst;

	if (name == "rul")
		return Token::KeywordRule;

	if (name == "pass")
		return Token::KeywordPass;

	if (name == "using")
		return Token::KeywordUsing;

	if (name == "ref")
		return Token::KeywordRef;

	if (name == "is")
		return Token::KeywordIs;

	if (name == "Alternative")
		return Token::KeywordAlternative;

	if (name == "act")
		return Token::KeywordAction;

	if (name == "subaction")
		return Token::KeywordSubAction;

	if (name == "frm")
		return Token::KeywordFrame;

	if (name == "classes")
		return Token::KeywordActionClass;

	if (name == "ctx")
		return Token::KeywordCtx;

	if (name == "can")
		return Token::KeywordCan;

	if (name == "true")
		return Token::KeywordTrue;

	if (name == "actions")
		return Token::KeywordActions;

	if (name == "import")
		return Token::KeywordImport;

	if (name == "false")
		return Token::KeywordFalse;

	if (name == "return")
		return Token::KeywordReturn;

	if (name == "system")
		return Token::KeywordSystem;

	if (name == "assert")
		return Token::KeywordAssert;

	if (name == "break")
		return Token::KeywordBreak;

	if (name == "continue")
		return Token::KeywordContinue;

	if (name == "trait")
		return Token::KeywordTrait;

	if (name == "while")
		return Token::KeywordWhile;

	if (name == "of")
		return Token::KeywordOf;

	if (name == "for")
		return Token::KeywordFor;

	if (name == "in")
		return Token::KeywordIn;

	if (name == "enable")
		return Token::KeywordEnable;

	if (name == "ext")
		return Token::KeywordExtern;

	if (name == "and")
		return Token::KeywordAnd;

	if (name == "or")
		return Token::KeywordOr;

	if (name == "let")
		return Token::KeywordLet;

	if (name == "fun")
		return Token::KeywordFun;

	if (name == "req")
		return Token::KeywordReq;

	if (name == "type")
		return Token::KeywordType;

	if (name == "__builtin_malloc_do_not_use")
		return Token::KeywordMalloc;

	if (name == "__builtin_destroy_do_not_use")
		return Token::KeywordDestroy;

	if (name == "__builtin_construct_do_not_use")
		return Token::KeywordContstruct;

	if (name == "__builtin_free_do_not_use")
		return Token::KeywordFree;

	if (name == "__builtin_from_array")
		return Token::KeywordFromArray;

	if (name == "__builtin_to_array")
		return Token::KeywordToArray;

	if (name == "OwningPtr")
		return Token::KeywordOwningPtr;

	if (name == "__builtin_mangled_name")
		return Token::KeywordMangledName;

	if (name == "__builtin_as_ptr_do_not_use")
		return Token::KeywordAsPtr;

	lIdent = name;
	return Token::Identifier;
}

Token Lexer::next()
{
	if (nestedParentesys == 300)
	{
		return Token::Error;
	}
	lComment.clear();
	auto result = nextWithoutTrailingConsume();

	// unless we are at the start of line, we skip over the next white space
	// characters, so that they don't show up in the locations
	if (not newLine or nestedParentesys != 0)
		consumeWhiteSpaceUntilNextMeaningfullChar();
	return result;
}

// returns true if there was indeed a comment at the current character
bool Lexer::eatComment()
{
	if (*in == '#')
	{
		parsingComment = true;
		eatChar();
		bool doubleComment = *in == '#';
		while (*in != '\n' and *in != '\0')
			lComment += eatChar();
		lComment += "\n";
		if (doubleComment)
			lComment.clear();
		parsingComment = false;
		return true;
	}
	return false;
}

Token Lexer::eatString()
{
	lString = "";
	eatChar();
	if (*in == '\0')
		return Token::Error;
	nestedParentesys++;
	parsingString = true;
	while (*in != '\"' and *in != '\0')
	{
		lString += eatCharLiteral();
	}
	parsingString = false;
	if (*in == '\0')
		return Token::Error;
	eatChar();
	nestedParentesys--;
	return Token::String;
}

char Lexer::eatCharLiteral()
{
	char toReturn;
	if (*in == '\\')
	{
		eatChar();
		switch (*in)
		{
			case '\0':
				toReturn = '\\';
				return toReturn;
			case '0':
				toReturn = '\0';
				break;
			case 'n':
				toReturn = '\n';
				break;
			case '\\':
				toReturn = '\\';
				break;
			case '\t':
				toReturn = '\t';
				break;
			case '\"':
				toReturn = '\"';
				break;
			case '\'':
				toReturn = '\'';
				break;
		}

		eatChar();
	}
	else
	{
		toReturn = *in;
		eatChar();
	}
	return toReturn;
}

Token Lexer::nextWithoutTrailingConsume()
{
	if (*in == '\0')
	{
		if (not emittedExtraNewLine and not newLine)
		{
			emittedExtraNewLine = true;
			return Token::Newline;
		}
		while (indentStack.size() > 1)
		{
			indentStack.pop_back();
			return Token::Deindent;
		}
		while (deindentToEmit != 0)
		{
			deindentToEmit--;
			return Token::Deindent;
		}
		return Token::End;
	}

	bool consumeLine = true;
	while (consumeLine)
	{
		consumeLine = false;
		if (nestedParentesys == 0)
		{
			auto t = eatSpaces();
			if (t.has_value())
				return t.value();

			if (deindentToEmit != 0)
			{
				deindentToEmit--;
				return Token::Deindent;
			}
		}
		else
			while (isspace(*in) or *in == '\n')
				eatChar();

		consumeLine = eatComment();
	}

	startToken();
	if (isdigit(*in) != 0)
		return eatNumber();

	if (*in == '\"')
		return eatString();

	if (*in == '\'')
	{
		parsingString = true;
		eatChar();
		lInt64 = eatCharLiteral();
		if (*in != '\'')
			return Token::Error;
		eatChar();
		parsingString = false;
		return Token::Character;
	}

	if (auto val = eatSymbol(); val.has_value())
	{
		auto c = eatChar();
		if (auto val = twoSymbols(c); val.has_value())
		{
			eatChar();
			return val.value();
		}

		return val.value();
	}

	if (isalpha(*in) != 0 or *in == '_')
		return eatIdent();
	return Token::Error;
}
