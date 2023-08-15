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
        return state.execute_from_raw_bytes(self.raw_bytes)


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
                result.append(RawBytesActionInvoker(result_bytes[:, index]))
                index = index + 1
        return result
