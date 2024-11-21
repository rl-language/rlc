#pragma once
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

#include "mlir/IR/Builders.h"
#include "rlc/dialect/Operations.hpp"

namespace mlir::rlc
{
	template<typename Base>
	class IRBuilderWrapper: public Base
	{
		public:
		using Base::Base;
#include "rlc/dialect/Builder.inc"
	};

	using IRBuilder = IRBuilderWrapper<mlir::OpBuilder>;

}	 // namespace mlir::rlc
