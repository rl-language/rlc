#
# This file is part of the RLC project.
#
# RLC is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License version 2 as published by the Free Software Foundation.
#
# RLC is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along with RLC. If not, see <https://www.gnu.org/licenses/>.
#
import math
from typing import Tuple
from random import uniform
from torchviz import make_dot

import torch
from torch import nn, Tensor
import torch.nn.functional as F
from torch.nn import (
    TransformerEncoder,
    TransformerEncoderLayer,
    TransformerDecoder,
    TransformerDecoderLayer,
)

from tempfile import TemporaryDirectory
import os
import math
import copy
import time

from .training_data import TrainingData


def generate_square_subsequent_mask(sz: int) -> Tensor:
    """Generates an upper-triangular matrix of ``-inf``, with zeros on ``diag``."""
    return torch.triu(torch.ones(sz, sz) * float("-inf"), diagonal=1)


def get_start_of_sequence(batch_size, ntokens, device):
    return torch.full(
        fill_value=ntokens - 2, size=(1, batch_size), device=device, dtype=torch.int32
    )


def get_end_of_sequence(batch_size, dictionary_size, ntokens, device):
    result = torch.full(
        fill_value=0,
        size=(1, batch_size, ntokens - 1),
        device=device,
        dtype=torch.int32,
    )
    result2 = torch.full(
        fill_value=1, size=(1, batch_size, 1), device=device, dtype=torch.int32
    )
    return torch.cat([result, result2], dim=2)


def next_probabilities(model, data: Tensor, output: Tensor, device, ntokens):
    if data.dim() == 1:
        data = data.view(-1, 1)
    if output.dim() == 1:
        output = output.view(-1, 1)
    output = model(data)
    return output[0, :]


def run_once(
    model, data: Tensor, randomize_pick, iterations, device, ntokens
) -> Tensor:
    if data.dim() == 1:
        data = data.view(-1, 1)

    result = get_start_of_sequence(data.size(1), ntokens, device)
    for i in range(iterations):
        mask = generate_square_subsequent_mask(i + 1)
        previous_tokens = result

        output = model(data)

        last_tokens = output[-1]
        indexes = (
            torch.multinomial(torch.softmax(last_tokens, dim=1), 1).int()
            if randomize_pick
            else torch.argmax(last_tokens, dim=1).view(-1, 1)
        )
        result = torch.cat([result, indexes.t()])

    return result[1:]


def to_numpy(tensor):
    return (
        tensor.detach().cpu().numpy() if tensor.requires_grad else tensor.cpu().numpy()
    )


class TransformerModel(nn.Module):
    def __init__(
        self,
        ntoken: int,
        d_model: int,
        nhead: int,
        d_hid: int,
        nlayers: int,
        dropout: float = 0.5,
    ):
        super().__init__()
        self.model_type = "Transformer"
        self.pos_encoder = PositionalEncoding(d_model, dropout)
        encoder_layers = TransformerEncoderLayer(d_model, nhead, d_hid, dropout)
        self.transformer_encoder = TransformerEncoder(encoder_layers, nlayers)
        self.encoder = nn.Embedding(257, d_model)
        self.d_model = d_model
        self.final_linear = nn.Linear(d_model, 1)
        self.final_linear2 = nn.Linear(89, ntoken)

        self.init_weights()

    def init_weights(self) -> None:
        initrange = 0.1
        self.encoder.weight.data.uniform_(-initrange, initrange)
        self.final_linear.weight.data.uniform_(-initrange, initrange)
        self.final_linear.bias.data.zero_()
        self.final_linear2.weight.data.uniform_(-initrange, initrange)
        self.final_linear2.bias.data.zero_()

    def forward(self, src: Tensor) -> Tensor:
        """
        Arguments:
            src: Tensor, shape ``[seq_len, batch_size]``
            src_mask: Tensor, shape ``[seq_len, seq_len]``

        Returns:
            output Tensor of shape ``[seq_len, batch_size, ntoken]``
        """
        src = self.encoder(src) * math.sqrt(self.d_model)
        # src = self.pos_encoder(src)
        # encoder_output = self.transformer_encoder(src)
        result = self.final_linear(src)
        result = torch.squeeze(result, dim=2)
        result = result.view(result.size(1), result.size(0))
        result = torch.relu(result)
        output = self.final_linear2(result)
        return output


