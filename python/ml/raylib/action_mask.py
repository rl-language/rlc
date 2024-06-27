from ray.rllib.core.rl_module.rl_module import RLModule, RLModuleConfig
from ray.rllib.policy.sample_batch import SampleBatch
import torch
from ray.rllib.utils.torch_utils import FLOAT_MIN
from ray.rllib.algorithms.ppo.torch.ppo_torch_rl_module import PPOTorchRLModule
import gymnasium as gym
from ray.rllib.utils.annotations import override
from ray.rllib.core.columns import Columns
from ray.rllib.utils.typing import TensorType

from typing import Dict, Optional, Tuple


class ActionMaskRLMBase(RLModule):
    @override(RLModule)
    def __init__(self, config: RLModuleConfig):
        # If observation space is not of type `Dict` raise an error.
        if not isinstance(config.observation_space, gym.spaces.dict.Dict):
            raise ValueError(
                "This RLModule requires the environment to provide a "
                "`gym.spaces.Dict` observation space of the form: \n"
                " {'action_mask': Box(0.0, 1.0, shape=(self.action_space.n,)),"
                "  'observation_space': self.observation_space}"
            )

        # While the environment holds an observation space that contains, both,
        # the action mask and the original observation space, the 'RLModule'
        # receives only the `"observation"` element of the space, but not the
        # action mask.
        self.observation_space_with_mask = config.observation_space
        config.observation_space = config.observation_space["observations"]

        # Keeps track if observation specs have been checked already.
        self._checked_observations = False

        # The PPORLModule, in its constructor will build networks for the original
        # observation space (i.e. without the action mask).
        super().__init__(config)



def _check_batch(batch):
    """Check whether the batch contains the required keys."""
    if "action_mask" not in batch[SampleBatch.OBS]:
        raise ValueError(
            "Action mask not found in observation. This model requires "
            "the environment to provide observations that include an "
            "action mask (i.e. an observation space of the Dict space "
            "type that looks as follows: \n"
            "{'action_mask': Box(0.0, 1.0, shape=(self.action_space.n,)),"
            "'observations': <observation_space>}"
        )
    if "observations" not in batch[SampleBatch.OBS]:
        raise ValueError(
            "Observations not found in observation.This model requires "
            "the environment to provide observations that include a "
            " (i.e. an observation space of the Dict space "
            "type that looks as follows: \n"
            "{'action_mask': Box(0.0, 1.0, shape=(self.action_space.n,)),"
            "'observations': <observation_space>}"
        )


