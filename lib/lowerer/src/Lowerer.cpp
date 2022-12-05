#include "rlc/lowerer/Lowerer.hpp"

#include "llvm/ADT/DepthFirstIterator.h"
#include "llvm/ADT/STLExtras.h"
#include "mlir/Dialect/ControlFlow/IR/ControlFlowOps.h"
#include "mlir/Dialect/LLVMIR/LLVMDialect.h"
#include "mlir/IR/Verifier.h"
#include "rlc/ast/Entity.hpp"
#include "rlc/ast/System.hpp"
#include "rlc/ast/Type.hpp"
#include "rlc/dialect/Operations.hpp"

using namespace llvm;
using namespace rlc;
using namespace std;

template<typename T>
static mlir::Attribute scalarConstantToAttr(
		T value, mlir::Type type, mlir::MLIRContext* context);

template<>
mlir::Attribute scalarConstantToAttr<int64_t>(
		int64_t value, mlir::Type type, mlir::MLIRContext* context)
{
	return mlir::IntegerAttr::get(type, value);
}

template<>
mlir::Attribute scalarConstantToAttr<bool>(
		bool value, mlir::Type type, mlir::MLIRContext* context)
{
	return mlir::IntegerAttr::get(type, value);
}

template<>
mlir::Attribute scalarConstantToAttr<double>(
		double value, mlir::Type type, mlir::MLIRContext* context)
{
	return mlir::FloatAttr::get(type, value);
}

static mlir::Type getFloatType(mlir::MLIRContext* context)
{
	return mlir::rlc::FloatType::get(context);
}

static mlir::Type getBoolType(mlir::MLIRContext* context)
{
	return mlir::rlc::BoolType::get(context);
}

static mlir::Type getIntType(mlir::MLIRContext* context)
{
	return mlir::rlc::IntegerType::get(context);
}

mlir::Location rlc::sourcePositionToLocation(
		const SourcePosition& location, mlir::MLIRContext* context)
{
	if (location.getFileName().empty())
		return mlir::UnknownLoc::get(context);
	auto loc = mlir::FileLineColLoc::get(
			context,
			location.getFileName(),
			location.getLine(),
			location.getColumn());
	return loc;
}

mlir::Type Lowerer::rlcBuiltinTollvmType(rlc::Type* t) const
{
	if (t->isBool())
		return getBoolType(context);
	if (t->isVoid())
		return mlir::rlc::VoidType::get(context);
	if (t->isDouble())
		return getFloatType(context);
	if (t->isLong())
		return getIntType(context);

	assert(false and "Unreachable");
	return nullptr;
}

mlir::FunctionType Lowerer::rlcFunctionTypeToLlvmType(rlc::Type* t) const
{
	assert(t->isFunctionType());
	auto returnType = rlcToLlvmType(t->getReturnType());
	SmallVector<mlir::Type, 3> args(t->getArgCount());
	transform(t->argumentsRange(), args.begin(), [this](Type* tp) {
		return rlcToLlvmType(tp);
	});

	return mlir::FunctionType::get(context, args, { returnType });
}

mlir::Type Lowerer::uncahedRlcToLlvmType(rlc::Type* t) const
{
	assert(not t->isUserDefined());

	if (t->isArray())
	{
		const size_t lenght = t->getArraySize();
		auto baseType = rlcToLlvmType(t->getContainedType(0));
		return mlir::rlc::ArrayType::get(
				context, baseType, static_cast<long>(lenght));
	}

	if (t->isBuiltin())
		return rlcBuiltinTollvmType(t);

	if (t->isFunctionType())
		return rlcFunctionTypeToLlvmType(t);

	assert(false and "unrechable");
	return nullptr;
}

mlir::Type Lowerer::rlcToLlvmType(rlc::Type* t) const
{
	if (auto llvmT = typeToTypeMap.find(t); llvmT != typeToTypeMap.end())
		return llvmT->second;

	auto type = uncahedRlcToLlvmType(t);
	typeToTypeMap.try_emplace(t, type);
	return type;
}

static llvm::SmallVector<mlir::Value, 2> loadValues(
		mlir::OpBuilder& builder, auto pointersRange, mlir::Location position)
{
	llvm::SmallVector<mlir::Value, 2> loadedArgs;
	for (mlir::Value arg : pointersRange)
	{
		auto loaded = builder.create<mlir::LLVM::LoadOp>(position, arg);
		loadedArgs.push_back(loaded);
	}
	return loadedArgs;
}

