#include "llvm/Support/CommandLine.h"
#include "llvm/Support/InitLLVM.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/Program.h"
#include "mlir/Tools/lsp-server-support/Transport.h"
#include "rlc/lsp/LSP.hpp"
#include "rlc/lsp/LSPContext.hpp"

#define DEBUG_TYPE "rlc-lsp-server"

namespace mlir::rlc::lsp
{

	class LSPServer
	{
		public:
		LSPServer(RLCServer &server): server(&server) {}
		void onInitialize(
				const mlir::lsp::InitializeParams &params,
				mlir::lsp::Callback<llvm::json::Value> reply)
		{
			// Send a response with the capabilities of this server.
			llvm::json::Object serverCaps{
				{ "textDocumentSync",
					llvm::json::Object{
							{ "openClose", true },
							{ "change", (int) mlir::lsp::TextDocumentSyncKind::Full },
							{ "save", true },
					} },
				{ "completionProvider",
					llvm::json::Object{
							{ "allCommitCharacters",
								{
										"\t",
										";",
										",",
										".",
										"=",
								} },
							{ "resolveProvider", false },
							{ "triggerCharacters",
								{ ".",
									"%",
									"^",
									"!",
									"#",
									"(",
									",",
									"<",
									":",
									"[",
									" ",
									"\"",
									"/" } },
					} },
				{ "definitionProvider", true },
				{ "referencesProvider", true },
				{ "hoverProvider", true },

				// For now we only support documenting symbols when the client
				// supports hierarchical symbols.
				{ "documentSymbolProvider",
					params.capabilities.hierarchicalDocumentSymbol },
			};

			// Per LSP, codeActionProvider can be either boolean or
			// CodeActionOptions. CodeActionOptions is only valid if the client
			// supports action literal via
			// textDocument.codeAction.codeActionLiteralSupport.
			serverCaps["codeActionProvider"] =
					params.capabilities.codeActionStructure
							? llvm::json::Object{ { "codeActionKinds",
																			{ mlir::lsp::CodeAction::kQuickFix,
																				mlir::lsp::CodeAction::kRefactor,
																				mlir::lsp::CodeAction::kInfo } } }
							: llvm::json::Value(true);

			llvm::json::Object result{
				{ { "serverInfo",
						llvm::json::Object{ { "name", "mlir-lsp-server" },
																{ "version", "0.0.0" } } },
					{ "capabilities", std::move(serverCaps) } }
			};
			reply(std::move(result));
		}

		void onInitialized(const mlir::lsp::InitializedParams &) {}

		void onShutdown(
				const mlir::lsp::NoParams &, mlir::lsp::Callback<std::nullptr_t> reply)
		{
			shutdownRequestReceived = true;
			reply(nullptr);
		}
		void onDocumentDidChange(
				const mlir::lsp::DidChangeTextDocumentParams &params)
		{
			// TODO: We currently only support full document updates, we should
			// refactor to avoid this.
			if (params.contentChanges.size() != 1)
				return;
			mlir::lsp::PublishDiagnosticsParams diagParams(
					params.textDocument.uri, params.textDocument.version);
			server->addOrUpdateDocument(
					params.textDocument.uri,
					params.contentChanges.front().text,
					params.textDocument.version,
					diagParams.diagnostics);

			// Publish any recorded diagnostics.
			publishDiagnostics(diagParams);
		}
		void onDocumentDidOpen(const mlir::lsp::DidOpenTextDocumentParams &params)
		{
			mlir::lsp::PublishDiagnosticsParams diagParams(
					params.textDocument.uri, params.textDocument.version);
			server->addOrUpdateDocument(
					params.textDocument.uri,
					params.textDocument.text,
					params.textDocument.version,
					diagParams.diagnostics);

			// Publish any recorded diagnostics.
			publishDiagnostics(diagParams);
		}
		void onDocumentDidClose(const mlir::lsp::DidCloseTextDocumentParams &params)
		{
			std::optional<int64_t> version =
					server->removeDocument(params.textDocument.uri);
			if (!version)
				return;

			// Empty out the diagnostics shown for this document. This will clear out
			// anything currently displayed by the client for this document (e.g. in
			// the "Problems" pane of VSCode).
			publishDiagnostics(mlir::lsp::PublishDiagnosticsParams(
					params.textDocument.uri, *version));
		}

		void onCompletion(
				const mlir::lsp::CompletionParams &params,
				mlir::lsp::Callback<mlir::lsp::CompletionList> reply)
		{
			reply(
					server->getCodeCompletion(params.textDocument.uri, params.position));
		}

