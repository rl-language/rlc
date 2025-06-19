/*
Copyright 2025 Massimo Fioravanti

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
#include <set>

#include "llvm/ADT/StringExtras.h"
#include "mlir/IR/BuiltinTypes.h"
#include "mlir/IR/IRMapping.h"
#include "mlir/IR/OpDefinition.h"
#include "rlc/dialect/ActionLiveness.hpp"
#include "rlc/dialect/Dialect.h"
#include "rlc/dialect/Interfaces.hpp"
#include "rlc/dialect/Operations.hpp"
#include "rlc/utils/IRange.hpp"

namespace
{
	void replace_all(
			std::string& s,
			std::string const& toReplace,
			std::string const& replaceWith)
	{
		std::string buf;
		std::size_t pos = 0;
		std::size_t prevPos;

		// Reserves rough estimate of final size of string.
		buf.reserve(s.size());

		while (true)
		{
			prevPos = pos;
			pos = s.find(toReplace, pos);
			if (pos == std::string::npos)
				break;
			buf.append(s, prevPos, pos - prevPos);
			buf += replaceWith;
			pos += toReplace.size();
		}

		buf.append(s, prevPos, s.size() - prevPos);
		s.swap(buf);
	}
	void serializeType(
			mlir::Type type,
			llvm::raw_ostream& OS,
			mlir::rlc::SerializationContext& ctx)
	{
		if (auto casted = type.dyn_cast<mlir::rlc::TemplateParameterType>())
		{
			if (casted.getName().starts_with("TraitType"))
				OS << casted.getName().drop_front(9);
			else
				OS << casted.getName();
			return;
		}
		if (auto casted = type.dyn_cast<mlir::rlc::RLCSerializable>())
		{
			casted.rlc_serialize(OS, ctx);
			return;
		}
		type.dump();
		abort();
	}

	void serializeTypeDecl(
			mlir::Type type,
			llvm::raw_ostream& OS,
			mlir::rlc::SerializationContext& ctx)
	{
		if (auto casted = type.dyn_cast<mlir::rlc::TemplateParameterType>())
		{
			if (casted.getIsIntLiteral())
				OS << "Int ";
			if (casted.getTrait() != nullptr)
			{
				serializeType(casted.getTrait(), OS, ctx);
				OS << " ";
			}
		}

		serializeType(type, OS, ctx);
	}

	bool areConsecutive(mlir::Location l, mlir::Location r)
	{
		auto first = mlir::dyn_cast<mlir::FileLineColLoc>(l);
		auto second = mlir::dyn_cast<mlir::FileLineColLoc>(r);
		if (not first or not second)
			return false;
		return first.getLine() + 1 == second.getLine() or
					 first.getLine() == second.getLine();
	}

	mlir::Location getEndLocation(mlir::Operation* op)
	{
		if (auto casted = mlir::dyn_cast<mlir::rlc::RangeBased>(op))
			return casted.getEndLocation();
		return op->getLoc();
	}

	void serializeBlock(
			mlir::Block& block,
			llvm::raw_ostream& OS,
			mlir::rlc::SerializationContext& ctx,
			bool separateObjects)
	{
		for (auto& op : block.getOperations())
		{
			if (mlir::isa<mlir::rlc::Yield>(op))
				continue;
			if (mlir::isa<mlir::rlc::TemplateInstantiationOp>(op))
				continue;
			if (mlir::isa<mlir::rlc::IsOp>(op))
				continue;
			if (mlir::isa<mlir::rlc::StorageCast>(op))
				continue;
			if (mlir::isa<mlir::rlc::ValueUpcastOp>(op))
				continue;
			if (op.hasAttr("synthetic"))
				continue;
			auto casted = mlir::dyn_cast<mlir::rlc::Serializable>(op);
			if (not casted)
			{
				op.dump();
				op.getLoc().dump();
				abort();
			}
			casted.serialize(OS, ctx);
			if (not separateObjects)
				continue;
			if (op.getNextNode() == nullptr)
				continue;
			if (mlir::isa<mlir::rlc::Yield>(op.getNextNode()))
				continue;
			if (not areConsecutive(
							getEndLocation(casted), op.getNextNode()->getLoc()))
				OS << "\n";
		}
	}

	void serializeExpression(
			mlir::Operation* op,
			llvm::raw_ostream& OS,
			mlir::rlc::SerializationContext& ctx)
	{
		auto casted = mlir::dyn_cast<mlir::rlc::Serializable>(op);
		if (not casted)
		{
			op->dump();
			abort();
		}
		casted.serialize(OS, ctx);
	}
	void serializeExpression(
			mlir::Value value,
			llvm::raw_ostream& OS,
			mlir::rlc::SerializationContext& ctx)
	{
		// if is a block argument
		if (value.getDefiningOp() == nullptr)
		{
			auto blockArguments = value.getParentBlock()->getArguments();
			auto argIndex = std::distance(
					blockArguments.begin(), llvm::find(blockArguments, value));

			auto* op = value.getParentRegion()->getParentOp();
			if (auto casted = mlir::dyn_cast<mlir::rlc::FunctionOp>(op))
				OS << casted.getInfo().getArguments()[argIndex].getName();
			else if (auto casted = mlir::dyn_cast<mlir::rlc::ActionFunction>(op))
				OS << casted.getInfo().getArguments()[argIndex].getName();
			else if (auto casted = mlir::dyn_cast<mlir::rlc::ForFieldStatement>(op))
				OS << mlir::dyn_cast<mlir::StringAttr>(casted.getNames()[argIndex])
									.getValue();
			else if (auto casted = mlir::dyn_cast<mlir::rlc::ActionStatement>(op))
				OS << casted.getInfo().getArguments()[argIndex].getName();
			else
				abort();

			return;
		}
		if (auto stm = mlir::dyn_cast<mlir::rlc::DeclarationStatement>(
						value.getDefiningOp()))
		{
			OS << stm.getSymName();
			return;
		}
		if (auto stmt =
						mlir::dyn_cast<mlir::rlc::TraitDefinition>(value.getDefiningOp()))
		{
			auto iter = llvm::find(stmt.getResults(), value);
			auto functionName =
					stmt.getMetaType().getRequestedFunctionNames()[std::distance(
							stmt.getResult().begin(), iter)];
			OS << mlir::cast<mlir::StringAttr>(functionName).getValue();
			return;
		}
		if (auto stm =
						mlir::dyn_cast<mlir::rlc::ConstantGlobalOp>(value.getDefiningOp()))
		{
			OS << stm.getName();
			return;
		}
		if (auto stm =
						mlir::dyn_cast<mlir::rlc::ForLoopVarDeclOp>(value.getDefiningOp()))
		{
			OS << stm.getName();
			return;
		}
		if (auto stm = mlir::dyn_cast<mlir::rlc::FunctionOp>(value.getDefiningOp()))
		{
			OS << stm.getUnmangledName();
			return;
		}
		if (auto stmt =
						mlir::dyn_cast<mlir::rlc::ActionStatement>(value.getDefiningOp()))
		{
			auto iter = llvm::find(stmt.getResults(), value);
			assert(iter != stmt.getResults().end());

			OS << stmt.getDeclaredNames()[std::distance(
					stmt.getResults().begin(), iter)];
			return;
		}
		if (auto acf =
						mlir::dyn_cast<mlir::rlc::ActionFunction>(value.getDefiningOp()))
		{
			if (value == acf.getResult())
			{
				OS << acf.getUnmangledName();
			}
			else if (value == acf.getIsDoneFunction())
			{
				OS << "is_done";
			}
			else
			{
				OS << mlir::cast<mlir::rlc::ActionStatement>(
									ctx.getBuilder()
											.actionFunctionValueToActionStatement(value)
											.front())
									.getName();
			}

			return;
		}
		auto casted =
				mlir::dyn_cast<mlir::rlc::Serializable>(value.getDefiningOp());
		if (not casted)
		{
			value.dump();
			value.getLoc().dump();
			llvm::outs().flush();
			abort();
		}
		casted.serialize(OS, ctx);
	}
}	 // namespace
	 //
	 //
void mlir::rlc::ValueUpcastOp::serialize(
		llvm::raw_ostream& OS, mlir::rlc::SerializationContext& ctx)
{
	serializeExpression(getInput(), OS, ctx);
}

void mlir::rlc::Comment::serialize(
		llvm::raw_ostream& OS, mlir::rlc::SerializationContext& ctx)
{
	ctx.indent(OS);
	OS << "#" << getText();
}

void mlir::rlc::BitNotOp::serialize(
		llvm::raw_ostream& OS, mlir::rlc::SerializationContext& ctx)
{
	OS << "~";
	serializeExpression(getOperand(), OS, ctx);
}

void mlir::rlc::BitOrOp::serialize(
		llvm::raw_ostream& OS, mlir::rlc::SerializationContext& ctx)
{
	serializeExpression(getLhs(), OS, ctx);
	OS << " | ";
	serializeExpression(getRhs(), OS, ctx);
}

void mlir::rlc::RightShiftOp::serialize(
		llvm::raw_ostream& OS, mlir::rlc::SerializationContext& ctx)
{
	serializeExpression(getLhs(), OS, ctx);
	OS << " >> ";
	serializeExpression(getRhs(), OS, ctx);
}

void mlir::rlc::LeftShiftOp::serialize(
		llvm::raw_ostream& OS, mlir::rlc::SerializationContext& ctx)
{
	serializeExpression(getLhs(), OS, ctx);
	OS << " << ";
	serializeExpression(getRhs(), OS, ctx);
}

void mlir::rlc::BitXorOp::serialize(
		llvm::raw_ostream& OS, mlir::rlc::SerializationContext& ctx)
{
	serializeExpression(getLhs(), OS, ctx);
	OS << " ^ ";
	serializeExpression(getRhs(), OS, ctx);
}

void mlir::rlc::BitAndOp::serialize(
		llvm::raw_ostream& OS, mlir::rlc::SerializationContext& ctx)
{
	serializeExpression(getLhs(), OS, ctx);
	OS << " & ";
	serializeExpression(getRhs(), OS, ctx);
}

void mlir::rlc::StringLiteralOp::serialize(
		llvm::raw_ostream& OS, mlir::rlc::SerializationContext& ctx)
{
	auto str = getValue().str();
	replace_all(str, "\"", "\\\"");
	OS << "\"" << str << "\"";
}

void mlir::rlc::AsByteArrayOp::serialize(
		llvm::raw_ostream& OS, mlir::rlc::SerializationContext& ctx)
{
	OS << "__builtin_to_array(";
	serializeExpression(getLhs(), OS, ctx);
	OS << ")";
}

void mlir::rlc::BracketsOp::serialize(
		llvm::raw_ostream& OS, mlir::rlc::SerializationContext& ctx)
{
	OS << "(";
	serializeExpression(getInput(), OS, ctx);
	OS << ")";
}

void mlir::rlc::BuiltinMangledNameOp::serialize(
		llvm::raw_ostream& OS, mlir::rlc::SerializationContext& ctx)
{
	OS << "__builtin_mangled_name(";
	serializeExpression(getValue(), OS, ctx);
	OS << ")";
}

void mlir::rlc::FromByteArrayOp::serialize(
		llvm::raw_ostream& OS, mlir::rlc::SerializationContext& ctx)
{
	OS << "__builtin_from_array";
	OS << "<";
	serializeType(getType(), OS, ctx);
	OS << ">(";
	serializeExpression(getLhs(), OS, ctx);
	OS << ")";
}

void mlir::rlc::IsAlternativeTypeOp::serialize(
		llvm::raw_ostream& OS, mlir::rlc::SerializationContext& ctx)
{
	serializeExpression(getInput(), OS, ctx);
	OS << " is Alternative";
}

void mlir::rlc::EnumFieldDeclarationOp::serialize(
		llvm::raw_ostream& OS, mlir::rlc::SerializationContext& ctx)
{
	ctx.indent(OS);
	OS << getName() << "\n";
}

void mlir::rlc::EnumDeclarationOp::serialize(
		llvm::raw_ostream& OS, mlir::rlc::SerializationContext& ctx)
{
	OS << "enum " << getName() << ":\n";
	auto _ = ctx.increaseIndent();

	serializeBlock(getBody().front(), OS, ctx, true);
}

void mlir::rlc::TraitDefinition::serialize(
		llvm::raw_ostream& OS, mlir::rlc::SerializationContext& ctx)
{
	// trait template names are prefixed with TraitType + $TraitName to make them
	// unique, we have to dtop that here
	OS << "trait<";
	for (auto tArg : llvm::drop_end(getMetaType().getTemplateParameters()))
	{
		serializeTypeDecl(tArg, OS, ctx);
		OS << ", ";
	}

	serializeTypeDecl(getMetaType().getTemplateParameters().back(), OS, ctx);
	OS << "> " << getMetaType().getName() << ":";
	auto indent = ctx.increaseIndent();
	OS << "\n";

	for (auto [type, name, argNames] : llvm::zip(
					 getMetaType().getRequestedFunctionTypes(),
					 getMetaType().getRequestedFunctionNames(),
					 getMetaType().getRequestedFunctionArgNames()))
	{
		ctx.indent(OS);
		OS << "fun " << name.getValue() << "(";
		size_t i = 0;
		for (auto [argType, argName] : llvm::zip(type.getInputs(), argNames))
		{
			serializeType(argType, OS, ctx);
			OS << " " << mlir::cast<mlir::StringAttr>(argName).getValue();
			if (++i != argNames.size())
				OS << ", ";
		}
		OS << ")";

		if (returnsVoid(type).failed())
		{
			OS << " -> ";
			serializeType(type.getResult(0), OS, ctx);
		}

		OS << "\n";
	}
}

void mlir::rlc::CallOp::serialize(
		llvm::raw_ostream& OS, mlir::rlc::SerializationContext& ctx)
{
	if (getOperation()->hasAttr("post_fix_call"))
	{
		serializeExpression(getArgs()[0], OS, ctx);
		serializeExpression(getCallee(), OS, ctx);
		return;
	}
	if (getOperation()->hasAttr("infix_syntax"))
	{
		if (getArgs().size() == 2)
		{
			serializeExpression(getArgs()[0], OS, ctx);
		}
		OS << mlir::cast<mlir::StringAttr>(getOperation()->getAttr("infix_syntax"))
							.getValue();
		serializeExpression(getArgs()[1], OS, ctx);
		return;
	}
	auto callee = getCallee();
	if (auto casted =
					mlir::dyn_cast_or_null<mlir::rlc::CanOp>(getCallee().getDefiningOp()))
	{
		callee = casted.getCallee();
		OS << "can ";
	}
	if (getIsMemberCall())
	{
		serializeExpression(getArgs().front(), OS, ctx);

		OS << ".";
	}

	serializeExpression(callee, OS, ctx);
	OS << "(";
	size_t i = getIsMemberCall();
	for (auto arg : llvm::drop_begin(getArgs(), getIsMemberCall()))
	{
		serializeExpression(arg, OS, ctx);
		if (i + 1 != getArgs().size())
			OS << ", ";
		i++;
	}
	OS << ")";
}

void mlir::rlc::MemberAccess::serialize(
		llvm::raw_ostream& OS, mlir::rlc::SerializationContext& ctx)
{
	serializeExpression(getValue(), OS, ctx);
	auto type = mlir::cast<mlir::rlc::ClassType>(getValue().getType());
	auto memberName = type.getMemberNames()[getMemberIndex()];
	OS << "." << memberName;
}

void mlir::rlc::ImplicitAssignOp::serialize(
		llvm::raw_ostream& OS, mlir::rlc::SerializationContext& ctx)
{
	serializeExpression(getLhs(), OS, ctx);
	OS << " = ";
	serializeExpression(getRhs(), OS, ctx);
}

void mlir::rlc::Constant::serialize(
		llvm::raw_ostream& OS, mlir::rlc::SerializationContext& ctx)
{
	if (not getConstantAsWritten().empty())
	{
		OS << getConstantAsWritten();
		return;
	}
	if (auto casted = mlir::dyn_cast<mlir::IntegerAttr>(getValue()))
	{
		auto typeCasted = mlir::cast<mlir::IntegerType>(casted.getType());
		if (typeCasted.getWidth() != 1)
		{
			OS << casted.getValue();
		}
		else
		{
			OS << (casted.getValue() == 0 ? "false" : "true");
		}
	}
	else if (auto casted = mlir::dyn_cast<mlir::FloatAttr>(getValue()))
		OS << casted.getValue();
	else
	{
		abort();
	}
}
void mlir::rlc::SubOp::serialize(
		llvm::raw_ostream& OS, mlir::rlc::SerializationContext& ctx)
{
	serializeExpression(getLhs(), OS, ctx);
	OS << " - ";
	serializeExpression(getRhs(), OS, ctx);
}

void mlir::rlc::ClassDeclaration::serialize(
		llvm::raw_ostream& OS, mlir::rlc::SerializationContext& ctx)
{
	auto type = getResult().getType().cast<mlir::rlc::ClassType>();
	OS << "cls";
	if (not type.getExplicitTemplateParameters().empty())
	{
		OS << "<";
		for (auto parameter : llvm::drop_end(type.getExplicitTemplateParameters()))
		{
			serializeTypeDecl(parameter.cast<mlir::rlc::RLCSerializable>(), OS, ctx);
			OS << ", ";
		}
		serializeTypeDecl(
				type.getExplicitTemplateParameters()
						.back()
						.cast<mlir::rlc::RLCSerializable>(),
				OS,
				ctx);

		OS << ">";
	}

	OS << " " << type.getName() << ":\n";
	for (auto field : type.getMembers())
	{
		(OS).indent(2);
		field.getType().cast<mlir::rlc::RLCSerializable>().rlc_serialize(OS, ctx);
		OS << " " << field.getName() << "\n";
	}

	auto _ = ctx.increaseIndent();
	if (not getBody().front().empty())
	{
		if (not getMemberFields().empty())
			OS << "\n";
		serializeBlock(getBody().front(), OS, ctx, true);
	}
}

void mlir::rlc::CastOp::serialize(
		llvm::raw_ostream& OS, mlir::rlc::SerializationContext& ctx)
{
	// casts to builtin types are writte as the original type in lowercase
	llvm::printLowerCase(prettyType(getType()), OS);
	OS << "(";
	serializeExpression(getLhs(), OS, ctx);
	OS << ")";
}

void mlir::rlc::DeclarationStatement::serialize(
		llvm::raw_ostream& OS, mlir::rlc::SerializationContext& ctx)
{
	ctx.indent(OS);
	auto terminator = mlir::cast<mlir::rlc::Yield>(getBody().front().back());

	// if the var decl immediatelly retruns a construct, it was in the form let
	// var : Type
	auto construct = mlir::dyn_cast<mlir::rlc::ConstructOp>(
			terminator.getArguments()[0].getDefiningOp());
	// if the construct is used in a implict assign, the right hand of the assign
	// is the real guy to serialize
	mlir::rlc::ImplicitAssignOp assign = nullptr;
	if (construct)
		for (auto user : construct.getResult().getUsers())
			if (auto casted = mlir::dyn_cast<mlir::rlc::ImplicitAssignOp>(user))
				assign = casted;

	assert(terminator.getArguments().size() == 1);
	if (mlir::isa<mlir::rlc::FrameType>(getType()))
	{
		OS << "frm ";
	}
	else if (isReference())
	{
		OS << "ref ";
	}
	else
	{
		OS << "let ";
	}
	OS << getSymName();
	if (assign != nullptr)
	{
		OS << " = ";
		serializeExpression(assign.getRhs(), OS, ctx);
	}
	else if (construct == nullptr)
	{
		OS << " = ";
		serializeExpression(terminator.getArguments().front(), OS, ctx);
	}
	else
	{
		OS << " : ";
		serializeType(construct.getShugarizedType().value().getType(), OS, ctx);
	}
	OS << "\n";
}

void mlir::rlc::ArrayAccess::serialize(
		llvm::raw_ostream& OS, mlir::rlc::SerializationContext& ctx)
{
	serializeExpression(getValue(), OS, ctx);
	OS << "[";
	serializeExpression(getMemberIndex(), OS, ctx);
	OS << "]";
}

void mlir::rlc::DestroyOp::serialize(
		llvm::raw_ostream& OS, mlir::rlc::SerializationContext& ctx)
{
	OS << "__builtin_destroy_do_not_use";
	OS << "(";
	serializeExpression(getValue(), OS, ctx);
	OS << ")";
}

void mlir::rlc::InplaceInitializeOp::serialize(
		llvm::raw_ostream& OS, mlir::rlc::SerializationContext& ctx)
{
	OS << "__builtin_construct_do_not_use";
	OS << "(";
	serializeExpression(getValue(), OS, ctx);
	OS << ")";
}

void mlir::rlc::IntegerLiteralUse::serialize(
		llvm::raw_ostream& OS, mlir::rlc::SerializationContext& ctx)
{
	serializeType(getInputType(), OS, ctx);
}

void mlir::rlc::AssertOp::serialize(
		llvm::raw_ostream& OS, mlir::rlc::SerializationContext& ctx)
{
	OS << "assert(";
	serializeExpression(getAssertion(), OS, ctx);
	OS << ", " << getMessage();
	OS << ")";
}

void mlir::rlc::TemplateInstantiationOp::serialize(
		llvm::raw_ostream& OS, mlir::rlc::SerializationContext& ctx)
{
	serializeExpression(getInputTemplate(), OS, ctx);
}

void mlir::rlc::FreeOp::serialize(
		llvm::raw_ostream& OS, mlir::rlc::SerializationContext& ctx)
{
	OS << "__builtin_free_do_not_use(";
	serializeExpression(getArgument(), OS, ctx);
	OS << ")";
}

void mlir::rlc::MallocOp::serialize(
		llvm::raw_ostream& OS, mlir::rlc::SerializationContext& ctx)
{
	OS << "__builtin_malloc_do_not_use<";
	serializeType(getShugarizedType().getType(), OS, ctx);
	OS << ">(";
	serializeExpression(getSize(), OS, ctx);
	OS << ")";
}
void mlir::rlc::ReturnStatement::serialize(
		llvm::raw_ostream& OS, mlir::rlc::SerializationContext& ctx)
{
	ctx.indent(OS);

	OS << "return";
	auto terminator = mlir::cast<mlir::rlc::Yield>(getBody().front().back());
	if (terminator.getArguments().size() == 0)
	{
		OS << "\n";
		return;
	}
	OS << " ";
	assert(terminator.getArguments().size() == 1);
	auto toReturn = terminator.getArguments().front();

	// if there is a construct it means that we have generated a copy out
	// out of a return statement. in that case there is a assigment operation
	// as other use the return value
	if (auto construct =
					mlir::dyn_cast<mlir::rlc::ConstructOp>(toReturn.getDefiningOp()))
	{
		auto iter = llvm::find_if(
				construct.getResult().getUsers(), [](mlir::Operation* op) {
					return mlir::isa<mlir::rlc::ImplicitAssignOp>(op);
				});
		auto original =
				mlir::cast<mlir::rlc::ImplicitAssignOp>(iter.getCurrent().getUser())
						.getRhs();
		serializeExpression(original, OS, ctx);
	}
	else
		serializeExpression(toReturn, OS, ctx);
	OS << "\n";
}

void mlir::rlc::EqualOp::serialize(
		llvm::raw_ostream& OS, mlir::rlc::SerializationContext& ctx)
{
	serializeExpression(getLhs(), OS, ctx);
	OS << " == ";
	serializeExpression(getRhs(), OS, ctx);
}

void mlir::rlc::NotEqualOp::serialize(
		llvm::raw_ostream& OS, mlir::rlc::SerializationContext& ctx)
{
	serializeExpression(getLhs(), OS, ctx);
	OS << " != ";
	serializeExpression(getRhs(), OS, ctx);
}

void mlir::rlc::GreaterEqualOp::serialize(
		llvm::raw_ostream& OS, mlir::rlc::SerializationContext& ctx)
{
	serializeExpression(getLhs(), OS, ctx);
	OS << " >= ";
	serializeExpression(getRhs(), OS, ctx);
}

void mlir::rlc::GreaterOp::serialize(
		llvm::raw_ostream& OS, mlir::rlc::SerializationContext& ctx)
{
	serializeExpression(getLhs(), OS, ctx);
	OS << " > ";
	serializeExpression(getRhs(), OS, ctx);
}

void mlir::rlc::MinusOp::serialize(
		llvm::raw_ostream& OS, mlir::rlc::SerializationContext& ctx)
{
	OS << "-";
	serializeExpression(getLhs(), OS, ctx);
}

void mlir::rlc::LessEqualOp::serialize(
		llvm::raw_ostream& OS, mlir::rlc::SerializationContext& ctx)
{
	serializeExpression(getLhs(), OS, ctx);
	OS << " <= ";
	serializeExpression(getRhs(), OS, ctx);
}

void mlir::rlc::LessOp::serialize(
		llvm::raw_ostream& OS, mlir::rlc::SerializationContext& ctx)
{
	serializeExpression(getLhs(), OS, ctx);
	OS << " < ";
	serializeExpression(getRhs(), OS, ctx);
}

void mlir::rlc::EnumUse::serialize(
		llvm::raw_ostream& OS, mlir::rlc::SerializationContext& ctx)
{
	serializeType(getResult().getType(), OS, ctx);
	OS << "::" << getUserWrittenEnum();
}

void mlir::rlc::ReminderOp::serialize(
		llvm::raw_ostream& OS, mlir::rlc::SerializationContext& ctx)
{
	serializeExpression(getLhs(), OS, ctx);
	OS << " % ";
	serializeExpression(getRhs(), OS, ctx);
}

void mlir::rlc::DivOp::serialize(
		llvm::raw_ostream& OS, mlir::rlc::SerializationContext& ctx)
{
	serializeExpression(getLhs(), OS, ctx);
	OS << " / ";
	serializeExpression(getRhs(), OS, ctx);
}

void mlir::rlc::MultOp::serialize(
		llvm::raw_ostream& OS, mlir::rlc::SerializationContext& ctx)
{
	serializeExpression(getLhs(), OS, ctx);
	OS << " * ";
	serializeExpression(getRhs(), OS, ctx);
}

void mlir::rlc::AddOp::serialize(
		llvm::raw_ostream& OS, mlir::rlc::SerializationContext& ctx)
{
	serializeExpression(getLhs(), OS, ctx);
	OS << " + ";
	serializeExpression(getRhs(), OS, ctx);
}

void mlir::rlc::ContinueStatement::serialize(
		llvm::raw_ostream& OS, mlir::rlc::SerializationContext& ctx)
{
	ctx.indent(OS);

	OS << "continue\n";
}
void mlir::rlc::BreakStatement::serialize(
		llvm::raw_ostream& OS, mlir::rlc::SerializationContext& ctx)
{
	ctx.indent(OS);

	OS << "break\n";
}

void mlir::rlc::ForLoopVarDeclOp::serialize(
		llvm::raw_ostream& OS, mlir::rlc::SerializationContext& ctx)
{
}

void mlir::rlc::ForFieldStatement::serialize(
		llvm::raw_ostream& OS, mlir::rlc::SerializationContext& ctx)
{
	ctx.indent(OS);
	OS << "for ";
	for (auto name : llvm::drop_end(getNames()))
	{
		OS << mlir::cast<mlir::StringAttr>(name).getValue() << ", ";
	}
	OS << mlir::cast<mlir::StringAttr>(getNames()[getNames().size() - 1])
						.getValue()
		 << " of ";

	auto terminator = mlir::cast<mlir::rlc::Yield>(getCondition().front().back());
	serializeExpression(terminator.getArguments()[0], OS, ctx);
	OS << ":\n";
	{
		auto _ = ctx.increaseIndent();
		serializeBlock(getBody().front(), OS, ctx, true);
	}
}

void mlir::rlc::ForLoopStatement::serialize(
		llvm::raw_ostream& OS, mlir::rlc::SerializationContext& ctx)
{
	ctx.indent(OS);
	OS << "for ";
	auto var = *getBody().getOps<mlir::rlc::ForLoopVarDeclOp>().begin();
	OS << var.getName() << " in ";

	auto terminator =
			mlir::cast<mlir::rlc::Yield>(getRangeExpression().front().back());
	serializeExpression(terminator.getArguments()[0], OS, ctx);
	OS << ":\n";
	{
		auto _ = ctx.increaseIndent();
		serializeBlock(getBody().front(), OS, ctx, true);
	}
}

void mlir::rlc::WhileStatement::serialize(
		llvm::raw_ostream& OS, mlir::rlc::SerializationContext& ctx)
{
	ctx.indent(OS);
	OS << "while ";
	auto terminator = mlir::cast<mlir::rlc::Yield>(getCondition().front().back());
	serializeExpression(terminator.getArguments()[0], OS, ctx);
	OS << ":\n";
	{
		auto _ = ctx.increaseIndent();
		serializeBlock(getBody().front(), OS, ctx, true);
	}
}

mlir::Location mlir::rlc::ActionStatement::getEndLocation()
{
	return getPrecondition().front().getTerminator()->getLoc();
}

mlir::Location mlir::rlc::ForFieldStatement::getEndLocation()
{
	return getBody().front().getTerminator()->getLoc();
}

mlir::Location mlir::rlc::IfStatement::getEndLocation()
{
	return getElseBranch().front().getTerminator()->getLoc();
}

mlir::Location mlir::rlc::WhileStatement::getEndLocation()
{
	return getBody().front().getTerminator()->getLoc();
}

void mlir::rlc::InitializerListOp::serialize(
		llvm::raw_ostream& OS, mlir::rlc::SerializationContext& ctx)
{
	OS << "[";
	auto yield = mlir::cast<mlir::rlc::Yield>(getBody().front().getTerminator());
	size_t i = 0;
	for (auto entry : yield.getArguments())
	{
		serializeExpression(entry, OS, ctx);
		if (++i != yield.getArguments().size())
			OS << ", ";
	}
	OS << "]";
}

void mlir::rlc::IfStatement::serialize(
		llvm::raw_ostream& OS, mlir::rlc::SerializationContext& ctx)
{
	// if we are the first operation in the else block of our parent if
	// instruction we can emit the `else if` syntax
	if (auto castedParent =
					mlir::dyn_cast<mlir::rlc::IfStatement>(getOperation()->getParentOp());
			not castedParent or
			&castedParent.getElseBranch().front().front() != getOperation())
		ctx.indent(OS);
	OS << "if ";
	auto terminator = mlir::cast<mlir::rlc::Yield>(getCondition().front().back());
	serializeExpression(terminator.getArguments()[0], OS, ctx);
	OS << ":\n";
	{
		auto _ = ctx.increaseIndent();
		serializeBlock(getTrueBranch().front(), OS, ctx, true);
	}
	if (getElseBranch().front().getOperations().size() == 1 and
			mlir::isa<mlir::rlc::Yield>(getElseBranch().front().getTerminator()))
		return;
	ctx.indent(OS);
	{
		if (mlir::isa<mlir::rlc::IfStatement>(getElseBranch().front().front()))
		{
			OS << "else ";
			serializeBlock(getElseBranch().front(), OS, ctx, true);
		}
		else
		{
			OS << "else:\n";
			auto _ = ctx.increaseIndent();
			serializeBlock(getElseBranch().front(), OS, ctx, true);
		}
	}
}

void mlir::rlc::IsOp::serialize(
		llvm::raw_ostream& OS, mlir::rlc::SerializationContext& ctx)
{
	serializeExpression(getExpression(), OS, ctx);
	OS << " is ";
	if (getShugarizedType().has_value())
	{
		serializeType(getShugarizedType()->getType(), OS, ctx);
	}
	else
		serializeType(getTypeOrTrait(), OS, ctx);
}

void mlir::rlc::NotOp::serialize(
		llvm::raw_ostream& OS, mlir::rlc::SerializationContext& ctx)
{
	OS << "!";
	serializeExpression(getLhs(), OS, ctx);
}

void mlir::rlc::OrOp::serialize(
		llvm::raw_ostream& OS, mlir::rlc::SerializationContext& ctx)
{
	serializeExpression(getLhs(), OS, ctx);
	OS << " or ";
	serializeExpression(getRhs(), OS, ctx);
}

void mlir::rlc::AndOp::serialize(
		llvm::raw_ostream& OS, mlir::rlc::SerializationContext& ctx)
{
	serializeExpression(getLhs(), OS, ctx);
	OS << " and ";
	serializeExpression(getRhs(), OS, ctx);
}

void mlir::rlc::CanOp::serialize(
		llvm::raw_ostream& OS, mlir::rlc::SerializationContext& ctx)
{
	OS << "can ";
	serializeExpression(getCallee(), OS, ctx);
}

void mlir::rlc::TypeAliasOp::serialize(
		llvm::raw_ostream& OS, mlir::rlc::SerializationContext& ctx)
{
	ctx.indent(OS);
	OS << "using " << getName() << " = ";
	serializeType(getShugarizedType().value().getType(), OS, ctx);
	OS << "\n";
}

void mlir::rlc::ConstantGlobalOp::serialize(
		llvm::raw_ostream& OS, mlir::rlc::SerializationContext& ctx)
{
	OS << "const " << getName() << " = ";
	if (auto casted = mlir::dyn_cast<mlir::IntegerAttr>(getValues()))
	{
		OS << casted.getInt();
	}
	else
	{
		casted.dump();
		abort();
	}
	OS << "\n";
}

void mlir::rlc::StatementList::serialize(
		llvm::raw_ostream& OS, mlir::rlc::SerializationContext& ctx)
{
	auto _ = ctx.increaseIndent();
	serializeBlock(getBody().front(), OS, ctx, true);
}

void mlir::rlc::FunctionOp::serialize(
		llvm::raw_ostream& OS, mlir::rlc::SerializationContext& ctx)
{
	ctx.indent(OS);
	if (isDeclaration())
		OS << "ext ";
	OS << "fun";
	llvm::SmallVector<mlir::Type, 2> tArgs;
	for (auto tArg : getTemplateParameters())
	{
		tArgs.push_back(mlir::cast<mlir::TypeAttr>(tArg).getValue());
	}
	if (getIsMemberFunction())
	{
		auto classType =
				mlir::cast<mlir::rlc::ClassType>(getFunctionType().getInput(0));
		for (auto tArg : classType.getExplicitTemplateParameters())
			tArgs.erase(tArgs.begin());
	}
	if (not tArgs.empty())
	{
		OS << "<";
		for (auto arg : llvm::drop_end(tArgs))
		{
			serializeTypeDecl(arg, OS, ctx);
			OS << ", ";
		}
		serializeTypeDecl(tArgs[tArgs.size() - 1], OS, ctx);
		OS << ">";
	}
	OS << " " << getUnmangledName() << "(";
	size_t current = 0;
	for (const auto& [name, type] :
			 llvm::zip(getArgNames(), getType().getInputs()))
	{
		auto iteration = current++;
		if (iteration == 0 and getIsMemberFunction())
			continue;

		serializeType(type, OS, ctx);
		OS << " " << name;
		if (iteration + 1 != getArgNames().size())
		{
			OS << ", ";
		}
	}
	OS << ")";
	if (returnsVoid(getType()).failed())
	{
		OS << " -> ";

		serializeType(getType().getResult(0), OS, ctx);
	}
	if (not getPrecondition().empty())
	{
		auto terminator =
				mlir::cast<mlir::rlc::Yield>(getPrecondition().front().back());
		if (not terminator.getArguments().empty())
		{
			assert(terminator.getArguments().size() == 1);
			OS << " { ";

			serializeExpression(terminator.getArguments()[0], OS, ctx);
			OS << " }";
		}
	}
	if (isDeclaration())
	{
		OS << "\n";
		return;
	}
	OS << ":\n";

	auto _ = ctx.increaseIndent();
	serializeBlock(getBody().front(), OS, ctx, true);
}

void mlir::rlc::Import::serialize(
		llvm::raw_ostream& OS, mlir::rlc::SerializationContext& ctx)
{
	ctx.indent(OS);
	OS << "import " << getText() << "\n";
}

void mlir::rlc::ExpressionStatement::serialize(
		llvm::raw_ostream& OS, mlir::rlc::SerializationContext& ctx)
{
	ctx.indent(OS);
	auto terminator = mlir::cast<mlir::rlc::Yield>(getBody().front().back());
	auto* toWrite = terminator->getPrevNode();

	serializeExpression(toWrite, OS, ctx);
	OS << "\n";
}

void mlir::rlc::StorageCast::serialize(
		llvm::raw_ostream& OS, mlir::rlc::SerializationContext& ctx)
{
	serializeExpression(getOperand(), OS, ctx);
}

void mlir::rlc::ActionStatement::serialize(
		llvm::raw_ostream& OS, mlir::rlc::SerializationContext& ctx)
{
	ctx.indent(OS);
	OS << "act " << getName() << "(";
	size_t current = 0;
	for (const auto& [name, type] :
			 llvm::zip(getDeclaredNames(), getResultTypes()))
	{
		// skip context types obtained from the action function signature
		auto iteration = current++;
		if (mlir::isa<mlir::rlc::ContextType>(type))
		{
			continue;
		}

		serializeType(type, OS, ctx);
		OS << " " << name;
		if (iteration + 1 != getResultTypes().size())
		{
			OS << ", ";
		}
	}
	OS << ")";
	if (not getPrecondition().empty())
	{
		auto terminator =
				mlir::cast<mlir::rlc::Yield>(getPrecondition().front().back());
		if (not terminator.getArguments().empty())
		{
			OS << " { ";

			serializeExpression(terminator.getArguments()[0], OS, ctx);
			OS << " }";
		}
	}
	OS << "\n";
}

void mlir::rlc::ActionsStatement::serialize(
		llvm::raw_ostream& OS, mlir::rlc::SerializationContext& ctx)
{
	ctx.indent(OS);
	OS << "actions:\n";
	auto _ = ctx.increaseIndent();
	for (size_t i = 0; i != getActions().size(); i++)
	{
		auto& block = getActions()[i];
		serializeBlock(block.front(), OS, ctx, true);
		if (getActions().size() == i + 1)
			continue;
		auto& nextBlock = getActions()[i + 1];
		if (not areConsecutive(
						block.front().getTerminator()->getLoc(),
						nextBlock.front().front().getLoc()))
			OS << "\n";
	}
}

void mlir::rlc::ShortCircuitingAnd::serialize(
		llvm::raw_ostream& OS, mlir::rlc::SerializationContext& ctx)
{
	serializeExpression(
			mlir::cast<mlir::rlc::Yield>(getLhs().front().getTerminator())
					.getArguments()[0],
			OS,
			ctx);
	OS << " and ";
	serializeExpression(
			mlir::cast<mlir::rlc::Yield>(getRhs().front().getTerminator())
					.getArguments()[0],
			OS,
			ctx);
}

void mlir::rlc::UsingTypeOp::serialize(
		llvm::raw_ostream& OS, mlir::rlc::SerializationContext& ctx)
{
	ctx.indent(OS);
	OS << "using " << getName() << " = type(";
	serializeExpression(
			mlir::cast<mlir::rlc::Yield>(getBody().front().getTerminator())
					.getArguments()[0],
			OS,
			ctx);
	OS << ")\n";
}

void mlir::rlc::ShortCircuitingOr::serialize(
		llvm::raw_ostream& OS, mlir::rlc::SerializationContext& ctx)
{
	serializeExpression(
			mlir::cast<mlir::rlc::Yield>(getLhs().front().getTerminator())
					.getArguments()[0],
			OS,
			ctx);
	OS << " or ";
	serializeExpression(
			mlir::cast<mlir::rlc::Yield>(getRhs().front().getTerminator())
					.getArguments()[0],
			OS,
			ctx);
}

void mlir::rlc::SubActionInfo::serialize(
		llvm::raw_ostream& OS, mlir::rlc::SerializationContext& ctx)
{
	ctx.indent(OS);

	OS << "subaction";
	if (not getRunOnce())
		OS << "*";

	auto yield =
			mlir::cast<mlir::rlc::Yield>(getForwardedArgs().front().getTerminator());

	if (not yield.getArguments().empty())
	{
		OS << "(";
		for (auto arg : llvm::drop_end(yield.getArguments()))
		{
			serializeExpression(arg, OS, ctx);
			OS << ", ";
		}
		serializeExpression(yield.getArguments().back(), OS, ctx);
		OS << ")";
	}

	yield = mlir::cast<mlir::rlc::Yield>(getArguments().front().getTerminator());
	if (getDeclaresVar())
	{
		auto stmt = mlir::dyn_cast<mlir::rlc::DeclarationStatement>(
				yield.getArguments()[0].getDefiningOp());
		OS << " ";
		OS << stmt.getSymName();
		OS << " = ";
		serializeExpression(
				mlir::cast<mlir::rlc::Yield>(stmt.getBody().front().getTerminator())
						.getArguments()[0],
				OS,
				ctx);
		OS << "\n";
		return;
	}
	OS << " ";

	for (auto arg : llvm::drop_end(yield.getArguments()))
	{
		serializeExpression(arg, OS, ctx);
		OS << ", ";
	}
	serializeExpression(yield.getArguments().back(), OS, ctx);
	OS << "\n";
}

void mlir::rlc::ActionFunction::serialize(
		llvm::raw_ostream& OS, mlir::rlc::SerializationContext& ctx)
{
	ctx.indent(OS);
	if (getOperation()->hasAttr("emit_classes"))
		OS << "@classes\n";
	OS << "act " << getUnmangledName() << "(";
	size_t current = 0;
	for (const auto& [name, type] :
			 llvm::zip(getArgNames(), getType().getInputs()))
	{
		auto iteration = current++;

		serializeType(type, OS, ctx);
		OS << " " << name;
		if (iteration + 1 != getArgNames().size())
		{
			OS << ", ";
		}
	}
	OS << ")";
	OS << " -> ";

	serializeType(getClassType(), OS, ctx);
	if (not getPrecondition().empty())
	{
		auto terminator =
				mlir::cast<mlir::rlc::Yield>(getPrecondition().front().back());
		if (not terminator.getArguments().empty())
		{
			OS << " { ";

			serializeExpression(terminator.getArguments()[0], OS, ctx);
			OS << " }";
		}
	}
	OS << ":\n";

	auto _ = ctx.increaseIndent();
	serializeBlock(getBody().front(), OS, ctx, true);
}

void mlir::rlc::FlatFunctionOp::serialize(
		llvm::raw_ostream& OS, mlir::rlc::SerializationContext& ctx)
{
	assert(false && "not implemented");
}

llvm::StringRef mlir::rlc::AddOp::getInfixSyntax() { return " + "; }

llvm::StringRef mlir::rlc::InitOp::getInfixSyntax() { return ""; }

llvm::StringRef mlir::rlc::SubOp::getInfixSyntax() { return " - "; }

llvm::StringRef mlir::rlc::NotEqualOp::getInfixSyntax() { return " != "; }

llvm::StringRef mlir::rlc::AndOp::getInfixSyntax() { return " and "; }

llvm::StringRef mlir::rlc::OrOp::getInfixSyntax() { return " or "; }

llvm::StringRef mlir::rlc::LessEqualOp::getInfixSyntax() { return " <= "; }

llvm::StringRef mlir::rlc::GreaterEqualOp::getInfixSyntax() { return " >= "; }

llvm::StringRef mlir::rlc::LessOp::getInfixSyntax() { return " < "; }

llvm::StringRef mlir::rlc::GreaterOp::getInfixSyntax() { return " > "; }

llvm::StringRef mlir::rlc::DivOp::getInfixSyntax() { return " / "; }

llvm::StringRef mlir::rlc::NotOp::getInfixSyntax() { return "!"; }

llvm::StringRef mlir::rlc::MinusOp::getInfixSyntax() { return "-"; }

llvm::StringRef mlir::rlc::MultOp::getInfixSyntax() { return " * "; }
llvm::StringRef mlir::rlc::ReminderOp::getInfixSyntax() { return " % "; }

llvm::StringRef mlir::rlc::EqualOp::getInfixSyntax() { return " == "; }
