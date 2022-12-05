#include "rlc/ast/System.hpp"

#include "llvm/ADT/DepthFirstIterator.h"
#include "llvm/Support/raw_ostream.h"
#include "rlc/ast/Entity.hpp"
#include "rlc/ast/EntityDeclaration.hpp"
#include "rlc/ast/FunctionDeclaration.hpp"
#include "rlc/ast/FunctionDefinition.hpp"
#include "rlc/ast/SymbolTable.hpp"
#include "rlc/ast/Type.hpp"
#include "rlc/ast/TypeUse.hpp"
#include "rlc/utils/Error.hpp"

using namespace llvm;
using namespace rlc;
using namespace std;

void System::print(llvm::raw_ostream& OS, bool printPosition) const
{
	llvm::yaml::Output Output(OS);
	Output << *const_cast<System*>(this);
}

void System::dump() const { print(outs()); }

FunctionDeclaration& System::addFunDeclaration(FunctionDeclaration fun)
{
	auto name = fun.canonicalName();
	auto result = funDecl.try_emplace(std::move(name), std::move(fun));
	return result.first->second;
}

ActionDefinition& System::addAction(ActionDefinition act)
{
	auto name = act.canonicalName();
	auto res = actions.try_emplace(std::move(name), std::move(act));
	return res.first->second;
}

FunctionDefinition& System::addFunction(FunctionDefinition fun)
{
	funDef.emplace_back(std::make_unique<FunctionDefinition>(std::move(fun)));
	return *funDef.back();
}

void System::addFunDeclaration(
		BuiltinFunctions builtin,
		std::string returnType,
		llvm::SmallVector<std::string, 3> argTypes,
		SourcePosition pos)
{
	SmallVector<ArgumentDeclaration, 3> args;
	auto u = SingleTypeUse::scalarType(std::move(returnType));
	for (auto a : argTypes)
		args.emplace_back("", SingleTypeUse::scalarType(a));

	addFunDeclaration(FunctionDeclaration(
			builtin, std::move(u), std::move(args), std::move(pos)));
}

void System::addFunDeclaration(
		std::string name,
		std::string returnType,
		llvm::SmallVector<std::string, 3> argTypes,
		SourcePosition pos)
{
	SmallVector<ArgumentDeclaration, 3> args;
	auto u = SingleTypeUse::scalarType(std::move(returnType));
	for (auto a : argTypes)
		args.emplace_back("", SingleTypeUse::scalarType(a));

	addFunDeclaration(FunctionDeclaration(
			std::move(name), std::move(u), std::move(args), std::move(pos)));
}

Error System::deduceEntitiesTypes(const SymbolTable& tb, TypeDB& db)
{
	for (auto& ep : entities)
		if (auto e = ep.second.getEntity().deduceType(tb, db); e)
			return e;

	return Error::success();
}

Error System::collectEntityTypes(SymbolTable& tb, TypeDB& db)
{
	// declare entities of actions
	for (auto& action : actions)
	{
		for (const auto& statement : llvm::depth_first(action.second))
		{
			Entity entity(action.second.getDeclaration().getEntityName());
			addEntity(EntityDeclaration(entity, action.second.getSourcePosition()));
		}
	}

	for (auto& ep : entities)
	{
		auto& entity = ep.second.getEntity();
		if (auto e = entity.createType(tb, db); e)
			return e;
	}

	return Error::success();
}

constexpr const char* SourceName = "source";
constexpr const char* DestName = "dest";

static Statement makeDefaultInitFunctionBody(
		const EntityDeclaration& entity, SymbolTable& tb, TypeDB& db)
{
	SmallVector<Statement, 3> list;
	for (const auto& field : entity.getEntity())
	{
		auto lhs = Expression::memberAccess(
				Expression::reference(DestName, entity.getSourcePosition()),
				field.getName(),
				entity.getSourcePosition());
		list.emplace_back(Statement::expStatement(
				Expression::call(
						Expression::reference(
								BuiltinFunctions::Init, entity.getSourcePosition()),
						{ std::move(lhs) },
						entity.getSourcePosition()),
				entity.getSourcePosition()));
	}

	return Statement::statmentList(std::move(list), entity.getSourcePosition());
}