static mlir::Value makeZero(
		mlir::Type type, mlir::OpBuilder& builder, mlir::Location position)
{
	if (type == getFloatType(builder.getContext()))
	{
		mlir::Attribute zero = mlir::FloatAttr::get(type, 0.0);
		return builder.create<mlir::LLVM::ConstantOp>(position, type, zero);
	}
	mlir::Attribute zero = mlir::IntegerAttr::get(type, 0);
	return builder.create<mlir::LLVM::ConstantOp>(position, type, zero);
}

static mlir::Value lowerArrayAccess(
		mlir::OpBuilder& builder,
		mlir::Value lPointer,
		mlir::Value rPointer,
		mlir::Location position)
{
	auto loaded = builder.create<mlir::LLVM::LoadOp>(position, rPointer);
	auto array_type = lPointer.getType();
	auto elem_type =
			array_type.cast<mlir::LLVM::LLVMArrayType>().getElementType();

	auto zero = makeZero(getIntType(builder.getContext()), builder, position);

	return builder.create<mlir::LLVM::GEPOp>(
			position, elem_type, lPointer, mlir::ValueRange({ zero, loaded }));
}

static llvm::Optional<mlir::Value> lowerBuiltinInstructionRlc(
		const FunctionDeclaration& decl,
		mlir::OpBuilder& builder,
		mlir::ValueRange args,
		mlir::Location position,
		mlir::MLIRContext* context)
{
	rlc::BuiltinFunctions FunType = stringToBuiltinFunction(decl.getName());
	switch (FunType)
	{
		case rlc::BuiltinFunctions::Not:
			return builder.create<mlir::rlc::NotOp>(position, args[0]).getResult();

		case rlc::BuiltinFunctions::Minus:
			return builder.create<mlir::rlc::MinusOp>(position, args[0]).getResult();

		case rlc::BuiltinFunctions::Init:
			builder.create<mlir::rlc::InitOp>(position, args[0]);
			return llvm::None;

		case rlc::BuiltinFunctions::Add:
			return builder.create<mlir::rlc::AddOp>(position, args[0], args[1])
					.getResult();

		case rlc::BuiltinFunctions::Subtract:
			return builder.create<mlir::rlc::SubOp>(position, args[0], args[1])
					.getResult();

		case rlc::BuiltinFunctions::Multiply:
			return builder.create<mlir::rlc::MultOp>(position, args[0], args[1])
					.getResult();

		case rlc::BuiltinFunctions::Divide:
			return builder.create<mlir::rlc::DivOp>(position, args[0], args[1])
					.getResult();

		case rlc::BuiltinFunctions::Reminder:
			return builder.create<mlir::rlc::ReminderOp>(position, args[0], args[1])
					.getResult();

		case rlc::BuiltinFunctions::And:
			return builder.create<mlir::rlc::AndOp>(position, args[0], args[1])
					.getResult();

		case rlc::BuiltinFunctions::Or:
			return builder.create<mlir::rlc::OrOp>(position, args[0], args[1])
					.getResult();

		case rlc::BuiltinFunctions::Equal:
			return builder.create<mlir::rlc::EqualOp>(position, args[0], args[1])
					.getResult();

		case rlc::BuiltinFunctions::NotEqual:
			return builder.create<mlir::rlc::NotEqualOp>(position, args[0], args[1])
					.getResult();

		case rlc::BuiltinFunctions::Greater:
			return builder.create<mlir::rlc::GreaterOp>(position, args[0], args[1])
					.getResult();

		case rlc::BuiltinFunctions::Less:
			return builder.create<mlir::rlc::LessOp>(position, args[0], args[1])
					.getResult();

		case rlc::BuiltinFunctions::LessEqual:
			return builder.create<mlir::rlc::LessEqualOp>(position, args[0], args[1])
					.getResult();

		case rlc::BuiltinFunctions::GrearEqual:
			return builder
					.create<mlir::rlc::GreaterEqualOp>(position, args[0], args[1])
					.getResult();

		case rlc::BuiltinFunctions::Bool:
			return builder
					.create<mlir::rlc::CastOp>(
							position, args[0], mlir::rlc::BoolType::get(context))
					.getResult();

		case rlc::BuiltinFunctions::Float:
			return builder
					.create<mlir::rlc::CastOp>(
							position, args[0], mlir::rlc::FloatType::get(context))
					.getResult();

		case rlc::BuiltinFunctions::Int:
			return builder
					.create<mlir::rlc::CastOp>(
							position, args[0], mlir::rlc::IntegerType::get(context))
					.getResult();

		case rlc::BuiltinFunctions::Assign:
			return builder.create<mlir::rlc::AssignOp>(position, args[0], args[1])
					.getResult();
	}

	assert(false && "unrechable");
	return builder.create<mlir::rlc::AssignOp>(position, args[0], args[1])
			.getResult();
}

