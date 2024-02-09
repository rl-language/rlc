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
#include "rlc/utils/Error.hpp"

using namespace std;
using namespace rlc;

rlc::RlcErrorCategory RlcErrorCategory::category;

error_condition rlc::make_error_condition(RlcErrorCode errc)
{
	return std::error_condition(
			static_cast<int>(errc), RlcErrorCategory::category);
}

[[nodiscard]] std::error_condition RlcErrorCategory::default_error_condition(
		int ev) const noexcept
{
	if (ev == 0)
		return std::error_condition(RlcErrorCode::success);
	return std::error_condition(RlcErrorCode::unexpectedToken);
}

[[nodiscard]] bool RlcErrorCategory::equivalent(
		const std::error_code& code, int condition) const noexcept
{
	bool equal = *this == code.category();
	auto v = default_error_condition(code.value()).value();
	equal = equal && static_cast<int>(v) == condition;
	return equal;
}

[[nodiscard]] std::string RlcErrorCategory::message(int ev) const noexcept
{
	switch (ev)
	{
		case 0:
			return "Success";
		case 1:
			return "Unexpected Token";
		case 2:
			return "Unknown Reference";
		case 3:
			return "Typeless Reference";
		case 4:
			return "Called item was not a function";
		case 5:
			return "Arguments missmatch";
		case 6:
			return "Arguments count missmatch";
		case 7:
			return "No matching function";
		case 8:
			return "Already defined variable";
		case 9:
			return "Already declared type";
		default:
			return "Unkown Error";
	}
}

const char RlcError::ID = '0';
