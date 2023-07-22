#include "rlc/conversions/RLCToC.hpp"

#include "llvm/ADT/TypeSwitch.h"
#include "llvm/Support/Format.h"
#include "rlc/dialect/Operations.hpp"
#include "rlc/dialect/Types.hpp"
#include "rlc/dialect/Visits.hpp"

using namespace rlc;

static std::string nonArrayTypeToString(mlir::Type type)
{
	std::string O;
	llvm::raw_string_ostream OS(O);
	llvm::TypeSwitch<mlir::Type>(type)
			.Case([&](mlir::rlc::EntityType Entity) { OS << Entity.getName(); })
			.Case<mlir::rlc::FloatType>([&](mlir::rlc::FloatType) { OS << "double"; })
			.Case<mlir::rlc::BoolType>([&](mlir::rlc::BoolType) { OS << "uint8_t"; })
			.Case<mlir::rlc::IntegerType>([&](mlir::rlc::IntegerType Type) {
				OS << "int" << Type.getSize() << "_t";
			})
			.Case<mlir::rlc::VoidType>([&](mlir::rlc::VoidType) { OS << "void"; })
			.Default([](auto type) {
				type.dump();
				llvm_unreachable(
						"while emitting c header, recieved  a unexpected type");
			});
	OS.flush();
	return O;
}

static std::string typeToString(mlir::Type type)
{
	std::string O;
	llvm::raw_string_ostream OS(O);
	llvm::TypeSwitch<mlir::Type>(type)
			.Case<mlir::rlc::ArrayType>([&](mlir::rlc::ArrayType array) {
				OS << typeToString(array.getUnderlying());
				OS << "[" << array.getSize() << "]";
			})
			.Default([&](auto type) { OS << nonArrayTypeToString(type); });
	OS.flush();
	return O;
}

static void printTypeField(
		llvm::StringRef fieldName, mlir::Type type, llvm::raw_ostream& OS)
{
	llvm::TypeSwitch<mlir::Type>(type)
			.Case<mlir::rlc::ArrayType>([&](mlir::rlc::ArrayType array) {
				printTypeField(fieldName, array.getUnderlying(), OS);
				OS << "[" << array.getSize() << "]";
			})
			.Default([&](auto type) { OS << nonArrayTypeToString(type); });

	if (not type.isa<mlir::rlc::ArrayType>())
		OS << " " << fieldName;
}

static void printTypeDefinition(mlir::Type type, llvm::raw_ostream& OS)
{
	llvm::TypeSwitch<mlir::Type>(type)
			.Case([&](mlir::rlc::EntityType Entity) {
				OS << "typedef struct {\n";

				for (const auto& [name, fieldType] :
						 llvm::zip(Entity.getFieldNames(), Entity.getBody()))
				{
					OS.indent(2);
					printTypeField(name, fieldType, OS);
					OS << ";\n";
				}

				OS << "} " << Entity.getName() << ";\n";
			})
			.Case<
					mlir::rlc::IntegerType,
					mlir::rlc::BoolType,
					mlir::rlc::FloatType,
					mlir::rlc::ArrayType>([&](auto) {	 // Pass, already defined by C
			})
			.Default([](auto type) {
				type.dump();
				llvm_unreachable(
						"while emitting c header, recieved  a unexpected type");
			});
}