void Lowerer::lowerBuiltinFunctionsRlc(
		const System& system, mlir::ModuleOp& module) const
{
	for (const auto& declaration : system.declarationsRange())
	{
		if (not declaration.second.isBuiltin())
			continue;

		const FunctionDeclaration& decl = declaration.second;
		auto position = sourcePositionToLocation(decl.getSourcePosition(), context);
		auto op = std::get<FuncType>(symbolToValue[Symbol(decl)]);

		auto* block = op.addEntryBlock();
		mlir::OpBuilder builder(block, block->begin());

		auto args = op.getArguments();
		assert(not args.empty());

		auto res =
				lowerBuiltinInstructionRlc(decl, builder, args, position, context);
		if (res.hasValue())
			builder.create<mlir::rlc::Yield>(position, mlir::ValueRange({ *res }));
		else
			builder.create<mlir::rlc::Yield>(position, mlir::ValueRange());
	}
}

Error Lowerer::lowerSystem(const System& system)
{
	auto Loc = sourcePositionToLocation(system.getPosition(), context);
	auto op = mlir::ModuleOp::create(Loc, llvm::StringRef(system.getName()));
	modules.emplace_back(op);
	mlir::ModuleOp& m = modules.back();

	SymbolTable table;
	table.insert(system);

	for (const auto& ent : system.entitiesRange())
	{
		table.insert(ent.second.getEntity());
		if (auto e = declareOpaqueStruct(ent.second.getEntity(), m); e)
			return e;
	}

	for (const auto& ent : system.entitiesRange())
		if (auto e = lowerEntity(ent.second.getEntity(), m); e)
			return e;

	for (const auto& decl : system.declarationsRange())
	{
		table.insert(decl.second);
		if (auto e = lowerDeclaration(decl.second, m); !e)
			return e.takeError();
	}

	for (const auto& decl : system.definitionsRange())
	{
		table.insert(decl->getDeclaration());
		if (auto e = lowerDeclaration(decl->getDeclaration(), m); !e)
			return e.takeError();
	}

	lowerBuiltinFunctionsRlc(system, m);

	for (const auto& decl : system.definitionsRange())
		if (auto op = lowerDefinitionToRlc(*decl, m, table); !op)
			return op.takeError();

	return Error::success();
}

Error Lowerer::declareOpaqueStruct(const Entity& entity, mlir::ModuleOp& module)
{
	if (entity.getType()->isBuiltin())
		return Error::success();
	auto realT = mlir::rlc::EntityType::getIdentified(context, entity.getName());
	typeToTypeMap.try_emplace(entity.getType(), realT);
	return Error::success();
}

Error Lowerer::lowerEntity(const Entity& entity, mlir::ModuleOp& module)
{
	if (entity.getType()->isBuiltin())
		return Error::success();
	auto declaredType = rlcToLlvmType(entity.getType());

	auto strctType = declaredType.dyn_cast<mlir::rlc::EntityType>();
	assert(!!strctType);

	SmallVector<mlir::Type, 3> types(entity.fieldsCount());
	const auto& fieldToLlvmType = [this](const EntityField& field) {
		return rlcToLlvmType(field.getType());
	};
	transform(entity, types.begin(), fieldToLlvmType);
	auto res = strctType.setBody(types);
	assert(res.succeeded());

	return Error::success();
}

bool Lowerer::verify(llvm::raw_ostream& OS)
{
	const auto isBroken = [&OS](mlir::ModuleOp& module) {
		auto res = mlir::verify(module);
		if (res.failed())
			OS << "Module Failed verify\n";
		return res.failed();
	};
	return none_of(modules, isBroken);
}

