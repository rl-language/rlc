#
# This file is part of the RLC project.
#
# RLC is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License version 2 as published by the Free Software Foundation.
#
# RLC is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along with RLC. If not, see <https://www.gnu.org/licenses/>.
#
from torch.utils.data import dataset
from typing import Tuple
import torch
from torch import nn, Tensor


def cut_source_into_blocks(source: Tensor, bptt: int) -> Tuple[Tensor, Tensor]:
    """Creates a 3D tensor from a 1D tensor.

    Args:
    tensor: The 1D tensor to convert.

    Returns:
    A 3D tensor with the same content as the input tensor.
    """

    num_blocks = len(source) // bptt
    three_d_tensor = torch.empty((num_blocks, bptt, source.size(1)), dtype=torch.int32)
    for i in range(num_blocks):
        block = source[i * bptt : (i + 1) * bptt]
        three_d_tensor[i] = block
    return three_d_tensor


def batchify(data: Tensor, batch_size: int, device: str) -> Tensor:
    """Divides the data into ``batch_size`` separate sequences, removing extra elements
    that wouldn't cleanly fit.

    Arguments:
        data: Tensor, shape ``[N]``
        batch_size: int, batch size

    Returns:
        Tensor of shape ``[N // batch_size, batch_size]``
    """
    seq_len = data.size(0) // batch_size
    data = data[: seq_len * batch_size]
    data = data.view(batch_size, seq_len).t().contiguous()
    return data.to(device)


def batchify_raw(data: Tensor, batch_size: int, device: str) -> Tensor:
    """Divides the data into ``batch_size`` separate sequences, removing extra elements
    that wouldn't cleanly fit.

    Arguments:
        data: Tensor, shape ``[N]``
        batch_size: int, batch size

    Returns:
        Tensor of shape ``[N // batch_size, batch_size]``
    """
    seq_len = data.size(0) // batch_size
    data = data[: seq_len * batch_size]
    data = torch.stack(
        [
            torch.transpose(data[x * batch_size : (x + 1) * batch_size], 0, 1)
            for x in range(seq_len)
        ]
    ).contiguous()
    return data.to(device)


class TrainingData:
    @staticmethod
    def from_raw_bytes(
        train_data: [[bytes]],
        train_data_result: [[bytes]],
        val_data: [[bytes]],
        val_data_result: [[bytes]],
        test_data: [[bytes]],
        test_data_result: [[bytes]],
    ):
        batch_size = 20
        device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
        train_data = batchify_raw(train_data, batch_size, device)
        val_data = batchify_raw(val_data, batch_size, device)
        test_data = batchify_raw(test_data, batch_size, device)
        train_data_result = batchify_raw(train_data_result, batch_size, device)
        val_data_result = batchify_raw(val_data_result, batch_size, device)
        test_data_result = batchify_raw(test_data_result, batch_size, device)
        print(train_data_result.size())
        return TrainingData(
            device,
            train_data,
            train_data_result,
            val_data,
            val_data_result,
            test_data,
            test_data_result,
            9,
        )

    def __init__(
        self,
        device,
        train_data,
        train_data_result,
        val_data,
        val_data_result,
        test_data,
        test_data_result,
        ntokens,
    ):
        self.device = device
        self.train_data = train_data
        self.train_data_result = train_data_result
        self.val_data = val_data
        self.val_data_result = val_data_result
        self.test_data = test_data
        self.test_data_result = test_data_result
        self.ntokens = ntokens  # size of vocabulary
