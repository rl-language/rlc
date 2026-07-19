#include <iostream>

#include "JavascriptWrapperStandard.h"
#include "clang/Format/Format.h"
#include "clang/Tooling/Core/Replacement.h"
#include "llvm/Support/Error.h"
#include "mlir/Pass/Pass.h"
#include "mlir/Transforms/DialectConversion.h"
#include "rlc/dialect/ActionArgumentAnalysis.hpp"
#include "rlc/dialect/Dialect.h"
#include "rlc/dialect/MemberFunctionsTable.hpp"
#include "rlc/dialect/Operations.hpp"
#include "rlc/dialect/Passes.hpp"
#include "rlc/dialect/Types.hpp"
#include "rlc/dialect/Visits.hpp"
#include "rlc/utils/PatternMatcher.hpp"

// TODO: Rimettere il file build.sh com'era all'inizio.
// TODO: Le alternative usate dentro le funzioni compaiono comunque...
// TODO: extern keyword
// TODO: Action functions
// TODO: Per le member functions non generiamo le precondizioni? Nel wrapper di
// python non viene fatto (vengono generate solo per le non-member funcitons).
// TODO: Aggiungere i dollari per evitare le collisioni
// TODO: Scrivere la funzione javascript getAddressSize(), che completiamo in
// base a isWasm64
// TODO: Vedere se puoi evitare di usare std::string

namespace mlir::rlc
{

#define GEN_PASS_DEF_PRINTJAVASCRIPTPASS
#include "rlc/dialect/Passes.inc"

	namespace
	{
		class JavascriptCodeGenerator
		{
			private:
			std::string unformattedBuffer{};
			llvm::raw_string_ostream printer{ unformattedBuffer };
			const bool isWasm64{};
			llvm::raw_ostream* OS{};
			MemberFunctionsTable table;
			llvm::DenseSet<mlir::rlc::ArrayType> arrays{};
			// TODO: Dubito che alternatives ci serve...o forse sì...?
			// Magari al posto di quel ciclo nel main, così escludiamo le alternative
			// definite all'interno delle funzioni.
			llvm::DenseSet<mlir::rlc::AlternativeType> alternatives{};
			llvm::DenseSet<mlir::rlc::OwningPtrType> pointers{};

			public:
			JavascriptCodeGenerator(
					bool isWasm64, llvm::raw_ostream* OS, mlir::ModuleOp operation)
					: isWasm64(isWasm64), OS(OS), table(MemberFunctionsTable{ operation })
			{
				if (this->isWasm64)
				{
					// Rimuovi questo if
				}
			}

			void autoIndentMyCode()
			{
				printer.flush();

				clang::format::FormatStyle style = clang::format::getLLVMStyle();
				style.Language = clang::format::FormatStyle::LK_JavaScript;

				std::vector<clang::tooling::Range> ranges{
					1, clang::tooling::Range(0, unformattedBuffer.size())
				};

				clang::tooling::Replacements replaces = clang::format::reformat(
						style, unformattedBuffer, ranges, "youCanInsertAnythingHere.js");

				auto formattedCode =
						clang::tooling::applyAllReplacements(unformattedBuffer, replaces);

				if (formattedCode)
				{
					*OS << (*formattedCode);
				}
				else
				{
					llvm::consumeError(formattedCode.takeError());
					*OS << unformattedBuffer;
				}
			}

			void generateGeneralWrapper() { printer << standardJavascriptWrapper; }

			// TODO: Rendere questa funzione privata?
			std::pair<llvm::SmallVector<int, 2>, mlir::Type> getSizeAndTypeFromArray(
					mlir::rlc::ArrayType currentType)
			{
				llvm::SmallVector<int, 2> sizes{};

				while (true)
				{
					const int value =
							cast<mlir::rlc::IntegerLiteralType>(currentType.getSize())
									.getValue();
					sizes.insert(sizes.begin(), value);

					auto newType =
							mlir::dyn_cast<mlir::rlc::ArrayType>(currentType.getUnderlying());
					if (newType)
					{
						currentType = newType;
					}
					else
					{
						return { sizes, currentType.getUnderlying() };
					}
				}
			}

