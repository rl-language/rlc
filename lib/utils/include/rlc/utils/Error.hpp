#pragma once

#include <system_error>

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
		[[nodiscard]] std::error_condition default_error_condition(int ev) const
				noexcept override;

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

}	 // namespace rlc