llvm::Expected<FuncType> Lowerer::lowerDeclaration(
		const FunctionDeclaration& decl, mlir::ModuleOp& module)
{
	auto t = rlcFunctionTypeToLlvmType(decl.getType());
	auto position = sourcePositionToLocation(decl.getSourcePosition(), context);
	assert(t);
	mlir::OpBuilder builder(module.getBodyRegion());

	auto op =
			builder.create<FuncType>(position, decl.getName(), decl.mangledName(), t);
	symbolToValue[Symbol(decl)] = op;
	return op;
}

static bool isTerminated(mlir::Block* block)
{
	return not block->empty() and
				 block->back().hasTrait<mlir::OpTrait::IsTerminator>();
}

static void emitReturnIfNeeded(
		mlir::OpBuilder& builder, const SourcePosition& position)
{
	if (isTerminated(builder.getBlock()))
		return;
	mlir::ValueRange range({});
	builder.create<mlir::LLVM::ReturnOp>(
			sourcePositionToLocation(position, builder.getContext()), range);
}

static void emitBranchIfNeeded(
		mlir::OpBuilder& builder,
		mlir::Block* destination,
		const SourcePosition& position)
{
	if (isTerminated(builder.getBlock()))
		return;
	mlir::ValueRange range({});
	builder.create<mlir::cf::BranchOp>(
			sourcePositionToLocation(position, builder.getContext()), destination);
}

static void emitVoidReturnIfNeeded(
		mlir::OpBuilder& builder,
		const SourcePosition& position,
		mlir::ValueRange yieldded = mlir::ValueRange())
{
	if (isTerminated(builder.getBlock()))
		return;
	builder.create<mlir::rlc::Yield>(
			sourcePositionToLocation(position, builder.getContext()), yieldded);
}

llvm::Expected<FuncType> Lowerer::lowerDefinitionToRlc(
		const FunctionDefinition& definition,
		mlir::ModuleOp& module,
		SymbolTable& table)
{
	auto op =
			std::get<FuncType>(symbolToValue[Symbol(definition.getDeclaration())]);

	auto* block = op.addEntryBlock();
	mlir::OpBuilder builder(block, block->begin());

	SymbolTable inner(&table);
	for (const auto& [argument, value] : llvm::zip(definition, op.getArguments()))
	{
		inner.insert(argument);
		symbolToValue[Symbol(argument)] = value;
	}

	if (auto error = lowerStatementToRlc(definition.getBody(), builder, inner);
			error)
		return error;

	emitVoidReturnIfNeeded(builder, definition.getSourcePosition());
	assert(op.verify().succeeded());
	return op;
}

template<>
llvm::Error Lowerer::lowerStatementToRlc<IfStatement>(
		const Statement& statement,
		mlir::OpBuilder& builder,
		SymbolTable& table) const
{
	auto& ifStatement = statement.get<IfStatement>();
	auto position = sourcePositionToLocation(statement.getPosition(), context);
	auto ifStmt = builder.create<mlir::rlc::IfStatement>(position);
	auto resumePoint = builder.saveInsertionPoint();

	builder.createBlock(&ifStmt.getCondition(), ifStmt.getCondition().begin());
	auto maybeValue =
			lowerExpressionRlc(ifStatement.getCondition(), builder, table);
	if (!maybeValue)
		return maybeValue.takeError();
	emitVoidReturnIfNeeded(builder, ifStatement.getPosition(), { *maybeValue });

	builder.createBlock(&ifStmt.getTrueBranch(), ifStmt.getTrueBranch().begin());
	if (auto maybeValue =
					lowerStatementToRlc(ifStatement.getTrueBranch(), builder, table);
			maybeValue)
		return maybeValue;
	emitVoidReturnIfNeeded(builder, ifStatement.getPosition());

	builder.createBlock(&ifStmt.getElseBranch(), ifStmt.getElseBranch().begin());
	if (ifStatement.hasFalseBranch())
	{
		auto maybeValue =
				lowerStatementToRlc(ifStatement.getFalseBranch(), builder, table);
		if (maybeValue)
			return maybeValue;
	}
	emitVoidReturnIfNeeded(builder, ifStatement.getPosition());
	builder.restoreInsertionPoint(resumePoint);
	assert(ifStmt.verify().succeeded());

	return llvm::Error::success();
}