			std::pair<int, int> getSizeAndAlignmentByType(mlir::Type type)
			{
				if (auto casted = mlir::dyn_cast<mlir::rlc::IntegerType>(type))
				{
					if (casted.getSize() == 64)
					{
						return { 8, 8 };
					}

					return { 1, 1 };
				}

				if (mlir::isa<mlir::rlc::FloatType>(type))
				{
					return { 8, 8 };
				}

				if (mlir::isa<mlir::rlc::BoolType>(type))
				{
					return { 1, 1 };
				}

				if (mlir::isa<mlir::rlc::StringLiteralType>(type))
				{
					int addressSize = isWasm64 ? 8 : 4;
					return { addressSize, addressSize };
				}

				if (auto casted = mlir::dyn_cast<mlir::rlc::ClassType>(type))
				{
					const auto result = getSizeAlignmentAndOffsetsOfStruct(casted);
					return { std::get<0>(result), std::get<1>(result) };
				}

				if (auto casted = mlir::dyn_cast<mlir::rlc::AlternativeType>(type))
				{
					llvm::SmallVector<std::pair<int, int>, 4> structFieldData{};
					structFieldData.push_back(getSizeAndAlignmentOfUnion(casted));
					structFieldData.push_back(getSizeAndAlignmentByType(
							mlir::rlc::IntegerType::get(type.getContext(), 64)));

					const auto result =
							getSizeAlignmentAndOffsetsOfStruct(structFieldData);
					return { std::get<0>(result), std::get<1>(result) };
				}

				if (auto casted = mlir::dyn_cast<mlir::rlc::ArrayType>(type))
				{
					std::pair<llvm::SmallVector<int, 2>, mlir::Type> result{
						getSizeAndTypeFromArray(casted)
					};

					int linearLength = 1;
					for (int x : result.first)
					{
						linearLength *= x;
					}

					int underlyingSize = getSizeAndAlignmentByType(result.second).first;

					return { linearLength * underlyingSize, underlyingSize };
				}

				if (auto casted = mlir::dyn_cast<mlir::rlc::OwningPtrType>(type))
				{
					int addressSize = isWasm64 ? 8 : 4;
					return { addressSize, addressSize };
				}

				std::cerr << "Can't compute size of type ";
				type.dump();
				std::abort();
			}

			std::pair<int, int> getSizeAndAlignmentOfUnion(
					mlir::rlc::AlternativeType type)
			{
				int maxSize = -1;
				int maxAlignment = -1;

				for (auto enumeration : llvm::enumerate(type.getUnderlying()))
				{
					std::pair<int, int> currentSizeAndAlignment =
							getSizeAndAlignmentByType(enumeration.value());

					if (currentSizeAndAlignment.first > maxSize)
					{
						maxSize = currentSizeAndAlignment.first;
					}

					if (currentSizeAndAlignment.second > maxAlignment)
					{
						maxAlignment = currentSizeAndAlignment.second;
					}
				}

				return { maxSize, maxAlignment };
			}

			std::tuple<int, int, llvm::SmallVector<int, 4>>
			getSizeAlignmentAndOffsetsOfStruct(
					mlir::rlc::ClassType classType

			)
			{
				llvm::SmallVector<std::pair<int, int>, 4> output{};
				auto memberTypes = classType.getMemberTypes();
				std::transform(
						memberTypes.begin(),
						memberTypes.end(),
						std::back_inserter(output),
						[this](mlir::Type type) {
							return getSizeAndAlignmentByType(type);
						});
				return getSizeAlignmentAndOffsetsOfStruct(output);
			}

			std::tuple<int, int, llvm::SmallVector<int, 4>>
			getSizeAlignmentAndOffsetsOfStruct(
					llvm::SmallVector<std::pair<int, int>, 4> structFieldData)
			{
				llvm::SmallVector<int, 4> offsets{};

				int structSize = 0;
				int maxAlignment = -1;

				for (size_t i{ 0 }; i < structFieldData.size(); i++)
				{
					const int memberSize = structFieldData[i].first;
					const int memberAlignment = structFieldData[i].second;

					structSize += structSize % memberAlignment;
					offsets.push_back(structSize);
					structSize += memberSize;

					if (memberAlignment > maxAlignment)
					{
						maxAlignment = memberAlignment;
					}
				}
				structSize += structSize % maxAlignment;

				return { structSize, maxAlignment, offsets };
			}