static Statement makeDefaultAssigmentFunctionBody(
		const EntityDeclaration& entity, SymbolTable& tb, TypeDB& db)
{
	SmallVector<Statement, 3> list;
	for (const auto& field : entity.getEntity())
	{
		auto lhs = Expression::memberAccess(
				Expression::reference(DestName, entity.getSourcePosition()),
				field.getName(),
				entity.getSourcePosition());
		auto rhs = Expression::memberAccess(
				Expression::reference(SourceName, entity.getSourcePosition()),
				field.getName(),
				entity.getSourcePosition());
		list.emplace_back(Statement::expStatement(
				Expression::assign(
						std::move(lhs), std::move(rhs), entity.getSourcePosition()),
				entity.getSourcePosition()));
	}

	list.emplace_back(Statement::returnStatement(
			Expression::reference(DestName, entity.getSourcePosition()),
			entity.getSourcePosition()));

	return Statement::statmentList(std::move(list), entity.getSourcePosition());
}

static Statement makeDefaultAssigmentFunctionArrayBody(
		Type* type, SymbolTable& tb, TypeDB& db)
{
	assert(type->isArray());
	SmallVector<Statement, 3> whileList;
	auto pos = SourcePosition();

	std::string varName = "var";
	auto lhs = Expression::arrayAccess(
			Expression::reference(DestName), Expression::reference(varName), pos);
	auto rhs = Expression::arrayAccess(
			Expression::reference(SourceName), Expression::reference(varName), pos);
	whileList.emplace_back(Statement::expStatement(
			Expression::assign(std::move(lhs), std::move(rhs), pos), pos));

	auto incrementExpression = Expression::assign(
			Expression::reference(varName),
			Expression::reference(varName) + Expression::int64Constant(1));
	whileList.emplace_back(
			Statement::expStatement(std::move(incrementExpression)));
	auto whileBody = Statement::statmentList(whileList);

	SmallVector<Statement, 3> list;
	list.emplace_back(
			Statement::declarationStatement(varName, Expression::int64Constant(0)));

	auto condition = Expression::reference(varName) <
									 Expression::int64Constant(type->getArraySize());
	list.emplace_back(
			Statement::whileStatement(std::move(condition), std::move(whileBody)));
	list.emplace_back(
			Statement::returnStatement(Expression::reference(DestName, pos), pos));

	return Statement::statmentList(std::move(list), pos);
}

static Statement makeDefaultInitFunctionArrayBody(
		Type* type, SymbolTable& tb, TypeDB& db)
{
	assert(type->isArray());
	SmallVector<Statement, 3> whileList;
	auto pos = SourcePosition();

	std::string varName = "var";
	auto lhs = Expression::arrayAccess(
			Expression::reference(DestName), Expression::reference(varName), pos);
	whileList.emplace_back(Statement::expStatement(
			Expression::call(
					Expression::reference(BuiltinFunctions::Init, pos),
					{ std::move(lhs) },
					pos),
			pos));

	auto incrementExpression = Expression::assign(
			Expression::reference(varName),
			Expression::reference(varName) + Expression::int64Constant(1));
	whileList.emplace_back(
			Statement::expStatement(std::move(incrementExpression)));
	auto whileBody = Statement::statmentList(whileList);

	SmallVector<Statement, 3> list;
	list.emplace_back(
			Statement::declarationStatement(varName, Expression::int64Constant(0)));

	auto condition = Expression::reference(varName) <
									 Expression::int64Constant(type->getArraySize());
	list.emplace_back(
			Statement::whileStatement(std::move(condition), std::move(whileBody)));

	return Statement::statmentList(std::move(list), pos);
}

Error System::defineImplicitInitArrayFunction(
		Type* type, SymbolTable& tb, TypeDB& db)
{
	auto* expectedType = db.getFunctionType(db.getVoidType(), { type });
	const auto* overload = tb.findOverload(BuiltinFunctions::Init, expectedType);
	if (overload != nullptr)
		return make_error<RlcError>(
				"found user defined overload of init of a array type, it is forbidden",
				RlcErrorCategory::errorCode(RlcErrorCode::argumentTypeMissmatch),
				overload->getSourcePosition());

	ArgumentDeclaration destination(
			DestName, SingleTypeUse(type), SourcePosition());

	auto body = makeDefaultInitFunctionArrayBody(type, tb, db);

	FunctionDefinition decl(
			BuiltinFunctions::Init,
			std::move(body),
			SingleTypeUse(db.getVoidType()),
			{ std::move(destination) },
			SourcePosition());

	auto& added = addFunction(std::move(decl));
	tb.insert(added.getDeclaration());

	if (auto error = added.getDeclaration().deduceType(tb, db); error)
		return error;

	return added.deduceTypes(tb, db);
}

