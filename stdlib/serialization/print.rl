# Copyright 2024 Massimo Fioravanti
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import string

ext fun print_string(String s)
ext fun print_string_lit(StringLiteral s)

# print a arbitrary object to stdout
fun<T> print(T to_print):
    if to_print is String:
        print_string(to_print)
    else if to_print is StringLiteral:
        print_string_lit(to_print)
    else:
        print_string(to_string(to_print)) 

# print a arbitrary object to stdout on multiple 
# lines and indented
fun<T> print_indented(T to_print):
    if to_print is String:
        print_string(to_print.to_indented_lines())
    else if to_print is StringLiteral:
        print_string_lit(to_print)
    else:
        print_string(to_string(to_print).to_indented_lines()) 