class TorchActionMaskRLM(ActionMaskRLMBase, PPOTorchRLModule):
    @override(PPOTorchRLModule)
    def setup(self):
        super().setup()
        # We need to reset here the observation space such that the
        # super`s (`PPOTorchRLModule`) observation space is the
        # original space (i.e. without the action mask) and `self`'s
        # observation space contains the action mask.
        self.config.observation_space = self.observation_space_with_mask

    @override(PPOTorchRLModule)
    def setup(self):
        super().setup()
        # We need to reset here the observation space such that the
        # super`s (`PPOTorchRLModule`) observation space is the
        # original space (i.e. without the action mask) and `self`'s
        # observation space contains the action mask.
        self.config.observation_space = self.observation_space_with_mask

    @override(PPOTorchRLModule)
    def _forward_inference(
        self, batch: Dict[str, TensorType], **kwargs
    ) -> Dict[str, TensorType]:
        # Preprocess the original batch to extract the action mask.
        action_mask, batch = self._preprocess_batch(batch)
        # Run the forward pass.
        outs = super()._forward_inference(batch, **kwargs)
        # Mask the action logits and return.
        return self._mask_action_logits(outs, action_mask)

    @override(PPOTorchRLModule)
    def _forward_exploration(
        self, batch: Dict[str, TensorType], **kwargs
    ) -> Dict[str, TensorType]:
        # Preprocess the original batch to extract the action mask.
        action_mask, batch = self._preprocess_batch(batch)
        # Run the forward pass.
        outs = super()._forward_exploration(batch, **kwargs)
        # Mask the action logits and return.
        return self._mask_action_logits(outs, action_mask)

    @override(PPOTorchRLModule)
    def _forward_train(
        self, batch: Dict[str, TensorType], **kwargs
    ) -> Dict[str, TensorType]:
        # Preprocess the original batch to extract the action mask.
        action_mask, batch = self._preprocess_batch(batch)
        # Run the forward pass.
        outs = super()._forward_train(batch, **kwargs)
        # Mask the action logits and return.
        return self._mask_action_logits(outs, action_mask)

    @override(PPOTorchRLModule)
    def _compute_values(self, batch: Dict[str, TensorType], device):
        # Preprocess the batch to extract the `observations` to `Columns.OBS`.
        _, batch = self._preprocess_batch(batch)
        # Call the super's method to compute values for GAE.
        return super()._compute_values(batch, device)

    def _preprocess_batch(
        self, batch: Dict[str, TensorType], **kwargs
    ) -> Tuple[TensorType, Dict[str, TensorType]]:
        """Extracts observations and action mask from the batch

        Args:
            batch: A dictionary containing tensors (at least `Columns.OBS`)

        Returns:
            A tuple with the action mask tensor and the modified batch containing
                the original observations.
        """
        # Check observation specs for action mask and observation keys.
        self._check_batch(batch)

        # Extract the available actions tensor from the observation.
        action_mask = batch[Columns.OBS].pop("action_mask")

        # Modify the batch for the `PPORLModule`'s `forward` method, i.e.
        # pass only `"obs"` into the `forward` method.
        batch[Columns.OBS] = batch[Columns.OBS].pop("observations")

        # Return the extracted action mask and the modified batch.
        return action_mask, batch

    def _check_batch(self, batch: Dict[str, TensorType]) -> Optional[ValueError]:
        """Assert that the batch includes action mask and observations.

        Args:
            batch: A dicitonary containing tensors (at least `Columns.OBS`) to be
                checked.

        Raises:
            `ValueError` if the column `Columns.OBS`  does not contain observations
                and action mask.
        """
        if not self._checked_observations:
            if "action_mask" not in batch[Columns.OBS]:
                raise ValueError(
                    "No action mask found in observation. This `RLModule` requires "
                    "the environment to provide observations that include an "
                    "action mask (i.e. an observation space of the Dict space "
                    "type that looks as follows: \n"
                    "{'action_mask': Box(0.0, 1.0, shape=(self.action_space.n,)),"
                    "'observations': self.observation_space}"
                )
            if "observations" not in batch[Columns.OBS]:
                raise ValueError(
                    "No observations found in observation. This 'RLModule` requires "
                    "the environment to provide observations that include the original "
                    "observations under a key `'observations'` in a dict (i.e. an "
                    "observation space of the Dict space type that looks as follows: \n"
                    "{'action_mask': Box(0.0, 1.0, shape=(self.action_space.n,)),"
                    "'observations': <observation_space>}"
                )
            self._checked_observations = True

    def _mask_action_logits(
        self, batch: Dict[str, TensorType], action_mask: TensorType
    ) -> Dict[str, TensorType]:
        """Masks the action logits for the output of `forward` methods

        Args:
            batch: A dictionary containing tensors (at least action logits).
            action_mask: A tensor containing the action mask for the current
                observations.

        Returns:
            A modified batch with masked action logits for the action distribution
            inputs.
        """
        # Convert action mask into an `[0.0][-inf]`-type mask.
        inf_mask = torch.clamp(torch.log(action_mask), min=FLOAT_MIN)

        # Mask the logits.
        batch[Columns.ACTION_DIST_INPUTS] += inf_mask

        # Return the batch with the masked action logits.
        return batch