Error System::defineImplicitAssigmentArrayFunction(
		Type* type, SymbolTable& tb, TypeDB& db)
{
	auto* expectedType = db.getFunctionType(type, { type, type });
	const auto* overload =
			tb.findOverload(BuiltinFunctions::Assign, expectedType);
	if (overload != nullptr)
		return make_error<RlcError>(
				"found user defined overload of array type, it is forbidden",
				RlcErrorCategory::errorCode(RlcErrorCode::argumentTypeMissmatch),
				overload->getSourcePosition());

	ArgumentDeclaration destination(
			DestName, SingleTypeUse(type), SourcePosition());
	ArgumentDeclaration source(SourceName, SingleTypeUse(type), SourcePosition());

	auto body = makeDefaultAssigmentFunctionArrayBody(type, tb, db);

	FunctionDefinition decl(
			BuiltinFunctions::Assign,
			std::move(body),
			SingleTypeUse(type),
			{ std::move(destination), std::move(source) },
			SourcePosition());

	auto& added = addFunction(std::move(decl));
	tb.insert(added.getDeclaration());

	if (auto error = added.getDeclaration().deduceType(tb, db); error)
		return error;

	return added.deduceTypes(tb, db);
}

Error System::defineImplicitInitFunction(
		const EntityDeclaration& entity, SymbolTable& tb, TypeDB& db)
{
	auto* entityType = entity.getEntity().getType();
	auto* expectedType = db.getFunctionType(db.getVoidType(), { entityType });
	// if the user has already defined it, do nothing
	if (tb.findOverload(BuiltinFunctions::Init, expectedType) != nullptr)
		return llvm::Error::success();

	auto type = SingleTypeUse(entityType);
	ArgumentDeclaration destination(DestName, type, entity.getSourcePosition());

	auto body = makeDefaultInitFunctionBody(entity, tb, db);
	FunctionDefinition decl(
			BuiltinFunctions::Init,
			std::move(body),
			SingleTypeUse(db.getVoidType()),
			{ std::move(destination) },
			entity.getSourcePosition());

	auto& added = addFunction(std::move(decl));
	tb.insert(added.getDeclaration());
	return added.getDeclaration().deduceType(tb, db);
}

Error System::defineImplicitAssigmentFunction(
		const EntityDeclaration& entity, SymbolTable& tb, TypeDB& db)
{
	auto* entityType = entity.getEntity().getType();
	assert(entityType != nullptr);
	auto* expectedType =
			db.getFunctionType(entityType, { entityType, entityType });
	// if the user has already defined it, do nothing
	if (tb.findOverload(BuiltinFunctions::Assign, expectedType) != nullptr)
		return llvm::Error::success();

	auto type = SingleTypeUse::scalarType(entityType->getName());
	ArgumentDeclaration destination(DestName, type, entity.getSourcePosition());
	ArgumentDeclaration source(SourceName, type, entity.getSourcePosition());
	auto body = makeDefaultAssigmentFunctionBody(entity, tb, db);
	FunctionDefinition decl(
			BuiltinFunctions::Assign,
			std::move(body),
			type,
			{ std::move(destination), std::move(source) },
			entity.getSourcePosition());

	auto& added = addFunction(std::move(decl));
	tb.insert(added.getDeclaration());

	return added.getDeclaration().deduceType(tb, db);
}

llvm::Error System::deduceImplicitArrayFunctions(SymbolTable& tb, TypeDB& db)
{
	for (auto* type : db.arrayTypesRange())
	{
		if (auto error = defineImplicitAssigmentArrayFunction(type, tb, db); error)
			return error;

		if (auto error = defineImplicitInitArrayFunction(type, tb, db); error)
			return error;
	}
	return llvm::Error::success();
}

