#
# Part of the RLC Project, under the MIT license. 
# See stdlib/LICENSE.TXT for license information.
# SPDX-License-Identifier: MIT
#

# A class to rapresent the absence of information
# usefull to implement functions that return optionals
# fun maybe_int() -> Int | Nothing:
cls Nothing:
	Bool _dont_care

fun none() -> Nothing:
	let to_return : Nothing
	return to_return
