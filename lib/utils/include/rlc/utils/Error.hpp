/*
This file is part of the RLC project.

RLC is free software: you can redistribute it and/or modify it under the terms
of the GNU General Public License version 2 as published by the Free Software
Foundation.

RLC is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
RLC. If not, see <https://www.gnu.org/licenses/>.
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
