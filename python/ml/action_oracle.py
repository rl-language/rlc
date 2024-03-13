#
# Copyright 2024 Massimo Fioravanti
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
from loader import State
import torch
from ml import RLCTransformer, run_once


class ActionOracle:
    def __init__(self, transformer: RLCTransformer, action_table):
        self.transformer = transformer
        self.action_table = action_table

    def __call__(self, states: State):
        non_null_states = [state for state in states if state is not None]
        tensor = (
            torch.IntTensor(
                [[int(x) for x in state.as_byte_vector()] for state in non_null_states]
            )
            .t()
            .to(self.transformer.device)
        )
        result_bytes = run_once(
            self.transformer.model,
            tensor,
            True,
            3,
            self.transformer.device,
            self.transformer.ntokens,
        )

        result = []
        index = 0
        for state in states:
            if state is None:
                result.append(None)
            else:
                num = result_bytes[0, index]
                num = num % len(self.action_table)
                if num >= len(self.action_table) or num < 0:
                    result.append(None)
                else:
                    action = self.action_table[num]
                    if not action.can_run(state):
                        result.append(None)
                    else:
                        result.append(action)
                index = index + 1
        return result
