import torch
from torch import nn, Tensor
from typing import Tuple
from torch.utils.data import dataset
from torchtext.vocab import build_vocab_from_iterator, Vocab
import time
import math
