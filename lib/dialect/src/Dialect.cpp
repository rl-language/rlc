
#include "rlc/dialect/Dialect.h"

#include "Dialect.inc"
#include "rlc/dialect/Types.hpp"

class TypeAliasASMInterface: public mlir::OpAsmDialectInterface
{
	public:
	using mlir::OpAsmDialectInterface::OpAsmDialectInterface;

	AliasResult getAlias(mlir::Type type, llvm::raw_ostream& OS) const final
	{
		if (auto casted = type.dyn_cast<mlir::rlc::EntityType>())
		{
			OS << casted.getName();
			return AliasResult::FinalAlias;
		}
		if (auto casted = type.dyn_cast<mlir::rlc::TraitMetaType>())
		{
			OS << "trait_" << casted.getName();
			return AliasResult::FinalAlias;
		}

		return AliasResult::NoAlias;
	}
};

void mlir::rlc::RLCDialect::initialize()
{
	registerTypes();
	registerOperations();
	addInterfaces<TypeAliasASMInterface>();
}