Error System::deduceImplicitFunctions(SymbolTable& tb, TypeDB& db)
{
	for (const auto& entity : entities)
	{
		if (entity.second.getEntity().getType()->isVoid())
			continue;

		if (auto error = defineImplicitAssigmentFunction(entity.second, tb, db);
				error)
			return error;

		if (auto error = defineImplicitInitFunction(entity.second, tb, db); error)
			return error;
	}
	return llvm::Error::success();
}

Error System::deduceFunctionDeclarationType(SymbolTable& tb, TypeDB& db)
{
	for (const auto& fDecl : funDecl)
		tb.insert(fDecl.second);

	for (const auto& fDef : funDef)
		tb.insert(fDef->getDeclaration());

	for (auto& fDecl : funDecl)
		if (auto e = fDecl.second.deduceType(tb, db); e)
			return e;

	for (auto& fDef : funDef)
		if (auto e = fDef->getDeclaration().deduceType(tb, db); e)
			return e;

	return Error::success();
}

Error System::deduceFunctionTypes(SymbolTable& tb, TypeDB& db)
{
	for (auto& fDef : funDef)
		if (auto e = fDef->deduceTypes(tb, db); e)
			return e;

	if (auto e = deduceImplicitArrayFunctions(tb, db); e)
		return e;

	return Error::success();
}

llvm::Error System::deduceCreatorActionFunction(
		const ActionDefinition& outermost, SymbolTable& tb, TypeDB& db)
{
	const auto& decl = outermost.getDeclaration();
	llvm::SmallVector<ArgumentDeclaration, 3> arguments;
	llvm::SmallVector<Statement, 3> assignStatement;

	auto declaration = Statement::declarationStatement(
			"arg0",
			Expression::zeroInitializer(
					SingleTypeUse::scalarType(outermost.getDeclaration().getEntityName()),
					decl.getSourcePosition()),
			decl.getSourcePosition());
	assignStatement.push_back(std::move(declaration));
	for (const auto& field : decl)
	{
		assignStatement.emplace_back(Statement::expStatement(
				Expression::assign(
						Expression::memberAccess(
								Expression::reference("arg0", decl.getSourcePosition()),
								field.getName(),
								decl.getSourcePosition()),
						Expression::reference(field.getName(), decl.getSourcePosition()),
						decl.getSourcePosition()),
				decl.getSourcePosition()));
		arguments.emplace_back(
				field.getName(), field.getTypeUse(), decl.getSourcePosition());
	}

	auto call = Expression::call(
			Expression::reference(
					outermost.implementatioName(), outermost.getSourcePosition()),
			{ Expression::reference("arg0", decl.getSourcePosition()) },
			outermost.getSourcePosition());
	assignStatement.emplace_back(
			Statement::expStatement(std::move(call), decl.getSourcePosition()));

	auto returnStatement = Statement::returnStatement(
			Expression::reference("arg0", decl.getSourcePosition()),
			decl.getSourcePosition());
	assignStatement.emplace_back(std::move(returnStatement));

	auto signature = FunctionDefinition(
			decl.getName(),
			Statement::statmentList(
					std::move(assignStatement), decl.getSourcePosition()),
			SingleTypeUse::scalarType(decl.getEntityName()),
			arguments,
			decl.getSourcePosition());

	auto& fun = addFunction(std::move(signature));

	tb.insert(fun.getDeclaration());
	if (auto error = fun.getDeclaration().deduceType(tb, db); error)
		return error;
	return llvm::Error::success();
}

