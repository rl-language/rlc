#pragma once

#include "mlir/Tools/lsp-server-support/Transport.h"
#include "rlc/lsp/LSPContext.hpp"

namespace mlir::rlc::lsp
{
	class LSPModuleInfoImpl;

	class LSPModuleInfo
	{
		public:
		explicit LSPModuleInfo(mlir::ModuleOp module, std::int64_t version);
		~LSPModuleInfo();

		LSPModuleInfo(const LSPModuleInfoImpl &other) = delete;
		LSPModuleInfo(LSPModuleInfo &&other): impl(other.impl)
		{
			other.impl = nullptr;
		}
		LSPModuleInfo &operator=(const LSPModuleInfoImpl &other) = delete;
		LSPModuleInfo &operator=(LSPModuleInfo &&other);

		mlir::ModuleOp getModule() const;
		mlir::Operation *getOperation(const mlir::lsp::Position &pos) const;
		mlir::LogicalResult getCompleteAccessMember(
				const mlir::lsp::Position &completePos,
				mlir::lsp::CompletionList &list) const;

		mlir::LogicalResult getCompleteFunction(
				const mlir::lsp::Position &completePos,
				mlir::lsp::CompletionList &list) const;

		void getLocationsOf(
				const mlir::lsp::Position &defPos,
				std::vector<mlir::lsp::Location> &locations) const;

		void findReferencesOf(
				const mlir::lsp::Position &pos,
				std::vector<mlir::lsp::Location> &references) const;

		[[nodiscard]] int64_t getVersion() const { return version; }

		private:
		LSPModuleInfoImpl *impl;
		int64_t version;
	};

	class RLCServer
	{
		public:
		explicit RLCServer(LSPContext &context): context(&context) {}
		/// Add or update the document, with the provided `version`, at the given
		/// URI. Any diagnostics emitted for this document should be added to
		/// `diagnostics`.
		void addOrUpdateDocument(
				const mlir::lsp::URIForFile &uri,
				StringRef contents,
				int64_t version,
				std::vector<mlir::lsp::Diagnostic> &diagnostics);

		/// Remove the document with the given uri. Returns the version of the
		/// removed document, or std::nullopt if the uri did not have a
		/// corresponding document within the server.
		std::optional<int64_t> removeDocument(const mlir::lsp::URIForFile &uri);

		/// Return the locations of the object pointed at by the given position.
		void getLocationsOf(
				const mlir::lsp::URIForFile &uri,
				const mlir::lsp::Position &defPos,
				std::vector<mlir::lsp::Location> &locations);

		/// Find all references of the object pointed at by the given position.
		void findReferencesOf(
				const mlir::lsp::URIForFile &uri,
				const mlir::lsp::Position &pos,
				std::vector<mlir::lsp::Location> &references);

		/// Find all of the document symbols within the given file.
		void findDocumentSymbols(
				const mlir::lsp::URIForFile &uri,
				std::vector<mlir::lsp::DocumentSymbol> &symbols);

		/// Find a hover description for the given hover position, or std::nullopt
		/// if one couldn't be found.
		std::optional<mlir::lsp::Hover> findHover(
				const mlir::lsp::URIForFile &uri, const mlir::lsp::Position &hoverPos);

		/// Get the code completion list for the position within the given file.
		mlir::lsp::CompletionList getCodeCompletion(
				const mlir::lsp::URIForFile &uri,
				const mlir::lsp::Position &completePos);

		/// Get the set of code actions within the file.
		void getCodeActions(
				const mlir::lsp::URIForFile &uri,
				const mlir::lsp::Range &pos,
				const mlir::lsp::CodeActionContext &context,
				std::vector<mlir::lsp::CodeAction> &actions)
		{
		}

		const LSPModuleInfo *getModuleFromUri(const mlir::lsp::URIForFile &uri);

		private:
		LSPContext *context;
		llvm::StringMap<LSPModuleInfo> fileToModule;
	};

}	 // namespace mlir::rlc::lsp
