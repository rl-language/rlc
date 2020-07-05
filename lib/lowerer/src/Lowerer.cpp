#include "rlc/lowerer/Lowerer.hpp"

#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "rlc/ast/Entity.hpp"
#include "rlc/ast/System.hpp"
#include "rlc/ast/Type.hpp"

using namespace llvm;
using namespace rlc;
using namespace std;

llvm::Type* Lowerer::rlcBuiltinTollvmType(rlc::Type* t) const
{
	if (t->isBool())
		return llvm::IntegerType::getInt1Ty(context);
	if (t->isVoid())
		return llvm::Type::getVoidTy(context);
	if (t->isDouble())
		return llvm::Type::getDoubleTy(context);
	if (t->isLong())
		return llvm::Type::getInt64Ty(context);

	assert(false and "Unreachable");
	return nullptr;
}

llvm::Type* Lowerer::rlcFunctionTypeToLlvmType(rlc::Type* t) const
{
	assert(t->isFunctionType());
	auto returnType = rlcToLlvmType(t->getReturnType());
	SmallVector<llvm::Type*, 3> args(t->getArgCount());
	transform(t->argumentsRange(), args.begin(), [this](Type* tp) {
		return rlcToLlvmType(tp);
	});

	return llvm::FunctionType::get(returnType, args, false);
}

llvm::Type* Lowerer::uncahedRlcToLlvmType(rlc::Type* t) const
{
	assert(not t->isUserDefined());
	if (t->isBuiltin())
		return rlcBuiltinTollvmType(t);

	if (t->isFunctionType())
		return rlcFunctionTypeToLlvmType(t);

	if (t->isArray())
	{
		const auto lenght = t->getArraySize();
		auto baseType = rlcToLlvmType(t->getContainedType(0));
		return llvm::ArrayType::get(baseType, lenght);
	}

	assert(false and "unrechable");
	return nullptr;
}

llvm::Type* Lowerer::rlcToLlvmType(rlc::Type* t) const
{
	if (auto llvmT = typeToTypeMap.find(t); llvmT != typeToTypeMap.end())
		return llvmT->second;

	auto type = uncahedRlcToLlvmType(t);
	typeToTypeMap.try_emplace(t, type);
	return type;
}

Error Lowerer::lowerSystem(const System& system)
{
	modules.emplace_back(
			std::make_unique<llvm::Module>(system.getName(), context));
	Module& m = *modules.back();

	for (const auto& ent : system.entitiesRange())
		if (auto e = declareOpaqueStruct(ent.second.getEntity(), m); e)
			return e;

	for (const auto& ent : system.entitiesRange())
		if (auto e = lowerEntity(ent.second.getEntity(), m); e)
			return e;

	// for (const auto& decl : system.declarationsRange())
	// if (auto e = lowerDeclaration(decl.second, m); e)
	// return e;

	// for (const auto& decl : system.definitionsRange())
	// if (auto e = lowerDeclaration(decl.second.getDeclaration(), m); e)
	// return e;

	return Error::success();
}

Error Lowerer::declareOpaqueStruct(const Entity& entity, Module& module)
{
	auto realT = StructType::create(context, entity.getName());
	auto ptrToT = realT->getPointerTo();
	typeToTypeMap.try_emplace(entity.getType(), ptrToT);
	return Error::success();
}

Error Lowerer::lowerEntity(const Entity& entity, Module& module)
{
	llvm::Type* declaredType =
			rlcToLlvmType(entity.getType())->getContainedType(0);
	assert(declaredType->isStructTy());
	auto strctType = cast<StructType>(declaredType);

	SmallVector<llvm::Type*, 3> types(entity.fieldsCount());
	const auto& fieldToLlvmType = [this](const EntityField& field) {
		return rlcToLlvmType(field.getType());
	};
	transform(entity, types.begin(), fieldToLlvmType);
	strctType->setBody(types);

	return Error::success();
}

bool Lowerer::verify(llvm::raw_ostream& OS)
{
	const auto isBroken = [&OS](const auto& module) {
		return llvm::verifyModule(*module, &OS);
	};
	return none_of(modules, isBroken);
}
