from ray.rllib.core.rl_module.rl_module import RLModule, RLModuleConfig
from ray.rllib.policy.sample_batch import SampleBatch
from ray.rllib.algorithms.ppo.torch.ppo_torch_rl_module import PPOTorchRLModule

class ActionMaskRLMBase(RLModule):
    def __init__(self, config: RLModuleConfig):
        if not isinstance(config.observation_space, gym.spaces.Dict):
            super().__init__(config)
            return
        # We need to adjust the observation space for this RL Module so that, when
        # building the default models, the RLModule does not "see" the action mask but
        # only the original observation space without the action mask. This tricks it
        # into building models that are compatible with the original observation space.
        config.observation_space = config.observation_space["observations"]

        # The PPORLModule, in its constructor, will build models for the modified
        # observation space.
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
    def _forward_inference(self, batch, **kwargs):
        return mask_forward_fn_torch(super()._forward_inference, batch, **kwargs)

    def _forward_train(self, batch, *args, **kwargs):
        return mask_forward_fn_torch(super()._forward_train, batch, **kwargs)

    def _forward_exploration(self, batch, *args, **kwargs):
        return mask_forward_fn_torch(super()._forward_exploration, batch, **kwargs)

    def _compute_values(self, batch, device=None):
      _check_batch(batch)

      # Extract the available actions tensor from the observation.
      action_mask = batch[SampleBatch.OBS]["action_mask"]

      # Modify the incoming batch so that the default models can compute logits and
      # values as usual.
      batch[SampleBatch.OBS] = batch[SampleBatch.OBS]["observations"]
      return super()._compute_values(batch, device)


def mask_forward_fn_torch(forward_fn, batch, **kwargs):
    _check_batch(batch)

    # Extract the available actions tensor from the observation.
    action_mask = batch[SampleBatch.OBS]["action_mask"]

    # Modify the incoming batch so that the default models can compute logits and
    # values as usual.
    batch[SampleBatch.OBS] = batch[SampleBatch.OBS]["observations"]

    outputs = forward_fn(batch, **kwargs)

    # Mask logits
    logits = outputs[SampleBatch.ACTION_DIST_INPUTS]
    # Convert action_mask into a [0.0 || -inf]-type mask.
    inf_mask = torch.clamp(torch.log(action_mask), min=FLOAT_MIN)
    masked_logits = logits + inf_mask

    # Replace original values with masked values.
    outputs[SampleBatch.ACTION_DIST_INPUTS] = masked_logits

    return outputs