			void emitJavascriptType(mlir::Type type)
			{
				if (auto casted = mlir::dyn_cast<mlir::rlc::IntegerType>(type))
				{
					if (casted.getSize() == 64)
					{
						printer << "IntWrapper";
					}
					else
					{
						printer << "ByteWrapper";
					}
				}
				else if (mlir::isa<mlir::rlc::FloatType>(type))
				{
					printer << "FloatWrapper";
				}
				else if (mlir::isa<mlir::rlc::BoolType>(type))
				{
					printer << "BoolWrapper";
				}
				else if (mlir::isa<mlir::rlc::StringLiteralType>(type))
				{
					printer << "StringLiteralWrapper";
				}
				else if (mlir::isa<mlir::rlc::VoidType>(type))
				{
					printer << "null";
				}
				else if (auto casted = mlir::dyn_cast<mlir::rlc::ClassType>(type))
				{
					printer << casted.getName();
				}
				else if (auto casted = mlir::dyn_cast<mlir::rlc::AlternativeType>(type))
				{
					alternatives.insert(casted);

					printer << "Alternative";
					for (auto enumeration : llvm::enumerate(casted.getUnderlying()))
					{
						printer << "_";
						emitJavascriptType(enumeration.value());
					}
				}
				else if (auto casted = mlir::dyn_cast<mlir::rlc::ArrayType>(type))
				{
					arrays.insert(casted);

					printer << "Array_";

					std::pair<llvm::SmallVector<int, 2>, mlir::Type> result{
						getSizeAndTypeFromArray(casted)
					};
					emitJavascriptType(result.second);

					for (int x : result.first)
					{
						printer << "_" << x;
					}
				}
				else if (auto casted = mlir::dyn_cast<mlir::rlc::OwningPtrType>(type))
				{
					pointers.insert(casted);

					printer << "Ptr_";
					emitJavascriptType(casted.getUnderlying());
				}
				else if (auto casted = mlir::dyn_cast<mlir::rlc::ReferenceType>(type))
				{
					emitJavascriptType(casted.getUnderlying());
				}
				else if (
						auto casted = mlir::dyn_cast<mlir::rlc::IntegerLiteralType>(type))
				{
					printer << casted.getValue();
				}
				else
				{
					std::cerr << "Can't emit an equivalent Javascript type for ";
					type.dump();
					std::abort();
				}
			}

			void emitOverloadDispatcher(
					llvm::StringRef name,
					llvm::ArrayRef<mlir::FunctionType> overloads,
					bool isMethod)
			{
				if (isMethod == false)
				{
					printer << "export function ";
				}
				printer << name << "(...args){";

				if (isMethod)
				{
					printer << "this._assertAddress();";
				}

				printer << "const signatures = [";

				for (auto overload : overloads)
				{
					printer << "[\"" << mangledName(name, isMethod, overload) << "\", [";

					for (auto type : overload.getInputs())
					{
						emitJavascriptType(type);
						printer << ", ";
					}

					mlir::Type result{};
					if (overload.getNumResults() == 0)
					{
						result = mlir::rlc::VoidType::get(overload.getContext());
					}
					else
					{
						result = overload.getResults()[0];
					}
					printer << "], ";
					emitJavascriptType(result);
					printer << ", ";
					if (mlir::dyn_cast<mlir::rlc::ReferenceType>(result))
					{
						printer << "true";
					}
					else
					{
						printer << "false";
					}
					printer << "], ";
				}
				printer << "];";

				if (isMethod)
				{
					printer << "args.unshift(this);";
				}

				printer << "return generalFunction(args, signatures);}";
			}

			void emitGettersAndSetters(
					mlir::rlc::ClassType classType, llvm::SmallVector<int, 4> offsets)
			{
				for (auto [type, name, offset] : llvm::zip(
								 classType.getMemberTypes(),
								 classType.getMemberNames(),
								 offsets))
				{
					printer << "get " << name << "(){return this._get(";
					emitJavascriptType(type);
					printer << ", " << offset << ");}";

					printer << "set " << name << "(value){this._set(";
					emitJavascriptType(type);
					printer << ", " << offset << ", value);}";
				}
			}

			void emitMemberFunctions(mlir::Type classType)
			{
				llvm::StringMap<llvm::SmallVector<mlir::FunctionType>> sortedOverloads;
				for (auto memberFunction : this->table.getMemberFunctionsOf(classType))
				{
					sortedOverloads[memberFunction.getUnmangledName()].push_back(
							memberFunction.getType());
				}

				if (not this->table.isTriviallyInitializable(classType))
				{
					auto fun = mlir::FunctionType::get(
							classType.getContext(), { classType }, {});
					sortedOverloads["init"].push_back(fun);
				}

				if (not this->table.isTriviallyDestructible(classType))
				{
					auto fun = mlir::FunctionType::get(
							classType.getContext(), { classType }, {});
					sortedOverloads["drop"].push_back(fun);
				}

				// TODO: C'è davvero bisogno di generare l'assign?
				if (not this->table.isTriviallyCopiable(classType))
				{
					auto fun = mlir::FunctionType::get(
							classType.getContext(), { classType, classType }, {});
					sortedOverloads["assign"].push_back(fun);
				}

				for (auto& pair : sortedOverloads)
				{
					emitOverloadDispatcher(pair.first(), pair.second, true);
				}
			}

