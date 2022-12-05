#pragma once

#include "llvm/ADT/StringRef.h"

namespace rlc
{
	class System;
	enum class BuiltinFunctions
	{
		And,
		Or,
		Greater,
		GrearEqual,
		Less,
		LessEqual,
		Equal,
		NotEqual,
		Add,
		Subtract,
		Divide,
		Multiply,
		Reminder,
		Not,
		Int,
		Bool,
		Float,
		Assign,
		Minus,
		Init,
	};

	llvm::StringRef builtinFunctionsToString(BuiltinFunctions fun);
	BuiltinFunctions stringToBuiltinFunction(llvm::StringRef name);

	void addBuilints(System& system);
}	 // namespace rlc
