#include "rlc/parser/Lexer.hpp"

#include <cctype>
#include <optional>
#include <string>

#include "llvm/ADT/StringRef.h"

using namespace std;
using namespace rlc;

char Lexer::eatChar()
{
	char c = *in;
	currentColumn++;
	newLine = c == '\n';
	if (c == '\n')
	{
		currentLine++;
		currentColumn = 0;
	}
	in++;
	return c;
}

llvm::StringRef rlc::tokenToString(Token t)
{
	switch (t)
	{
		case Token::End:
			return "End";
		case Token::Begin:
			return "Begin";
		case Token::KeywordSystem:
			return "KeywordSystem";
		case Token::KeywordAnd:
			return "KeywordAnd";
		case Token::KeywordOr:
			return "KeywordOr";
		case Token::KeywordRule:
			return "KeywordRule";
		case Token::KeywordEvent:
			return "KeywordEvent";
		case Token::KeywordAction:
			return "KeywordAction";
		case Token::KeywordEntity:
			return "KeywordEntity";
		case Token::KeywordIf:
			return "KeywordIf";
		case Token::KeywordElse:
			return "KeywordElse";
		case Token::KeywordWhile:
			return "KeywordWhile";
		case Token::KeywordFor:
			return "KeywordFor";
		case Token::KeywordReturn:
			return "KeywordReturn";
		case Token::KeywordEnable:
			return "KeywordEnable";
		case Token::KeywordTrue:
			return "KeywordTrue";
		case Token::KeywordFalse:
			return "KeywordFalse";
		case Token::KeywordIn:
			return "KeywordIn";
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
			return "Colonts";
		case Token::Equal:
			return "Equal";
		case Token::Identifier:
			return "Identifier";
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
		case Token::Dot:
			return "Dot";
		case Token::Void:
			return "Void";
		case Token::Module:
			return "Module";
	}
	return "";
}

optional<Token> Lexer::eatSpaces()
{
	bool beginOfLine = newLine;
	size_t indent = 0;
	while (isspace(*in) != 0 and *in != '\n')
	{
		eatChar();
		indent++;
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

	while (isdigit(*in) != 0)
		number.push_back(eatChar());

	if (*in != '.')
	{
		lInt64 = stol(number);
		return Token::Int64;
	}

	number.push_back(eatChar());
	while (isdigit(*in) != 0)
		number.push_back(eatChar());

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
		case '\n':
			return Token::Newline;
	}
	return nullopt;
}

optional<Token> Lexer::twoSymbols(char current)
{
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
	}
	return nullopt;
}

Token Lexer::eatIdent()
{
	string name;
	name.push_back(eatChar());
	while (isspace(*in) == 0 and not eatSymbol() and *in != '\0')
		name.push_back(eatChar());

	if (name == "if")
		return Token::KeywordIf;

	if (name == "else")
		return Token::KeywordElse;

	if (name == "ent")
		return Token::KeywordEntity;

	if (name == "evn")
		return Token::KeywordEvent;

	if (name == "rul")
		return Token::KeywordRule;

	if (name == "act")
		return Token::KeywordAction;

	if (name == "true")
		return Token::KeywordTrue;

	if (name == "false")
		return Token::KeywordFalse;

	if (name == "return")
		return Token::KeywordReturn;

	if (name == "system")
		return Token::KeywordSystem;

	if (name == "while")
		return Token::KeywordWhile;

	if (name == "for")
		return Token::KeywordFor;

	if (name == "in")
		return Token::KeywordIn;

	if (name == "enable")
		return Token::KeywordEnable;

	if (name == "and")
		return Token::KeywordAnd;

	if (name == "or")
		return Token::KeywordOr;
	if (name == "void")
		return Token::Void;

	lIdent = name;
	return Token::Identifier;
}

Token Lexer::next()
{
	if (*in == '\0')
		return Token::End;

	auto t = eatSpaces();
	if (t.has_value())
		return t.value();

	if (deindentToEmit != 0)
	{
		deindentToEmit--;
		return Token::Deindent;
	}

	if (*in == '#')	 // eat comments
		while (eatChar() != '\n')
			;

	if (isdigit(*in) != 0)
		return eatNumber();

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

	if (isalpha(*in) != 0)
		return eatIdent();
	return Token::Error;
}