static void printFunctionSignature(
		llvm::StringRef originalName,
		llvm::StringRef name,
		llvm::StringRef cShortName,
		mlir::FunctionType type,
		mlir::ArrayAttr fieldNames,
		llvm::raw_ostream& OS)
{
	OS << "#ifdef RLC_GET_FUNCTION_DECLS\n";
	if (type.getResults().empty())
		OS << "void ";
	else
		printTypeField("", type.getResults().front(), OS);
	OS << name << "(";
	for (size_t index = 0; index < fieldNames.size(); index++)
	{
		printTypeField(
				("* " + fieldNames[index].cast<mlir::StringAttr>().getValue()).str(),
				type.getInput(index),
				OS);

		if (index + 1 < fieldNames.size())
			OS << ", ";
	}
	OS << ");\n";
	OS << "#endif\n\n";

	OS << "#ifdef RLC_VISIT_FUNCTION\n";

	OS << "#define RLC_ARGUMENTS_COUNT " << type.getInputs().size() << "\n";

	OS << "#define RLC_ARGUMENTS ";
	size_t I = 0;
	for (auto [type, name] : llvm::zip(type.getInputs(), fieldNames))
	{
		I++;
		printTypeField("*", type, OS);
		OS << " " << name.cast<mlir::StringAttr>().getValue();
		if (I != fieldNames.size())
			OS << ", ";
	}
	OS << "\n";

	OS << "RLC_VISIT_FUNCTION(" << originalName << ", " << name << ", ";
	OS << cShortName;
	OS << ", ";
	if (type.getResults().empty())
		OS << "void ";
	else
		printTypeField("", type.getResults().front(), OS);
	OS << ", ";

	I = 0;
	for (auto name : fieldNames)
	{
		I++;
		OS << " " << name.cast<mlir::StringAttr>().getValue();
		if (I != fieldNames.size())
			OS << ", ";
	}
	OS << ")\n";
	OS << "#undef RLC_ARGUMENTS_COUNT\n";
	OS << "#undef RLC_ARGUMENTS\n";
	OS << "#endif\n\n";
}

void rlc::rlcToCHeader(mlir::ModuleOp Module, llvm::raw_ostream& OS)
{
	OS << "#ifndef RLC_HEADER\n";
	OS << "#ifdef RLC_C_HEADER\n#undef RLC_C_HEADER\n#define RLC_HEADER\n";
	OS << R""""(
#include "stddef.h"
#include "stdint.h"
#define RLC_GET_FUNCTION_DECLS
#define RLC_GET_TYPE_DECLS
#define RLC_VISIT_FUNCTION(name, mangled_name, cShortName, return_type, ...)   \
	static inline return_type cShortName(RLC_ARGUMENTS)                         \
	{                                                                            \
		return mangled_name(__VA_ARGS__);                                          \
	} 
#endif
)"""";
	OS << "#endif\n";

	OS << "#ifdef RLC_GET_TYPE_DECLS\n";
	for (auto type : postOrderTypes(Module))
		printTypeDefinition(type, OS);
	OS << "#undef RLC_GET_TYPE_DECLS\n";
	OS << "#endif\n\n";

	OS << "#ifdef RLC_TYPE\n";
	for (auto type : postOrderTypes(Module))
		if (auto casted = type.dyn_cast<mlir::rlc::EntityType>())
			OS << "RLC_TYPE(" << casted.getName() << ")\n";
	OS << "#undef RLC_TYPE\n";
	OS << "#endif\n\n";

	for (auto fun : Module.getOps<mlir::rlc::FunctionOp>())
	{
		if (fun.isInternal())
			continue;
		if (fun.getUnmangledName() == "main")
			continue;
		std::string cShortName = ((not fun.getFunctionType().getInputs().empty() and
															 fun.getFunctionType()
																	 .getInputs()
																	 .front()
																	 .isa<mlir::rlc::EntityType>())
																	? fun.getFunctionType()
																						.getInputs()
																						.front()
																						.cast<mlir::rlc::EntityType>()
																						.getName() +
																				"_" + fun.getUnmangledName()
																	: fun.getUnmangledName())
																 .str();

		printFunctionSignature(
				fun.getUnmangledName(),
				fun.getMangledName(),
				cShortName,
				fun.getFunctionType(),
				fun.getArgNames(),
				OS);
	}

	mlir::rlc::ModuleBuilder builder(Module);
	for (auto fun : Module.getOps<mlir::rlc::ActionFunction>())
	{
		printFunctionSignature(
				fun.getUnmangledName(),
				fun.getMangledName(),
				(fun.getEntityType().getName() + "_" + fun.getUnmangledName()).str(),
				fun.getFunctionType(),
				fun.getArgNames(),
				OS);

		llvm::iterator_range<mlir::Operation**> actionsStatements =
				builder.actionStatementsOfAction(fun);
		for (const auto& [action, type] :
				 llvm::zip(actionsStatements, fun.getActions()))
		{
			auto casted = mlir::cast<mlir::rlc::ActionStatement>(action);

			auto castedType = type.getType().cast<mlir::FunctionType>();
			llvm::SmallVector<mlir::Attribute, 2> attrs(
					{ mlir::StringAttr::get(casted.getContext(), "self") });
			for (auto attr : casted.getDeclaredNames().getAsRange<mlir::StringAttr>())
				attrs.push_back(attr);

			printFunctionSignature(
					casted.getName(),
					mlir::rlc::mangledName(casted.getName(), castedType),
					(fun.getEntityType().getName() + "_" + casted.getName()).str(),
					castedType,
					mlir::ArrayAttr::get(casted.getContext(), attrs),
					OS);
		}

		printFunctionSignature(
				"is_done",
				mlir::rlc::mangledName("is_done", fun.getIsDoneFunctionType()),
				(fun.getEntityType().getName() + "_is_done").str(),
				fun.getIsDoneFunctionType(),
				mlir::ArrayAttr::get(
						Module.getContext(),
						{ mlir::StringAttr::get(Module.getContext(), "arg0") }),
				OS);
	}

	OS << "#ifdef RLC_GET_FUNCTION_DECLS\n";
	OS << "#undef RLC_GET_FUNCTION_DECLS\n";
	OS << "#endif\n\n";

	OS << "#ifdef RLC_VISIT_FUNCTION\n";
	OS << "#undef RLC_VISIT_FUNCTION\n";
	OS << "#endif\n";
}

