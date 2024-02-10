# Copyright 2024 Cem Cebeci
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

ext fun fuzzer_get_input(Int max) -> Int
ext fun fuzzer_pick_argument(Int min, Int max) -> Int
ext fun fuzzer_print(Int message)
ext fun fuzzer_skip_input()
ext fun fuzzer_is_input_long_enough() -> Bool
