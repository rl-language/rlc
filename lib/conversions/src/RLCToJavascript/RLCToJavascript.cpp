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
			mlir::rlc::ModuleBuilder builder;
			mlir::ModuleOp moduleOp;

			llvm::DenseSet<mlir::rlc::ArrayType> arrays{};
			llvm::DenseSet<mlir::rlc::AlternativeType> alternatives{};
			llvm::DenseSet<mlir::rlc::OwningPtrType> pointers{};

			llvm::StringMap<mlir::rlc::EnumDeclarationOp> enums{};

			struct SizeAlignment
			{
				int size{};
				int alignment{};
			};

			struct SizeAlignmentOffsets
			{
				int size{};
				int alignment{};
				llvm::SmallVector<int, 4> offsets{};
			};

			struct ArrayTypeAndDimensions
			{
				mlir::Type type{};
				llvm::SmallVector<int, 2> dimensions{};
			};

			public:
			JavascriptCodeGenerator(
					bool isWasm64, llvm::raw_ostream* OS, mlir::ModuleOp moduleOp)
					: isWasm64(isWasm64),
						OS(OS),
						table(MemberFunctionsTable{ moduleOp }),
						builder(ModuleBuilder{ moduleOp }),
						moduleOp(moduleOp)
			{
			}

			void generateCode()
			{
				emitGeneralWrapper();
				emitClassesAndEnums();
				emitFreeFunctions();
				emitArrayDeclarations();
				emitPointerDeclarations();
				emitAlternativeDeclarations();

				autoIndentMyCode();
			}

			private:
			void emitGeneralWrapper()
			{
				printer << standardJavascriptWrapper
								<< "const pointerSize = " << (isWasm64 ? 8 : 4) << ";";
			}

			void emitClassesAndEnums()
			{
				for (auto op : moduleOp.getOps<mlir::rlc::EnumDeclarationOp>())
				{
					enums[op.getName()] = op;
				}

				for (auto t : ::rlc::postOrderTypes(moduleOp))
				{
					if (auto casted = mlir::dyn_cast<mlir::rlc::ClassType>(t))
					{
						if (enums.count(casted.getName()) == 0)
						{
							emitClassDeclaration(casted);
						}
						else
						{
							emitEnumDeclaration(casted, enums[casted.getName()]);
						}
					}
				}
			}

			void emitFreeFunctions()
			{
				llvm::StringMap<llvm::SmallVector<mlir::FunctionType>> sortedOverloads;
				for (auto op : moduleOp.getOps<mlir::rlc::FunctionOp>())
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

				for (auto op : moduleOp.getOps<mlir::rlc::ActionFunction>())
				{
					if (op.getIsMemberFunction())
						continue;

					sortedOverloads[op.getUnmangledName()].push_back(
							op.getMainActionType());
					if (not op.getPrecondition().empty())
						sortedOverloads["can_" + op.getUnmangledName().str()].push_back(
								mlir::FunctionType::get(
										op.getContext(),
										op.getMainActionType().getInputs(),
										{ mlir::rlc::BoolType::get(op.getContext()) }));
				}

				for (auto& pair : sortedOverloads)
				{
					emitOverloadDispatcher(pair.first(), pair.second, false);
				}
			}

			void emitArrayDeclarations()
			{
				for (auto type : arrays)
				{
					printer << "export class ";
					emitJavascriptType(type);
					printer << " extends ArrayWrapper{";

					ArrayTypeAndDimensions result{ getTypeAndDimensionsOfArray(type) };

					printer << "static _getDimensions(){ return [";
					for (int x : result.dimensions)
					{
						printer << x << ", ";
					}
					printer << "];}";

					printer << "static _getElementClass() { return ";
					emitJavascriptType(result.type);
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

			void emitAlternativeDeclarations()
			{
				for (auto type : alternatives)
				{
					printer << "export class ";
					emitJavascriptType(type);
					printer << " extends AlternativeWrapper{";

					const auto result = getSizeAlignmentAndOffsetsOfAlternative(type);
					printer << "static _getSize() { return " << result.size << ";}";

					printer << "static _getIndexOffset(){ return " << result.offsets[1]
									<< ";}";

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

			/////////////////////////////////////////////////////////////////////////
			/////////////////////////////////////////////////////////////////////////
			/////////////////////////////////////////////////////////////////////////
			/////////////////////////////////////////////////////////////////////////
			/////////////////////////////////////////////////////////////////////////
			/////////////////////////////////////////////////////////////////////////
			/////////////////////////////////////////////////////////////////////////
			/////////////////////////////////////////////////////////////////////////
			/////////////////////////////////////////////////////////////////////////

			void emitOverloadDispatcher(
					llvm::StringRef name,
					llvm::ArrayRef<mlir::FunctionType> overloads,
					bool isMethod)
			{
				if (isPrivate(name))
				{
					return;
				}

				if (isMethod == false)
				{
					printer << "export function ";
				}

				emitMangledFunctionName(name);
				printer << "(...args){";

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
					printer << "static ";
					emitMangledVariableName(value.value().getName());
					printer << " = " << i << ";";
					i++;
				}

				emitMemberFunctions(type);
				printer << "}";
			}

			void emitClassDeclaration(mlir::rlc::ClassType type)
			{
				printer << "export class ";
				emitJavascriptType(type);
				printer << " extends ClassWrapper{";

				printer << "static _getSize() { return ";
				SizeAlignmentOffsets result = getSizeAlignmentAndOffsetsOfStruct(type);
				printer << result.size << ";}";
				printer << "static _getMemberFieldNames(){ return [";
				for (auto name : type.getMemberNames())
				{
					if (isPrivate(name))
					{
						continue;
					}

					printer << "\"";
					emitMangledVariableName(name);
					printer << "\", ";
				}

				printer << "]; }";
				emitGettersAndSetters(type, result.offsets);
				emitMemberFunctions(type);

				if (builder.isClassOfAction(type))
				{
					emitActionFunctions(type);
				}

				printer << "}";
			}

			bool isPrivate(llvm::StringRef str) { return str[0] == '_'; }

			void emitGettersAndSetters(
					mlir::rlc::ClassType classType, llvm::SmallVector<int, 4> offsets)
			{
				for (auto [type, name, offset] : llvm::zip(
								 classType.getMemberTypes(),
								 classType.getMemberNames(),
								 offsets))
				{
					if (isPrivate(name))
					{
						continue;
					}

					printer << "get ";
					emitMangledVariableName(name);
					printer << "(){return this._get(";
					emitJavascriptType(type);
					printer << ", " << offset << ");}";

					printer << "set ";
					emitMangledVariableName(name);
					printer << "(value){this._set(";
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

					if (not memberFunction.getPrecondition().empty())
					{
						sortedOverloads["can_" + memberFunction.getUnmangledName().str()]
								.push_back(
										mlir::FunctionType::get(
												memberFunction.getContext(),
												memberFunction.getType().getInputs(),
												{ mlir::rlc::BoolType::get(
														memberFunction.getContext()) }));
					}
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

			void emitActionFunctions(mlir::rlc::ClassType type)
			{
				auto action = mlir::cast<mlir::rlc::ActionFunction>(
						builder.getActionOf(type).getDefiningOp());

				for (auto value : action.getActions())
				{
					mlir::Operation* statement =
							builder.actionFunctionValueToActionStatement(value).front();
					auto actionStatement =
							mlir::cast<mlir::rlc::ActionStatement>(statement);
					emitSingleActionFunction(
							action.getClassType(),
							actionStatement.getName(),
							actionStatement.getResultTypes(),
							mlir::rlc::VoidType::get(action.getContext()));
					emitSingleActionFunction(
							action.getClassType(),
							("can_" + actionStatement.getName()).str(),
							actionStatement.getResultTypes(),
							mlir::rlc::BoolType::get(action.getContext()));
				}

				emitSingleActionFunction(
						action.getClassType(),
						"is_done",
						{},
						mlir::rlc::BoolType::get(action.getContext()));
			}

			void emitSingleActionFunction(
					mlir::rlc::ClassType frameType,
					llvm::StringRef actionName,
					mlir::TypeRange argTypes,
					mlir::Type resultType)
			{
				llvm::SmallVector<mlir::Type> args = { frameType };
				for (auto arg : argTypes)
					args.push_back(arg);
				auto fType = mlir::FunctionType::get(
						resultType.getContext(), args, { resultType });

				llvm::SmallVector<mlir::FunctionType> overloads = { fType };
				emitOverloadDispatcher(actionName, overloads, true);
			}

			/////////////////////////////////////////////////////////////////////////
			/////////////////////////////////////////////////////////////////////////
			/////////////////////////////////////////////////////////////////////////
			/////////////////////////////////////////////////////////////////////////
			/////////////////////////////////////////////////////////////////////////
			/////////////////////////////////////////////////////////////////////////
			/////////////////////////////////////////////////////////////////////////
			/////////////////////////////////////////////////////////////////////////
			/////////////////////////////////////////////////////////////////////////

			void emitMangledFunctionName(llvm::StringRef name)
			{
				printer << "f_" << name;
			}

			void emitMangledVariableName(llvm::StringRef name)
			{
				printer << "v_" << name;
			}

			void emitMangledClassName(mlir::rlc::ClassType type)
			{
				const auto templateParameters = type.getExplicitTemplateParameters();
				if (templateParameters.size() == 0)
				{
					printer << "Cls_" << type.getName();
				}
				else
				{
					printer << "ClsT_" << type.getName() << "$";
					bool comma = false;
					for (const auto t : templateParameters)
					{
						if (comma)
						{
							printer << "_";
						}
						comma = true;
						emitJavascriptType(t);
					}
					printer << "$";
				}
			}

			void emitMangledEnumName(mlir::rlc::ClassType type)
			{
				printer << "Enum_" << type.getName();
			}

			void emitMangledActionFunctionName(mlir::rlc::ClassType type)
			{
				printer << "Act_" << type.getName();
			}

			void emitMangledAlternativeName(mlir::rlc::AlternativeType type)
			{
				printer << "Alt" << type.getUnderlying().size();
				for (auto enumeration : llvm::enumerate(type.getUnderlying()))
				{
					printer << "_";
					emitJavascriptType(enumeration.value());
				}
			}

			void emitMangledArrayName(mlir::rlc::ArrayType type)
			{
				printer << "Arr_";

				ArrayTypeAndDimensions result{ getTypeAndDimensionsOfArray(type) };

				for (int x : result.dimensions)
				{
					printer << x << "_";
				}

				emitJavascriptType(result.type);
			}

			void emitMangledPointerName(mlir::rlc::OwningPtrType type)
			{
				printer << "Ptr_";
				emitJavascriptType(type.getUnderlying());
			}

			/////////////////////////////////////////////////////////////////////////
			/////////////////////////////////////////////////////////////////////////
			/////////////////////////////////////////////////////////////////////////
			/////////////////////////////////////////////////////////////////////////
			/////////////////////////////////////////////////////////////////////////
			/////////////////////////////////////////////////////////////////////////
			/////////////////////////////////////////////////////////////////////////
			/////////////////////////////////////////////////////////////////////////
			/////////////////////////////////////////////////////////////////////////

			ArrayTypeAndDimensions getTypeAndDimensionsOfArray(
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
						return { currentType.getUnderlying(), sizes };
					}
				}
			}

			SizeAlignment getSizeAndAlignmentByType(mlir::Type type)
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
					const SizeAlignmentOffsets result =
							getSizeAlignmentAndOffsetsOfStruct(casted);
					return { result.size, result.alignment };
				}

				if (auto casted = mlir::dyn_cast<mlir::rlc::AlternativeType>(type))
				{
					const auto result = getSizeAlignmentAndOffsetsOfAlternative(casted);
					return { result.size, result.alignment };
				}

				if (auto casted = mlir::dyn_cast<mlir::rlc::ArrayType>(type))
				{
					ArrayTypeAndDimensions result{ getTypeAndDimensionsOfArray(casted) };

					int linearLength = 1;
					for (int x : result.dimensions)
					{
						linearLength *= x;
					}

					SizeAlignment underlyingSizeAndAlignment =
							getSizeAndAlignmentByType(result.type);

					return { linearLength * underlyingSizeAndAlignment.size,
									 underlyingSizeAndAlignment.alignment };
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

			int correctSizeWithAlignment(int size, int alignment)
			{
				const int remainder{ size % alignment };
				if (remainder != 0)
				{
					size += alignment - remainder;
				}
				return size;
			}

			SizeAlignment getSizeAndAlignmentOfUnion(mlir::rlc::AlternativeType type)
			{
				int maxSize = -1;
				int maxAlignment = -1;

				for (auto enumeration : llvm::enumerate(type.getUnderlying()))
				{
					SizeAlignment currentSizeAndAlignment =
							getSizeAndAlignmentByType(enumeration.value());

					if (currentSizeAndAlignment.size > maxSize)
					{
						maxSize = currentSizeAndAlignment.size;
					}

					if (currentSizeAndAlignment.alignment > maxAlignment)
					{
						maxAlignment = currentSizeAndAlignment.alignment;
					}
				}

				maxSize = correctSizeWithAlignment(maxSize, maxAlignment);

				return { maxSize, maxAlignment };
			}

			SizeAlignmentOffsets getSizeAlignmentAndOffsetsOfAlternative(
					mlir::rlc::AlternativeType type)
			{
				llvm::SmallVector<SizeAlignment, 4> structFieldData{};
				structFieldData.push_back(getSizeAndAlignmentOfUnion(type));
				structFieldData.push_back(getSizeAndAlignmentByType(
						mlir::rlc::IntegerType::get(type.getContext(), 64)));

				return getSizeAlignmentAndOffsetsOfStruct(structFieldData);
			}

			SizeAlignmentOffsets getSizeAlignmentAndOffsetsOfStruct(
					mlir::rlc::ClassType classType

			)
			{
				llvm::SmallVector<SizeAlignment, 4> output{};
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

			SizeAlignmentOffsets getSizeAlignmentAndOffsetsOfStruct(
					llvm::SmallVector<SizeAlignment, 4> structFieldData)
			{
				llvm::SmallVector<int, 4> offsets{};

				int structSize = 0;
				int maxAlignment = -1;

				for (size_t i{ 0 }; i < structFieldData.size(); i++)
				{
					const int memberSize = structFieldData[i].size;
					const int memberAlignment = structFieldData[i].alignment;

					structSize = correctSizeWithAlignment(structSize, memberAlignment);
					offsets.push_back(structSize);
					structSize += memberSize;

					if (memberAlignment > maxAlignment)
					{
						maxAlignment = memberAlignment;
					}
				}
				structSize = correctSizeWithAlignment(structSize, maxAlignment);

				return { structSize, maxAlignment, offsets };
			}

			void emitJavascriptType(mlir::Type type)
			{
				if (auto casted = mlir::dyn_cast<mlir::rlc::IntegerType>(type))
				{
					if (casted.getSize() == 64)
					{
						printer << "Int";
					}
					else
					{
						printer << "Byte";
					}
				}
				else if (mlir::isa<mlir::rlc::FloatType>(type))
				{
					printer << "Float";
				}
				else if (mlir::isa<mlir::rlc::BoolType>(type))
				{
					printer << "Bool";
				}
				else if (mlir::isa<mlir::rlc::StringLiteralType>(type))
				{
					printer << "StringLiteral";
				}
				else if (mlir::isa<mlir::rlc::VoidType>(type))
				{
					printer << "null";
				}
				else if (auto casted = mlir::dyn_cast<mlir::rlc::ClassType>(type))
				{
					if (enums.count(casted.getName()) == 0)
					{
						if (builder.isClassOfAction(casted))
						{
							emitMangledActionFunctionName(casted);
						}
						else
						{
							emitMangledClassName(casted);
						}
					}
					else
					{
						emitMangledEnumName(casted);
					}
				}
				else if (auto casted = mlir::dyn_cast<mlir::rlc::AlternativeType>(type))
				{
					alternatives.insert(casted);
					emitMangledAlternativeName(casted);
				}
				else if (auto casted = mlir::dyn_cast<mlir::rlc::ArrayType>(type))
				{
					arrays.insert(casted);
					emitMangledArrayName(casted);
				}
				else if (auto casted = mlir::dyn_cast<mlir::rlc::OwningPtrType>(type))
				{
					pointers.insert(casted);
					emitMangledPointerName(casted);
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
				else if (auto casted = mlir::dyn_cast<mlir::rlc::ContextType>(type))
				{
					emitJavascriptType(casted.getUnderlying());
				}
				else if (auto casted = mlir::dyn_cast<mlir::rlc::FrameType>(type))
				{
					emitJavascriptType(casted.getUnderlying());
				}
				else
				{
					std::cerr << "Can't emit an equivalent Javascript type for ";
					type.dump();
					std::abort();
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
			codeGenerator.generateCode();
		}
	};
}	 // namespace mlir::rlc