static std::string unmangledName(
		mlir::Value op, mlir::rlc::ModuleBuilder& builder)
{
	if (auto casted = mlir::dyn_cast<mlir::rlc::FunctionOp>(op.getDefiningOp()))
	{
		return casted.getUnmangledName().str();
	}
	if (auto casted =
					mlir::dyn_cast<mlir::rlc::ActionFunction>(op.getDefiningOp()))
	{
		if (casted.getIsDoneFunction() == op)
			return "is_done";

		if (auto maybeStatement =
						mlir::dyn_cast_or_null<mlir::rlc::ActionStatement>(
								builder.actionFunctionValueToActionStatement(op));
				maybeStatement)
			return maybeStatement.getName().str();

		return casted.getUnmangledName().str();
	}
	llvm_unreachable("unrechable");
	return "";
}

static std::string mangledName(
		mlir::Value op, mlir::rlc::ModuleBuilder& builder)
{
	if (auto casted = mlir::dyn_cast<mlir::rlc::FunctionOp>(op.getDefiningOp()))
	{
		return casted.getMangledName();
	}
	if (auto casted =
					mlir::dyn_cast<mlir::rlc::ActionFunction>(op.getDefiningOp()))
	{
		return mlir::rlc::mangledName(
				unmangledName(op, builder), op.getType().cast<mlir::FunctionType>());
	}
	llvm_unreachable("unrechable");
	return "";
}