template<>
llvm::Error Lowerer::lowerStatementToRlc<WhileStatement>(
		const Statement& statement,
		mlir::OpBuilder& builder,
		SymbolTable& table) const
{
	auto& whileStatement = statement.get<WhileStatement>();
	auto position = sourcePositionToLocation(statement.getPosition(), context);

	auto whileOp = builder.create<mlir::rlc::WhileStatement>(position);
	auto resumePoint = builder.saveInsertionPoint();

	builder.createBlock(&whileOp.getCondition(), whileOp.getCondition().begin());
	auto maybeValue =
			lowerExpressionRlc(whileStatement.getCondition(), builder, table);
	if (!maybeValue)
		return maybeValue.takeError();
	emitVoidReturnIfNeeded(
			builder, whileStatement.getPosition(), { *maybeValue });

	builder.createBlock(&whileOp.getBody(), whileOp.getBody().begin());
	if (auto maybeValue =
					lowerStatementToRlc(whileStatement.getBody(), builder, table);
			maybeValue)
		return maybeValue;
	emitVoidReturnIfNeeded(builder, whileStatement.getPosition());
	builder.restoreInsertionPoint(resumePoint);
	assert(whileOp.verify().succeeded());
	return llvm::Error::success();
}

template<>
llvm::Error Lowerer::lowerStatementToRlc<ActionStatement>(
		const Statement& statement,
		mlir::OpBuilder& builder,
		SymbolTable& table) const
{
	const auto& action = statement.get<ActionStatement>();

	auto position = sourcePositionToLocation(statement.getPosition(), context);
	llvm::SmallVector<std::string> fields;
	llvm::SmallVector<mlir::Type> types;

	for (const auto& field : action.getDeclaration())
	{
		fields.push_back(field.getName());
		types.push_back(rlcToLlvmType(field.getType()));
	}

	auto op = builder.create<mlir::rlc::ActionStatement>(
			position, types, action.getDeclaration().getName(), fields);

	for (const auto& [field, value] :
			 llvm::zip(action.getDeclaration(), op.getResults()))
	{
		symbolToValue[Symbol(field)] = value;
	}
	assert(op.verify().succeeded());

	return llvm::Error::success();
}

template<>
llvm::Error Lowerer::lowerStatementToRlc<ExpressionStatement>(
		const Statement& statement,
		mlir::OpBuilder& builder,
		SymbolTable& table) const
{
	auto expression = statement.get<ExpressionStatement>();
	auto position = sourcePositionToLocation(expression.getPosition(), context);
	auto exprOp = builder.create<mlir::rlc::ExpressionStatement>(position);
	auto resumePoint = builder.saveInsertionPoint();

	builder.createBlock(&exprOp.getBody(), exprOp.getBody().begin());
	if (auto maybeValue =
					lowerExpressionRlc(expression.getExpression(), builder, table);
			!maybeValue)
		return maybeValue.takeError();
	emitVoidReturnIfNeeded(builder, expression.getPosition());

	builder.restoreInsertionPoint(resumePoint);
	assert(exprOp.verify().succeeded());

	return llvm::Error::success();
}

template<>
llvm::Error Lowerer::lowerStatementToRlc<StatementList>(
		const Statement& statement,
		mlir::OpBuilder& builder,
		SymbolTable& table) const
{
	const auto& action = statement.get<StatementList>();

	auto position = sourcePositionToLocation(statement.getPosition(), context);
	auto listOp = builder.create<mlir::rlc::StatementList>(position);
	auto resumePoint = builder.saveInsertionPoint();

	builder.createBlock(&listOp.getBody(), listOp.getBody().begin());

	SymbolTable inner(&table);
	for (const auto& child : statement)
	{
		if (auto error = lowerStatementToRlc<Statement>(child, builder, inner);
				error)
			return error;
	}
	emitVoidReturnIfNeeded(builder, statement.getPosition());

	builder.restoreInsertionPoint(resumePoint);
	assert(listOp.verify().succeeded());
	return llvm::Error::success();
}

mlir::Value Lowerer::lowerAlloca(
		Type* type, mlir::OpBuilder& builder, const SourcePosition& position) const
{
	auto one_type = mlir::IntegerType::get(context, 64);
	auto lowered_type = rlcToLlvmType(type);
	mlir::Attribute one = mlir::IntegerAttr::get(one_type, 1);
	auto constant1 = builder.create<mlir::LLVM::ConstantOp>(
			sourcePositionToLocation(position, context), one_type, one);

	return builder.create<mlir::LLVM::AllocaOp>(
			sourcePositionToLocation(position, context), lowered_type, constant1, 0);
}

