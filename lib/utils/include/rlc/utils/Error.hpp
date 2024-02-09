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
#pragma once

#include <system_error>
#include <utility>

#include "llvm/Support/Error.h"
#include "mlir/IR/Location.h"

namespace rlc
{
	enum class RlcErrorCode
	{
		success = 0,
		unexpectedToken,
		unknownReference,
		typelessReference,
		nonFunctionCalled,
		argumentCountMissmatch,
		argumentTypeMissmatch,
		noMatchingFunction,
		alreadyDefininedVariable,
		alreadyDeclaredType
	};
}

namespace std
{
	/**
	 * This class is required to specity that ParserErrorCode is a enum that is
	 * used to rappresent errors.
	 */
	template<>
	struct is_error_condition_enum<rlc::RlcErrorCode>: public true_type
	{
	};
};	// namespace std

namespace rlc
{
	class RlcErrorCategory: public std::error_category
	{
		public:
		static RlcErrorCategory category;
		[[nodiscard]] std::error_condition default_error_condition(
				int ev) const noexcept override;

		[[nodiscard]] const char* name() const noexcept override
		{
			return "Rlc Error";
		}

		[[nodiscard]] bool equivalent(
				const std::error_code& code, int condition) const noexcept override;

		[[nodiscard]] std::string message(int ev) const noexcept override;

		static std::error_code errorCode(RlcErrorCode c)
		{
			return std::error_code(static_cast<int>(c), category);
		}
	};

	std::error_condition make_error_condition(RlcErrorCode errc);

	class RlcError: public llvm::ErrorInfo<RlcError>
	{
		private:
		std::string text;
		std::error_code ec;
		mlir::Location position;

		public:
		const static char ID;

		RlcError(std::string text, std::error_code ec, mlir::Location position)
				: text(std::move(text)), ec(ec), position(std::move(position))
		{
		}

		[[nodiscard]] const mlir::Location& getPosition() const { return position; }

		[[nodiscard]] const std::string& getText() const { return text; }

		[[nodiscard]] std::error_code convertToErrorCode() const override
		{
			return ec;
		}
		void log(llvm::raw_ostream& OS) const override { OS << text << "\n"; }
	};

}	 // namespace rlc