void rlc::rlcToGodot(mlir::ModuleOp Module, llvm::raw_ostream& OS)
{
	OS << "extern \"C\"{\n";
	OS << "#define RLC_GET_TYPE_DECLS\n";
	OS << "#define RLC_GET_FUNCTION_DECLS\n";

	rlcToCHeader(Module, OS);
	OS << "}\n";
	mlir::rlc::ModuleBuilder builder(Module);

	OS << "#ifdef RLC_GODOT\n";
	OS << "#undef RLC_GODOT\n";
	OS << "#include <Godot.hpp>\n";
	OS << "#include <Node.hpp>\n";
	for (auto type : postOrderTypes(Module))
	{
		if (type.isa<mlir::rlc::ArrayType>() or type.isa<mlir::FunctionType>())
			continue;
		auto wrapperNameString = nonArrayTypeToString(type) + "Wrapper";
		const auto* wrapperName = wrapperNameString.c_str();
		OS << llvm::format(
				R"""""(
class %s: public godot::Node {
  GODOT_CLASS(%s, godot::Node);

public:
  %s content;

  static void _register_methods() {
    godot::register_method("_init", &%s::_init);
)""""",
				wrapperName,
				wrapperName,
				nonArrayTypeToString(type).c_str(),
				wrapperName);

		OS << llvm::format(
				R"""""(
		}
  %s() {_init();}
  void _init() {
					)""""",
				wrapperName);

		OS << builder.getInitFunctionOf(type)
							.getDefiningOp<mlir::rlc::FunctionOp>()
							.getMangledName()
			 << "(&content);\n}\n";

		OS << "};\n";
	}

	OS << R"""""(
class RLCLib: public godot::Node {
  GODOT_CLASS(RLCLib, godot::Node);

public:
  static void _register_methods() {
    register_method("_init", &RLCLib::_init);
)""""";

	for (const auto& pair : builder.getSymbolTable().allDirectValues())
	{
		if (pair.first().starts_with("_") or pair.first() == "main")
			continue;
		OS << "register_method(\"" << pair.first() << "\", &RLCLib::RLC_"
			 << pair.first() << ");\n";
	}

	OS << R"""""(
  }

  RLCLib() {}

  void _init() {}
)""""";

	for (const auto& pair : builder.getSymbolTable().allDirectValues())
	{
		if (pair.first().starts_with("_") or pair.first() == "main")
			continue;
		OS << "godot::Variant RLC_" << pair.first() << "(godot::Variant args);\n";
	}

	OS << "};\n";

	for (const auto& pair : builder.getSymbolTable().allDirectValues())
	{
		if (pair.first().starts_with("_") or pair.first() == "main")
			continue;
		OS << "godot::Variant RLCLib::RLC_" << pair.first()
			 << "(godot::Variant args){\n";
		OS << "godot::Array casted = args;\n";

		for (auto& overload : pair.second)
		{
			if (auto casted = overload.getDefiningOp<mlir::rlc::FunctionOp>();
					casted and casted.isInternal())
				continue;

			auto fType = overload.getType().cast<mlir::FunctionType>();
			bool isVoid = fType.getNumResults() == 0 or
										fType.getResults().front().isa<mlir::rlc::VoidType>();
			OS << "if (casted.size() == " << fType.getNumInputs();

			for (size_t I = 0; I < fType.getNumInputs(); I++)
			{
				if (not fType.getInput(I).isa<mlir::rlc::EntityType>())
					continue;
				OS << " && godot::Object::cast_to<" << typeToString(fType.getInput(I))
					 << "Wrapper>((godot::Object*)(casted[" << I << "]))";
			}

			OS << ") {\n";

			for (size_t I = 0; I < fType.getNumInputs(); I++)
			{
				if (not fType.getInput(I).isa<mlir::rlc::EntityType>())
				{
					OS << "auto arg" << I << "= ";
					OS << "((" << typeToString(fType.getInput(I)) << ")(casted[" << I
						 << "]));\n ";
				}
			}

			if (not isVoid)
			{
				if (fType.getResult(0).isa<mlir::rlc::EntityType>())
				{
					OS << "auto to_return =" << typeToString(fType.getResult(0))
						 << "Wrapper::_new();\nto_return->content = ";
				}
				else
				{
					OS << "auto to_return =";
				}
			}

			OS << mangledName(overload, builder) << "(";

			for (size_t I = 0; I < fType.getNumInputs(); I++)
			{
				if (fType.getInput(I).isa<mlir::rlc::EntityType>())
					OS << "&(godot::Object::cast_to<" << typeToString(fType.getInput(I))
						 << "Wrapper>((godot::Object*)(casted[" << I << "]))->content)";
				else
					OS << "&arg" << I;
				if (I + 1 < fType.getNumInputs())
					OS << ", ";
			}

			OS << ");\n";
			if (isVoid)
				OS << "return godot::Variant();\n";
			else
				OS << "return to_return;\n";
			OS << "}\n";
		}

		OS << "return godot::Variant();";
		OS << "}\n";
	}

	OS << R"""""(
/** GDNative Initialize **/
extern "C" void GDN_EXPORT godot_gdnative_init(godot_gdnative_init_options *o) {
  godot::Godot::gdnative_init(o);
}

/** GDNative Terminate **/
extern "C" void GDN_EXPORT
godot_gdnative_terminate(godot_gdnative_terminate_options *o) {
  godot::Godot::gdnative_terminate(o);
}

/** NativeScript Initialize **/
extern "C" void GDN_EXPORT godot_nativescript_init(void *handle) {
  godot::Godot::nativescript_init(handle);
)""""";

	for (auto type : postOrderTypes(Module))
	{
		if (type.isa<mlir::rlc::ArrayType>() or type.isa<mlir::FunctionType>())
			continue;

		OS << "godot::register_class<" << typeToString(type) << "Wrapper>();";
	}

	OS << R"""""(
  godot::register_class<RLCLib>();
}
)""""";

	OS << "#endif\n";
}
