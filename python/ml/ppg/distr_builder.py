import warnings
from functools import partial
import torch as th
import torch.distributions as dis
from gym3.types import Discrete, Real, TensorType
from torch.distributions.utils import probs_to_logits, logits_to_probs


class Categorical:
    def __init__(self, probs_shape):
        # NOTE: probs_shape is supposed to be
        #       the shape of probs that will be
        #       produced by policy network
        if len(probs_shape) < 1:
            raise ValueError("`probs_shape` must be at least 1.")
        self.probs_dim = len(probs_shape)
        self.probs_shape = probs_shape
        self._num_events = probs_shape[-1]
        self._batch_shape = probs_shape[:-1] if self.probs_dim > 1 else th.Size()
        self._event_shape = th.Size()

    def set_probs_(self, probs):
        self.probs = probs
        self.logits = probs_to_logits(self.probs)

    def set_probs(self, probs):
        self.probs = probs / probs.sum(-1, keepdim=True)
        self.logits = probs_to_logits(self.probs)

    def set_logits(self, logits):
        self.logits = logits - logits.logsumexp(dim=-1, keepdim=True)
        self.probs = logits_to_probs(self.logits)

    def sample(self, sample_shape=th.Size()):
        if not isinstance(sample_shape, th.Size):
            sample_shape = th.Size(sample_shape)
        probs_2d = self.probs.reshape(-1, self._num_events)
        samples_2d = th.multinomial(probs_2d, sample_shape.numel(), True).T
        return samples_2d.reshape(sample_shape + self._batch_shape + self._event_shape)

    def log_prob(self, value):
        value = value.long().unsqueeze(-1)
        value, log_pmf = th.broadcast_tensors(value, self.logits)
        value = value[..., :1]
        return log_pmf.gather(-1, value).squeeze(-1)

    def entropy(self):
        min_real = th.finfo(self.logits.dtype).min
        logits = th.clamp(self.logits, min=min_real)
        p_log_p = logits * self.probs
        return -p_log_p.sum(-1)


def _make_categorical(x, ncat, shape):
    x = x.reshape((*x.shape[:-1], *shape, ncat))
    cat = Categorical(x.shape)
    cat.set_logits(x)
    return cat


def _make_normal(x, shape):
    warnings.warn("Using stdev=1")
    return dis.Normal(loc=x.reshape(x.shape[:-1] + shape), scale=1.0)


def _make_bernoulli(x, shape):  # pylint: disable=unused-argument
    return dis.Bernoulli(logits=x)


def tensor_distr_builder(ac_space):
    """
    Like distr_builder, but where ac_space is a TensorType
    """
    assert isinstance(ac_space, TensorType)
    eltype = ac_space.eltype
    # if eltype == Discrete(2):
    # return (ac_space.size, partial(_make_bernoulli, shape=ac_space.shape))
    if isinstance(eltype, Discrete):
        return (
            eltype.n * ac_space.size,
            partial(_make_categorical, shape=ac_space.shape, ncat=eltype.n),
        )
    else:
        raise ValueError(f"Expected ScalarType, got {type(ac_space)}")


def distr_builder(ac_type) -> "(int) size, (function) distr_from_flat":
    """
    Tell a network constructor what it needs to produce a certain output distribution
    Returns:
        - size: the size of a flat vector needed to construct the distribution
        - distr_from_flat: function that takes flat vector and turns it into a
          torch.Distribution object.
    """
    if isinstance(ac_type, TensorType):
        return tensor_distr_builder(ac_type)
    else:
        raise NotImplementedError
