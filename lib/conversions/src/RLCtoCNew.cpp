/*
Copyright 2025 Leila Shekofteh

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

#include "mlir/IR/BuiltinOps.h"
#include "mlir/IR/TypeRange.h"
#include "rlc/conversions/RLCToC.hpp"
#include "mlir/Pass/Pass.h"
#include "llvm/ADT/TypeSwitch.h"
#include "llvm/Support/Format.h"
#include "rlc/dialect/Operations.hpp"
#include "rlc/dialect/SymbolTable.h"
#include "rlc/dialect/Types.hpp"
#include "rlc/dialect/Visits.hpp"
#include "rlc/utils/PatternMatcher.hpp"
#include "rlc/dialect/Passes.hpp"
#include "rlc/dialect/MemberFunctionsTable.hpp"





namespace mlir::rlc
{

    static void printPrelude(
        StreamWriter& writer,
        mlir::ModuleOp Module
    ){
        // writer.writenl("#ifdef __cplusplus");
        // writer.writenl("extern \"C\" {");
        // writer.writenl("endif");
        writer.writenl("#ifdef RLC_HEADER");
        writer.writenl("#ifdef RLC_C_HEADER");
        writer.writenl("#undef RLC_C_HEADER");
        writer.writenl("#define RLC_HEADER");
        writer.writenl("#include \"stddef.h\"");
        writer.writenl("#include \"stdint.h\"");
        writer.writenl("#define RLC_GET_FUNCTION_DECLS");
        writer.writenl("#define RLC_GET_TYPE_DECLS");

        writer.writenl("#define RLC_VISIT_FUNCTION(name, mangled_name, cShortName return_type, ...)");
        writer.writenl("static inline return_type cShortName(RLC_ARGUMENTS) {");
        writer.writenl("return_type ret_value;");
        writer.writenl("mangled_name(&ret_value, __VA_ARGS__);");
        writer.writenl("return ret_value;");
        writer.writenl("}");
        writer.writenl("#endif");

        writer.writenl("#ifdef RLC_GET_TYPE_DECLS");

        for (auto type : ::rlc::postOrderTypes(Module))
		    writer.writeType(type);
        
        writer.writenl("#undef RLC_GET_TYPE_DECLS");
        writer.writenl("#endif");
        writer.endLine();

        // class type definition ???
        writer.writenl("#ifdef RLC_TYPE");
        for (auto type : ::rlc::postOrderTypes(Module))
            writer.writeType(type);
        writer.writenl("#undef RLC_TYPE");
        writer.writenl("#endif");
        writer.endLine();
    }

    static void registerCommonTypeSerialization(TypeSerializer& s)
	{
        s.add([](IntegerLiteralType type, llvm::raw_string_ostream& OS)-> void{
            OS << type.getValue();
        });
        s.add([&](mlir::rlc::FrameType type, llvm::raw_string_ostream& OS) {
			OS << s.convert(type.getUnderlying());
		});
        s.add([&](mlir::rlc::ContextType type, llvm::raw_string_ostream& OS) {
			OS << s.convert(type.getUnderlying());
		});
		s.add([&](mlir::rlc::ArrayType type, llvm::raw_string_ostream& OS) {
			OS << s.convert(type.getUnderlying()) << " [ "
			<< s.convert(type.getSize())
            << "]";
		});
		s.add([&](mlir::rlc::OwningPtrType type, llvm::raw_string_ostream& OS) {
			OS << s.convert(type.getUnderlying()) << "*";
		});
		s.add([&](mlir::rlc::ReferenceType type, llvm::raw_string_ostream& OS) {
			OS << s.convert(type.getUnderlying()) << "*";
		});
		s.add(
				[&](mlir::rlc::AlternativeType type, llvm::raw_string_ostream& OS) {
					OS << type.getMangledName();
				});
        s.add([&](mlir::rlc::ClassType type, llvm::raw_string_ostream& OS) {
			OS << "RLC_TYPE(" << type.mangledName() << ")\n";
		});
	}

    static void registerCTypeSerializion(TypeSerializer& s){
        s.add([](IntegerType type, llvm::raw_string_ostream& OS)-> void{
            OS << "int" << type.getSize() << "_t";
        });
        s.add([](FloatType type, llvm::raw_string_ostream& OS)-> void{
            OS << "double" ;
        });
        s.add([](BoolType type, llvm::raw_string_ostream& OS)-> void{
            OS << "bool" ;
        });
        s.add([](StringLiteralType type, llvm::raw_string_ostream& OS)-> void{
            OS << "char*" ;
        });
        s.add([](VoidType type, llvm::raw_string_ostream& OS)-> void{
            OS << "void" ;
        });
        s.add(
            [&](AlternativeType type, llvm::raw_string_ostream& OS) {
               OS << "struct " << type.getMangledName();
            });
        s.add(
            [&](ClassType type, llvm::raw_string_ostream& OS) {
                OS << "union " << type.mangledName();
        });
        registerCommonTypeSerialization(s);
    }

    static void printMangledWrapper(
        llvm::StringRef unmangledName,
        llvm::StringRef mangledName,
        llvm::ArrayRef<llvm::StringRef> fieldNames,
        mlir::FunctionType type,
        StreamWriter& w,
        mlir::Type resultType,
        bool isMemberFunction
    ) {

    }

    static void printFieldName(
        llvm::StringRef fieldName,
		mlir::Type type,
		StreamWriter& writer,
		bool isRef = false
    ){
        if (not type.isa<mlir::rlc::ArrayType>())
	    {
            if (isRef)
                writer.write("(&");
            writer.write(" " + fieldName);
            if (isRef)
                writer.write(")");
	    }
    }

    static void printFunctionDecl(
        llvm::ArrayRef<llvm::StringRef> argsNames,
        TypeRange types,
        mlir::Type resultType,
        StreamWriter& writer
    ){
        writer.write("(");
        if (not mlir::isa<mlir::rlc::VoidType>(resultType)){
            writer.writeType(resultType);
            printFieldName(" * __result", resultType, writer);
            if (types.size() != 0) writer.write(", ");
        }

        for (size_t index = 0; index != types.size(); index++)
		{
			writer.writeType(types[index]);
            printFieldName("* " + argsNames[index].str(), types[index], writer);
            if (index + 1 < argsNames.size()) writer.write(", ");
		}
        writer.writenl(")");
    }

    static void printVisitFunctionDecl(
        llvm::StringRef originalName,
        std::string name,
        llvm::StringRef cShortName,
        llvm::ArrayRef<llvm::StringRef> argsNames,
        TypeRange types,
        mlir::Type resultType,
        StreamWriter& writer
    ) {
        writer.write("#define RLC_ARGUMENTS_COUNT ");
        writer.writenl(types.size());
        writer.write("#define RLC_ARGUMENTS ");
        size_t I = 0;
        for (auto [type, name] : llvm::zip(types, argsNames))
        {
            I++;
            writer.writeType(type);
            printFieldName("* ", type, writer);
            writer.write(" " + name);
            if (I != argsNames.size()) writer.write(", ");
        }
        writer.endLine();

        writer.write("RLC_VISIT_FUNCTION");
        writer.write("(");
        writer.write(originalName + ", ");
        writer.write(name + ", ");
        writer.write(cShortName + ", ");

        if (not mlir::isa<mlir::rlc::VoidType>(resultType)){
            writer.writeType(resultType);
            if (types.size() != 0) writer.write(", ");
        }else {
            writer.write("void");
        }
        writer.write(", ");

        I = 0;
        for (auto name : argsNames)
        {
            I++;
            writer.write(" " + name);
            if (I != argsNames.size()) writer.write(", ");
        }
        writer.writenl(")");
    }

    static void declareFunction(
        llvm::StringRef unmangledName,
		bool isMemberFunction,
		llvm::StringRef cShortName,
		mlir::FunctionType type,
		llvm::ArrayRef<llvm::StringRef> argsNames,
		StreamWriter& writer
    ){
        std::string mangledName = mlir::rlc::mangledName(unmangledName, isMemberFunction, type);

        mlir::Type resultType = type.getNumResults() == 0
        ? mlir::rlc::VoidType::get(type.getContext())
        : type.getResult(0);

        writer.writenl("#ifdef RLC_GET_FUNCTION_DECLS");
        writer.write("void ", mangledName);
        printFunctionDecl(argsNames, type.getInputs(), resultType, writer);
        writer.writenl("#endif");
        writer.endLine();

        // Visit function
        writer.writenl("#ifdef RLC_VISIT_FUNCTION");

        printVisitFunctionDecl(unmangledName, mangledName, cShortName, argsNames, type.getInputs(), resultType, writer);

        writer.writenl("#undef RLC_ARGUMENTS_COUNT");
        writer.writenl("#undef RLC_ARGUMENTS");
        writer.writenl("#endif");
        writer.endLine();
    }

    static void printFunctionAndCanFunctionSignature(
        llvm::StringRef UnmangledName,
		bool isMemberFunction,
		llvm::StringRef cShortName,
		mlir::FunctionType type,
		llvm::ArrayRef<llvm::StringRef> argNames,
        bool preconditionExist,
		StreamWriter& writer
    ){
        declareFunction(
            UnmangledName,
            isMemberFunction,
            cShortName, 
            type, 
            argNames,
            writer);

        auto canDoType = mlir::FunctionType::get(
                    type.getContext(),
                    type.getInputs(),
                    { mlir::rlc::BoolType::get(type.getContext()) });

        if (preconditionExist)
            declareFunction(
            "can_" + UnmangledName.str(),
            isMemberFunction,
            "can_" + cShortName.str(), 
            canDoType, 
            argNames,
            writer);
    }
    
    class FunctionToCFunction
    {
        public:
        
        void apply(FunctionOp op, StreamWriter& w)
        {
            if (op.isInternal())
			    return;
		    if (op.getUnmangledName() == "main")
			    return;
            std::string cShortName = 
            ((not op.getFunctionType().getInputs().empty() and op.getFunctionType().getInputs().front().isa<mlir::rlc::ClassType>()) ? op.getFunctionType().getInputs().front().cast<mlir::rlc::ClassType>().getName() + "_" + op.getUnmangledName() : op.getUnmangledName()).str();

            declareFunction(
                op.getUnmangledName(),
                op.getIsMemberFunction(),
                cShortName, 
                op.getFunctionType(), 
                op.getInfo().getArgNames(),
                w);
            if (not op.getPrecondition().empty())
                declareFunction(
                "can_" + op.getUnmangledName().str(),
                op.getIsMemberFunction(),
                cShortName, 
                mlir::FunctionType::get(
                    op.getContext(),
                    op.getType(),
                    { mlir::rlc::BoolType::get(op.getContext()) }), 
                op.getInfo().getArgNames(),
                w);
        }
    };

    class ActionToCFunction
    {
        private:
		mlir::rlc::ModuleBuilder& builder;

        public:
		ActionToCFunction(mlir::rlc::ModuleBuilder& builder): builder(builder)
		{
		}
        void apply(mlir::rlc::ActionFunction op, mlir::rlc::StreamWriter& w)
        {
            std::string cShortName = (op.getClassType().getName() + "_" + op.getUnmangledName()).str();

            printFunctionAndCanFunctionSignature(
                op.getUnmangledName(), 
                false, 
                cShortName,
                op.getFunctionType(),
                op.getInfo().getArgNames(),
                !op.getPrecondition().empty(),
                w);

            // using ActionKey = std::pair<std::string, const void*>;
            // std::set<ActionKey> alreadyAdded;

            for (const auto value : op.getActions()){
                mlir::Operation* statement =
						builder.actionFunctionValueToActionStatement(value).front();

                auto actionStatement =
						mlir::cast<mlir::rlc::ActionStatement>(statement);

                // ActionKey key(actionStatement.getName(), value.getAsOpaquePointer());
                // if (alreadyAdded.contains(key))
                //     continue;

                // alreadyAdded.insert(key);

                llvm::SmallVector<llvm::StringRef> argNames = { "self" };
				for (auto arg : actionStatement.getDeclaredNames())
					argNames.push_back(arg);

                auto functionType = mlir::cast<mlir::FunctionType>(value.getType());

                std::string cShortName = (op.getClassType().getName() + "_" + actionStatement.getName()).str();

                printFunctionAndCanFunctionSignature(actionStatement.getName(),
                true, 
                cShortName,
                functionType, 
                argNames, 
                true,
                w);
            }

            cShortName = (op.getClassType().getName() + "_is_done").str();

            printFunctionAndCanFunctionSignature(
                "is_done", 
                true, 
                cShortName, 
                op.getIsDoneFunctionType(), 
                {"arg0"}, 
                false, 
                w);

            w.writenl("#ifdef RLC_GET_FUNCTION_DECLS");
            w.writenl("#undef RLC_GET_FUNCTION_DECLS");
            w.writenl("#endif");
            w.endLine();

            w.writenl("#ifdef RLC_VISIT_FUNCTION");
            w.writenl("#undef RLC_VISIT_FUNCTION");
            w.writenl("#endif");
        }
    };

    class AliasToCAlias
    {
        public:
        void apply(mlir::rlc::TypeAliasOp op, mlir::rlc::StreamWriter& w){

            w.write("typedef ");
            w.writeType(op.getAliased());
            w.write(" ");
            w.write(op.getName());
            w.writenl(";");
                
        }
    };



    #define GEN_PASS_DEF_PRINTCHEADERPASS
    #include "rlc/dialect/Passes.inc"
    
    struct PrintCHeaderPass:
    impl::PrintCHeaderPassBase<PrintCHeaderPass>{
        using impl::PrintCHeaderPassBase<PrintCHeaderPass>::PrintCHeaderPassBase;
        void runOnOperation() override
        {
            rlc::PatternMatcher matcher(*OS);
            MemberFunctionsTable table(getOperation());
            mlir::rlc::ModuleBuilder builder(getOperation());
            mlir::ModuleOp module;
            matcher.addTypeSerializer();
            registerCTypeSerializion(matcher.getWriter().getTypeSerializer());

            matcher.add<FunctionToCFunction>();
            matcher.add<ActionToCFunction>(builder);
            matcher.add<AliasToCAlias>();

            printPrelude(matcher.getWriter(), module);

            // emit declarations of functions
			matcher.apply(getOperation());
        }
    };

}