FuncType Lowerer::getAssignFunctionOf(Type* tp, SymbolTable& table) const
{
	auto* assignFunctionType = tybeDB->getFunctionType(tp, { tp, tp });
	auto* correctOverload =
			table.findOverload(BuiltinFunctions::Assign, assignFunctionType);
	assert(correctOverload != nullptr);
	return std::get<FuncType>(symbolToValue[Symbol(*correctOverload)]);
}

template<>
llvm::Error Lowerer::lowerStatementToRlc<DeclarationStatement>(
		const Statement& statement,
		mlir::OpBuilder& builder,
		SymbolTable& table) const
{
	const auto& declaration = statement.get<DeclarationStatement>();
	auto position = sourcePositionToLocation(statement.getPosition(), context);
	table.insert(declaration);

	auto resType = rlcToLlvmType(declaration.getType());
	auto decl = builder.create<mlir::rlc::DeclarationStatement>(
			position, resType, declaration.getName());
	auto resumePoint = builder.saveInsertionPoint();

	auto* block = builder.createBlock(&decl.getBody(), decl.getBody().begin());

	auto maybeValue =
			lowerExpressionRlc(declaration.getExpression(), builder, table);
	if (!maybeValue)
		return maybeValue.takeError();

	builder.create<mlir::rlc::Yield>(position, *maybeValue);

	symbolToValue[Symbol(declaration)] = decl;
	builder.restoreInsertionPoint(resumePoint);
	assert(decl.verify().succeeded());

	return llvm::Error::success();
}

template<>
llvm::Error Lowerer::lowerStatementToRlc<ReturnStatement>(
		const Statement& statement,
		mlir::OpBuilder& builder,
		SymbolTable& table) const
{
	const auto& declaration = statement.get<ReturnStatement>();
	auto position = sourcePositionToLocation(statement.getPosition(), context);

	auto returned_type = declaration.isVoid()
													 ? mlir::rlc::VoidType::get(context)
													 : rlcToLlvmType(declaration.getReturnedType());
	auto decl =
			builder.create<mlir::rlc::ReturnStatement>(position, returned_type);
	auto resumePoint = builder.saveInsertionPoint();

	auto* block = builder.createBlock(&decl.getBody(), decl.getBody().begin());

	if (not declaration.isVoid())
	{
		auto maybeValue =
				lowerExpressionRlc(declaration.getExpression(), builder, table);
		if (!maybeValue)
			return maybeValue.takeError();
		builder.create<mlir::rlc::Yield>(
				position, mlir::ValueRange({ *maybeValue }));
	}
	emitVoidReturnIfNeeded(builder, statement.getPosition());
	builder.restoreInsertionPoint(resumePoint);
	assert(decl.verify().succeeded());

	return llvm::Error::success();
}

template<>
llvm::Error Lowerer::lowerStatementToRlc<Statement>(
		const Statement& statement,
		mlir::OpBuilder& builder,
		SymbolTable& table) const
{
	return statement.visit([&]<typename T>(const T& _) {
		auto res = lowerStatementToRlc<T>(statement, builder, table);
		return res;
	});
}

template<>
llvm::Expected<mlir::Value> Lowerer::lowerExpressionRlc<MemberAccess>(
		const Expression& expression,
		mlir::OpBuilder& builder,
		SymbolTable& table) const
{
	const auto& access = expression.get<MemberAccess>();
	auto position = sourcePositionToLocation(expression.getPosition(), context);

	auto maybeValue = lowerExpressionRlc(access.getExp(), builder, table);
	if (!maybeValue)
		return maybeValue.takeError();

	auto* type = access.getExp().getType();

	int32_t index = type->indexOfSubType(access.getFieldName());
	return builder.create<mlir::rlc::MemberAccess>(position, *maybeValue, index);
}

