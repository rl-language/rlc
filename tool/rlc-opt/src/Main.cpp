/*
Copyright 2024 Massimo Fioravanti

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
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Error.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"
#include "mlir/InitAllDialects.h"
#include "mlir/Tools/mlir-opt/MlirOptMain.h"
#include "rlc/dialect/Dialect.h"
#include "rlc/dialect/Passes.hpp"
#include "rlc/parser/Parser.hpp"
#include "rlc/utils/IRange.hpp"

using namespace rlc;
using namespace llvm;
using namespace std;

int main(int argc, char* argv[])
{
	mlir::rlc::registerdialectPasses();
	mlir::DialectRegistry registry;
	registry
			.insert<mlir::BuiltinDialect, mlir::rlc::RLCDialect, mlir::DLTIDialect>();
	auto result = mlir::MlirOptMain(argc, argv, "rlc-opt", registry);
	return mlir::asMainReturnCode(result);
}