class PositionalEncoding(nn.Module):
    def __init__(self, d_model: int, dropout: float = 0.1, max_len: int = 5000):
        super().__init__()
        self.dropout = nn.Dropout(p=dropout)

        position = torch.arange(max_len).unsqueeze(1)
        div_term = torch.exp(
            torch.arange(0, d_model, 2) * (-math.log(10000.0) / d_model)
        )
        pe = torch.zeros(max_len, 1, d_model)
        pe[:, 0, 0::2] = torch.sin(position * div_term)
        pe[:, 0, 1::2] = torch.cos(position * div_term)
        self.register_buffer("pe", pe)

    def forward(self, x: Tensor) -> Tensor:
        """
        Arguments:
            x: Tensor, shape ``[seq_len, batch_size, embedding_dim]``
        """
        x = x + self.pe[: x.size(0)]
        return self.dropout(x)


class RLCTransformer:
    def __init__(self, ntokens: int, device: str):
        # d_model is the number of expected feature in the input
        self.exec = 0
        self.model = TransformerModel(
            ntoken=ntokens, d_model=100, nhead=4, d_hid=100, nlayers=2, dropout=0.2
        ).to(device)
        self.criterion = nn.MSELoss()
        self.optimizer = torch.optim.SGD(self.model.parameters(), lr=5.0)
        self.scheduler = torch.optim.lr_scheduler.StepLR(
            self.optimizer, 1.0, gamma=0.95
        )
        self.device = device
        self.ntokens = ntokens

    def train(self, train_data: Tensor, result_data: Tensor) -> None:
        self.model.train()  # turn on train mode
        total_loss = 0.0
        log_interval = 10
        start_time = time.time()
        sequence_lenght = result_data.size(1)
        src_mask = generate_square_subsequent_mask(sequence_lenght).to(self.device)

        num_batches = train_data.size(0)
        for i in range(0, train_data.size(0)):
            self.optimizer.zero_grad()
            data = train_data[i]

            targets = result_data[i, :]

            targets_reshaped = targets
            output = self.model(data)
            loss = self.criterion(output, targets_reshaped)

            loss.backward()
            torch.nn.utils.clip_grad_norm_(self.model.parameters(), 0.5)
            self.optimizer.step()

            total_loss += loss.item()
            if i % log_interval == 0 and i > 0:
                lr = self.scheduler.get_last_lr()[0]
                ms_per_batch = (time.time() - start_time) * 1000 / log_interval
                cur_loss = total_loss / log_interval
                ppl = math.exp(cur_loss)
                print(
                    f"| {i:5d}/{num_batches:5d} batches | "
                    f"lr {lr:02.2f} | ms/batch {ms_per_batch:5.2f} | "
                    f"loss {cur_loss:5.2f} | ppl {ppl:8.2f}"
                )
                total_loss = 0
                start_time = time.time()

    def evaluate(self, eval_data: Tensor, result_data: Tensor) -> float:
        self.model.eval()  # turn on evaluation mode
        total_loss = 0.0
        src_mask = generate_square_subsequent_mask(result_data.size(1)).to(self.device)

        with torch.no_grad():
            for i in range(0, eval_data.size(0)):
                data = eval_data[i]
                targets = result_data[i, :]
                seq_len = result_data[i].size(0)
                output = self.model(data)
                total_loss += seq_len * self.criterion(output, targets).item()
        return total_loss / (len(eval_data) - 1)

    def train_from_dataset(self, training_data: TrainingData, epochs):
        best_val_loss = float("inf")
        with TemporaryDirectory() as tempdir:
            best_model_params_path = os.path.join(tempdir, "best_model_params.pt")

            for epoch in range(1, epochs):
                epoch_start_time = time.time()
                self.train(
                    train_data=training_data.train_data,
                    result_data=training_data.train_data_result,
                )
                val_loss = self.evaluate(
                    eval_data=training_data.val_data,
                    result_data=training_data.val_data_result,
                )
                val_ppl = math.exp(val_loss)
                elapsed = time.time() - epoch_start_time
                print("-" * 89)
                print(
                    f"| end of epoch {epoch:3d} | time: {elapsed:5.2f}s | "
                    f"valid loss {val_loss:5.2f} | valid ppl {val_ppl:8.2f}"
                )
                print("-" * 89)

                if val_loss < best_val_loss:
                    best_val_loss = val_loss
                    torch.save(self.model.state_dict(), best_model_params_path)

                self.scheduler.step()
            self.model.load_state_dict(
                torch.load(best_model_params_path)
            )  # load best model states
