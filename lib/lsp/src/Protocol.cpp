#include "rlc/lsp/Protocol.hpp"

bool mlir::rlc::lsp::fromJSON(
		const llvm::json::Value &value,
		mlir::rlc::lsp::RenameParams &result,
		llvm::json::Path path)
{
	llvm::json::ObjectMapper o(value, path);
	if (!o)
		return false;
	// We deliberately don't fail if we can't parse individual fields.
	return o.map("textDocument", result.textDocument) and
				 o.map("newName", result.newName) and
				 o.map("position", result.position);
}