			void emitClassDeclaration(mlir::rlc::ClassType type)
			{
				printer << "export class ";
				emitJavascriptType(type);
				printer << " extends ClassWrapper{";

				printer << "static _getSize() { return ";
				auto result = getSizeAlignmentAndOffsetsOfStruct(type);
				printer << std::get<0>(result) << ";}";
				printer << "static _getMemberFieldNames(){ return [";
				for (auto name : type.getMemberNames())
				{
					printer << "\"" << name << "\", ";
				}

				printer << "]; }";
				emitGettersAndSetters(type, std::get<2>(result));
				emitMemberFunctions(type);
				printer << "}";
			}

			void emitEnumDeclaration(
					mlir::rlc::ClassType type, mlir::rlc::EnumDeclarationOp enumDecl)
			{
				printer << "export class ";
				emitJavascriptType(type);
				printer << " extends EnumWrapper{";

				int i{ 0 };
				for (auto value :
						 llvm::enumerate(enumDecl.getBody()
																 .getOps<mlir::rlc::EnumFieldDeclarationOp>()))
				{
					printer << "static " << value.value().getName() << " = " << i << ";";
					i++;
				}

				emitMemberFunctions(type);
				printer << "}";
			}

			void emitAlternativeDeclarations()
			{
				for (auto type : alternatives)
				{
					printer << "export class ";
					emitJavascriptType(type);
					printer << " extends AlternativeWrapper{";

					printer << "static _getSize() { return "
									<< getSizeAndAlignmentByType(type).first << ";}";
					printer << "static _getAlternativeClasses() { return [";

					for (auto enumeration : llvm::enumerate(type.getUnderlying()))
					{
						emitJavascriptType(enumeration.value());
						printer << ", ";
					}

					printer << "];}";

					emitMemberFunctions(type);
					printer << "}";
				}
			}

			void emitArrayDeclarations()
			{
				for (auto type : arrays)
				{
					printer << "export class ";
					emitJavascriptType(type);
					printer << " extends ArrayWrapper{";

					std::pair<llvm::SmallVector<int, 2>, mlir::Type> result{
						getSizeAndTypeFromArray(type)
					};

					printer << "static _getDimensions(){ return [";
					for (int x : result.first)
					{
						printer << x << ", ";
					}
					printer << "];}";

					printer << "static _getElementClass() { return ";
					emitJavascriptType(result.second);
					printer << ";}";

					printer << "}";
				}
			}

			void emitPointerDeclarations()
			{
				for (auto type : pointers)
				{
					printer << "export class ";
					emitJavascriptType(type);
					printer << " extends PtrWrapper{";

					printer << "static _getElementClass() { return ";
					emitJavascriptType(type.getUnderlying());
					printer << ";}";

					printer << "}";
				}
			}
		};

	}	 // namespace

	struct PrintJavascriptPass: impl::PrintJavascriptPassBase<PrintJavascriptPass>
	{
		using impl::PrintJavascriptPassBase<
				PrintJavascriptPass>::PrintJavascriptPassBase;

		void runOnOperation() override
		{
			JavascriptCodeGenerator codeGenerator{ this->isWasm64,
																						 this->OS,
																						 this->getOperation() };

			// TODO: Spostare tutto questo qui sotto dentro codeGenerator.generate();
			// ?

			codeGenerator.generateGeneralWrapper();
			llvm::StringMap<mlir::rlc::EnumDeclarationOp> enums;
			for (auto op : getOperation().getOps<mlir::rlc::EnumDeclarationOp>())
			{
				enums[op.getName()] = op;
			}

			for (auto t : ::rlc::postOrderTypes(this->getOperation()))
			{
				if (auto casted = mlir::dyn_cast<mlir::rlc::ClassType>(t))
				{
					if (enums.count(casted.getName()) == 0)
					{
						codeGenerator.emitClassDeclaration(casted);
					}
					else
					{
						codeGenerator.emitEnumDeclaration(casted, enums[casted.getName()]);
					}
				}
			}

			llvm::StringMap<llvm::SmallVector<mlir::FunctionType>> sortedOverloads;
			for (auto op : getOperation().getOps<mlir::rlc::FunctionOp>())
			{
				if (op.getIsMemberFunction())
					continue;

				sortedOverloads[op.getUnmangledName()].push_back(op.getType());
				if (not op.getPrecondition().empty())
					sortedOverloads["can_" + op.getUnmangledName().str()].push_back(
							mlir::FunctionType::get(
									op.getContext(),
									op.getType().getInputs(),
									{ mlir::rlc::BoolType::get(op.getContext()) }));
			}

			for (auto& pair : sortedOverloads)
			{
				codeGenerator.emitOverloadDispatcher(pair.first(), pair.second, false);
			}

			codeGenerator.emitArrayDeclarations();
			codeGenerator.emitPointerDeclarations();
			codeGenerator.emitAlternativeDeclarations();

			codeGenerator.autoIndentMyCode();
		}
	};
}	 // namespace mlir::rlc