		void onCodeAction(
				const mlir::lsp::CodeActionParams &params,
				mlir::lsp::Callback<llvm::json::Value> reply)
		{
			mlir::lsp::URIForFile uri = params.textDocument.uri;

			// Check whether a particular CodeActionKind is included in the
			// response.
			auto isKindAllowed = [only(params.context.only)](StringRef kind) {
				if (only.empty())
					return true;
				return llvm::any_of(only, [&](StringRef base) {
					return kind.consume_front(base) &&
								 (kind.empty() || kind.starts_with("."));
				});
			};

			// We provide a code action for fixes on the specified diagnostics.
			std::vector<mlir::lsp::CodeAction> actions;
			if (isKindAllowed(mlir::lsp::CodeAction::kQuickFix))
				server->getCodeActions(
						uri, params.range.start, params.context, actions);
			reply(std::move(actions));
		}

		[[nodiscard]] const RLCServer *getServer() const { return server; }

		[[nodiscard]] bool isShuttingDown() const
		{
			return shutdownRequestReceived;
		}
		void onDocumentSymbol(
				const mlir::lsp::DocumentSymbolParams &params,
				mlir::lsp::Callback<std::vector<mlir::lsp::DocumentSymbol>> reply)
		{
			std::vector<mlir::lsp::DocumentSymbol> symbols;
			server->findDocumentSymbols(params.textDocument.uri, symbols);
			reply(std::move(symbols));
		}
		void onHover(
				const mlir::lsp::TextDocumentPositionParams &params,
				mlir::lsp::Callback<std::optional<mlir::lsp::Hover>> reply)
		{
			reply(server->findHover(params.textDocument.uri, params.position));
		}
		mlir::lsp::OutgoingNotification<mlir::lsp::PublishDiagnosticsParams> &
		getPublishDiagnostic()
		{
			return publishDiagnostics;
		}

		private:
		RLCServer *server;
		mlir::lsp::OutgoingNotification<mlir::lsp::PublishDiagnosticsParams>
				publishDiagnostics;
		bool shutdownRequestReceived = false;
	};
}	 // namespace mlir::rlc::lsp

int main(int argc, char *argv[])
{
	using namespace mlir::rlc::lsp;
	llvm::cl::ParseCommandLineOptions(argc, argv);
	LSPContext context;
	llvm::InitLLVM X(argc, argv);
	auto pathToRlc = llvm::sys::fs::getMainExecutable(argv[0], (void *) &main);
	auto rlcDirectory =
			llvm::sys::path::parent_path(pathToRlc).str() + "/../lib/rlc/stdlib";
	context.addInclude(rlcDirectory);

	mlir::lsp::JSONTransport transport(
			stdin,
			llvm::outs(),
			/*inputStyle*/ mlir::lsp::JSONStreamStyle::Standard,
			/*prettyPrint*/ false);
	RLCServer server(context);
	mlir::lsp::MessageHandler handler(transport);
	LSPServer lsp(server);
	handler.method("initialize", &lsp, &LSPServer::onInitialize);
	handler.notification("initialized", &lsp, &LSPServer::onInitialized);
	handler.method("shutdown", &lsp, &LSPServer::onShutdown);
	handler.method("textDocument/codeAction", &lsp, &LSPServer::onCodeAction);
	handler.method(
			"textDocument/documentSymbol", &lsp, &LSPServer::onDocumentSymbol);
	handler.method("textDocument/hover", &lsp, &LSPServer::onHover);

	// Document Changes
	handler.notification(
			"textDocument/didOpen", &lsp, &LSPServer::onDocumentDidOpen);
	handler.notification(
			"textDocument/didClose", &lsp, &LSPServer::onDocumentDidClose);
	handler.notification(
			"textDocument/didChange", &lsp, &LSPServer::onDocumentDidChange);

	lsp.getPublishDiagnostic() =
			handler.outgoingNotification<mlir::lsp::PublishDiagnosticsParams>(
					"textDocument/publishDiagnostics");
	handler.method("textDocument/completion", &lsp, &LSPServer::onCompletion);

	mlir::LogicalResult result = mlir::success();
	if (llvm::Error error = transport.run(handler))
	{
		mlir::lsp::Logger::error("Transport error: {0}", error);
		llvm::consumeError(std::move(error));
		result = mlir::failure();
	}
	else
	{
		result = mlir::success(lsp.isShuttingDown());
	}
	return result.succeeded() ? 0 : -1;
}