template<>
llvm::Expected<mlir::Value> Lowerer::lowerExpressionRlc<Reference>(
		const Expression& expression,
		mlir::OpBuilder& builder,
		SymbolTable& table) const
{
	const auto& reference = expression.get<Reference>();
	assert(table.contains(reference.getName()));

	auto possible_names = table.range(reference.getName());
	assert(not possible_names.empty());

	const auto& addressabelSymbol =
			llvm::find_if(possible_names, [&](const auto& S) {
				return S.second.hasAddress() and
							 S.second.getType() == expression.getType();
			});

	assert(addressabelSymbol != possible_names.end());
	assert(symbolToValue.contains(addressabelSymbol->second));

	auto& Op = symbolToValue.find(addressabelSymbol->second)->second;

	if (std::holds_alternative<mlir::Value>(Op))
		return std::get<mlir::Value>(Op);

	auto position = sourcePositionToLocation(expression.getPosition(), context);
	return builder.create<mlir::rlc::Reference>(position, std::get<FuncType>(Op));
}

template<>
llvm::Expected<mlir::Value> Lowerer::lowerExpressionRlc<ArrayAccess>(
		const Expression& expression,
		mlir::OpBuilder& builder,
		SymbolTable& table) const
{
	const auto& access = expression.get<ArrayAccess>();
	auto lhs = lowerExpressionRlc<Expression>(access.getArray(), builder, table);
	if (!lhs)
		return lhs;

	auto rhs =
			lowerExpressionRlc<Expression>(access.getIndexing(), builder, table);
	if (!rhs)
		return rhs;

	auto position = sourcePositionToLocation(expression.getPosition(), context);
	return builder.create<mlir::rlc::ArrayAccess>(position, *lhs, *rhs);
}

template<>
llvm::Expected<mlir::Value> Lowerer::lowerExpressionRlc<ZeroInitializer>(
		const Expression& expression,
		mlir::OpBuilder& builder,
		SymbolTable& table) const
{
	const auto& constant = expression.get<ZeroInitializer>();
	auto position = sourcePositionToLocation(expression.getPosition(), context);
	const auto* overload = table.findOverload(
			BuiltinFunctions::Init,
			tybeDB->getFunctionType(tybeDB->getVoidType(), { expression.getType() }));

	auto op = std::get<FuncType>(symbolToValue[Symbol(*overload)]);
	return builder.create<mlir::rlc::ConstructOp>(position, op);
}

template<>
llvm::Expected<mlir::Value> Lowerer::lowerExpressionRlc<ScalarConstant>(
		const Expression& expression,
		mlir::OpBuilder& builder,
		SymbolTable& table) const
{
	const auto& constant = expression.get<ScalarConstant>();
	auto position = sourcePositionToLocation(expression.getPosition(), context);

	if (constant.isA<int64_t>())
		return builder.create<mlir::rlc::Constant>(position, constant.getInt());

	if (constant.isA<bool>())
		return builder.create<mlir::rlc::Constant>(position, constant.get<bool>());

	if (constant.isA<double>())
		return builder.create<mlir::rlc::Constant>(
				position, constant.get<double>());

	assert("unrechable" && false);
	return builder.create<mlir::rlc::Constant>(position, constant.get<double>());
}

template<>
llvm::Expected<mlir::Value> Lowerer::lowerExpressionRlc<Call>(
		const Expression& expression,
		mlir::OpBuilder& builder,
		SymbolTable& table) const
{
	const auto& call = expression.get<Call>();

	auto position = sourcePositionToLocation(expression.getPosition(), context);
	llvm::SmallVector<mlir::Value, 2> args;
	auto callee = lowerExpressionRlc<Expression>(
			call.getFunctionExpression(), builder, table);
	if (not callee)
		return callee.takeError();

	for (const auto& arg : call.argsRange())
	{
		auto maybeArg = lowerExpressionRlc<Expression>(arg, builder, table);
		if (not maybeArg)
			return maybeArg.takeError();
		args.push_back(*maybeArg);
	}

	if (expression.getType()->isVoid())
	{
		builder.create<mlir::rlc::CallOp>(
				position, mlir::TypeRange(), *callee, args);
		return mlir::Value();
	}

	return builder.create<mlir::rlc::CallOp>(position, *callee, args)
			.getResults()
			.front();
}

template<>
llvm::Expected<mlir::Value> Lowerer::lowerExpressionRlc<Expression>(
		const Expression& expression,
		mlir::OpBuilder& builder,
		SymbolTable& table) const
{
	return expression.visit([&]<typename T>(const T& _) {
		llvm::Expected<mlir::Value> expr =
				lowerExpressionRlc<T>(expression, builder, table);
		return expr;
	});
}