llvm::Error System::deduceWrapperActionFunction(
		const ActionDefinition& outermost,
		const ActionDeclaration& decl,
		SymbolTable& tb,
		TypeDB& db)
{
	llvm::SmallVector<ArgumentDeclaration, 3> arguments;
	arguments.emplace_back(
			"arg0",
			SingleTypeUse::scalarType(outermost.getDeclaration().getEntityName()),
			decl.getSourcePosition());
	llvm::SmallVector<Statement, 3> assignStatement;
	for (const auto& field : decl)
	{
		assignStatement.emplace_back(Statement::expStatement(
				Expression::assign(
						Expression::memberAccess(
								Expression::reference("arg0", decl.getSourcePosition()),
								field.getName(),
								decl.getSourcePosition()),
						Expression::reference(field.getName(), decl.getSourcePosition()),
						decl.getSourcePosition()),
				decl.getSourcePosition()));
		arguments.emplace_back(
				field.getName(), field.getTypeUse(), decl.getSourcePosition());
	}

	auto call = Expression::call(
			Expression::reference(
					outermost.implementatioName(), outermost.getSourcePosition()),
			{ Expression::reference("arg0", decl.getSourcePosition()) },
			outermost.getSourcePosition());
	assignStatement.emplace_back(
			Statement::expStatement(std::move(call), decl.getSourcePosition()));

	auto signature = FunctionDefinition(
			decl.getName(),
			Statement::statmentList(
					std::move(assignStatement), decl.getSourcePosition()),
			SingleTypeUse::scalarType(builtinTypeToString(BuiltinType::VOID).str()),
			arguments,
			decl.getSourcePosition());

	auto& fun = addFunction(std::move(signature));

	tb.insert(fun.getDeclaration());
	if (auto error = fun.getDeclaration().deduceType(tb, db); error)
		return error;
	return llvm::Error::success();
}

llvm::Error System::deduceActionsFunctions(SymbolTable& tb, TypeDB& db)
{
	for (auto& action : actions)
	{
		llvm::SmallVector<ActionDeclaration*, 4> decls;
		FunctionDefinition& fun = addFunction(action.second.asFunction(tb));
		tb.insert(fun.getDeclaration());
		if (auto error = fun.getDeclaration().deduceType(tb, db); error)
			return error;

		if (auto error = deduceCreatorActionFunction(action.second, tb, db))
			return error;

		auto toGenerate = action.second.allInnerActions();
		for (const auto* decl : toGenerate)
		{
			if (auto error =
							deduceWrapperActionFunction(action.second, *decl, tb, db))
				return error;
		}
	}

	return llvm::Error::success();
}

llvm::Error System::deduceActionsEntities(SymbolTable& tb, TypeDB& db)
{
	// ToDo mutually recursive actions do not work
	for (auto& action : actions)
	{
		if (auto error = action.second.getDeclaration().deduceType(tb, db); error)
			return error;
		if (auto error = action.second.deduceTypes(tb, db); error)
			return error;
		SmallVector<EntityField, 3> fields;

		for (auto& argument : action.second.getDeclaration())
		{
			fields.emplace_back(
					SingleTypeUse(argument.getType()),
					argument.getName(),
					argument.getSourcePosition());
		}

		for (const auto& statement : llvm::depth_first(action.second))
		{
			if (not statement.statement->isDeclarationStatement())
				continue;

			const auto& declaration =
					statement.statement->get<DeclarationStatement>();

			if (declaration.isPrivate())
				continue;

			fields.emplace_back(
					SingleTypeUse(declaration.getType()),
					declaration.getName(),
					statement.statement->getPosition());
		}

		for (const auto& statement : llvm::depth_first(action.second))
		{
			if (not statement.statement->isActionStatement())
				continue;

			const auto& declaration = statement.statement->get<ActionStatement>();

			for (const auto& arg : declaration.getDeclaration())
				fields.emplace_back(
						SingleTypeUse(arg.getType()),
						arg.getName(),
						arg.getSourcePosition());
		}

		fields.insert(
				fields.begin(),
				EntityField(
						SingleTypeUse::scalarType(
								builtinTypeToString(BuiltinType::LONG).str()),
						"resume_point",
						action.second.getSourcePosition()));

		Entity& entity =
				entities.find(action.second.getDeclaration().getEntityName())
						->second.getEntity();
		entity.addField(fields);
		if (auto error = entity.deduceType(tb, db); error)
			return error;
	}

	return llvm::Error::success();
}

Error System::typeCheck(const SymbolTable& tb, TypeDB& db)
{
	SymbolTable table(&tb);
	if (auto e = collectEntityTypes(table, db); e)
		return e;

	if (auto e = deduceEntitiesTypes(table, db); e)
		return e;

	if (auto e = deduceFunctionDeclarationType(table, db); e)
		return e;

	if (auto e = deduceActionsEntities(table, db); e)
		return e;

	if (auto e = deduceImplicitFunctions(table, db); e)
		return e;

	if (auto e = deduceActionsFunctions(table, db); e)
		return e;

	if (auto e = deduceFunctionTypes(table, db); e)
		return e;

	return Error::success();
}
