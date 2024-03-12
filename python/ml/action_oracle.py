#
# This file is part of the RLC project.
#
# RLC is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License version 2 as published by the Free Software Foundation.
#
# RLC is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along with RLC. If not, see <https://www.gnu.org/licenses/>.
#
from loader import State
import torch
from ml import RLCTransformer, run_once


class RawBytesActionInvoker:
    def __init__(self, raw_bytes: bytes):
        self.raw_bytes = raw_bytes

    def __call__(self, state: State):
        if state is None:
            return None
        if state.is_done():
            return state
        action = state.simulation.action_from_byte_vector(self.raw_bytes)
        if action is None:
            return None
        if not action.can_run(state):
            return None
        return action.run(state)

class ActionOracle:
    def __init__(self, transformer: RLCTransformer):
        self.transformer = transformer

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
                result.append(RawBytesActionInvoker(None))
            else:
                result.append(RawBytesActionInvoker(result_bytes[:, index].tolist()))
                index = index + 1
        return result
