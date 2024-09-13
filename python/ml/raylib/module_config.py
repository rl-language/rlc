from ray.rllib.algorithms.ppo import PPOConfig
from ray.rllib.core.rl_module.marl_module import (
    MultiAgentRLModuleSpec,
    MultiAgentRLModule,
    MultiAgentRLModuleConfig,
)
from hyperopt import hp
import ray
import torch
from ray.tune.search.hyperopt import HyperOptSearch
from ray.rllib.core.rl_module.rl_module import SingleAgentRLModuleSpec
from ml.raylib.action_mask import TorchActionMaskRLM
from ml.raylib.environment import RLCEnvironment
from ray.rllib.env.multi_agent_env_runner import MultiAgentEnvRunner


def configure_mapping(num_players, league_play, true_self_play):
    def agent_to_module_mapping_fn(agent_id, episode, **kwargs):
        if true_self_play:
            return "p0"
        assert agent_id in range(num_players)
        modulo_id = hash(episode.id_) % (num_players * 5)
        for i in range(num_players):
            if agent_id == i:
                if modulo_id == i and league_play:
                    return f"p{i + num_players}"
                else:
                    return f"p{i}"
        print(num_players, league_play, true_self_play, agent_id)
        assert False

    return agent_to_module_mapping_fn


class CallBack(ray.rllib.algorithms.callbacks.DefaultCallbacks):
    def on_episode_end(
        self,
        *,
        episode,
        env_runner,
        metrics_logger,
        env,
        env_index,
        rl_module,
        **kwargs,
    ):
        env.log_extra_metrics(metrics_logger)


def get_config(
    program, num_agents=1, exploration=True, league_play=False, true_self_play=False, num_rollout_workers=8
):
    state_size = RLCEnvironment(program=program).state_size
    actions_count = RLCEnvironment(program=program).num_actions
    print("state size", state_size)
    print("actions count", actions_count)
    # Step 1: Configure PPO to run 64 parallel workers to collect samples from the env.
    total_policies_count = num_agents if not true_self_play else 1
    total_policies_count = (
        (total_policies_count * 2) if league_play else total_policies_count
    )
    ppo_config = (
        PPOConfig()
        .multi_agent(
            policies=({f"p{x}" for x in range(total_policies_count)}),
            policy_mapping_fn=(
                configure_mapping(
                    num_agents, league_play=league_play, true_self_play=true_self_play
                )
            ),
            policies_to_train=[
                f"p{x}" for x in range(num_agents if not true_self_play else 1)
            ],
        )
        .experimental(_disable_preprocessor_api=True)
        .api_stack(
            enable_rl_module_and_learner=True, enable_env_runner_and_connector_v2=True
        )
        .callbacks(CallBack)
        .rl_module(
            rl_module_spec=MultiAgentRLModuleSpec(
                module_specs=(
                    {
                        f"p{x}": SingleAgentRLModuleSpec(
                            TorchActionMaskRLM,
                            observation_space=RLCEnvironment(
                                program=program
                            ).observation_space[0],
                        )
                        for x in range(total_policies_count)
                    }
                ),
            ),
        )
        .training(lr=2e-4)
        .evaluation(
            evaluation_config=PPOConfig.overrides(
                policy_mapping_fn=(
                    configure_mapping(
                        num_agents,
                        league_play=league_play,
                        true_self_play=true_self_play,
                    )
                )
            ),
            evaluation_duration=1,
            evaluation_duration_unit="episodes",
            # disabled evaluation due to crash of ray in 2.30
            # ToDo: enable me back
            evaluation_parallel_to_training=False,
            evaluation_interval=0,
            evaluation_num_workers=0,
        )
        .resources(
            num_gpus=1 if torch.cuda.is_available() else 0,
            num_gpus_per_learner_worker=1 if torch.cuda.is_available() else 0,
            num_cpus_per_worker=1,
            num_cpus_for_local_worker=1,
        )
        .environment(
            env="rlc_env",
            env_config={
                "observation_space": RLCEnvironment(program=program).observation_space,
                "action_space": RLCEnvironment(program=program).action_space,
            },
        )
        .rollouts(
            num_rollout_workers=num_rollout_workers,
            num_envs_per_worker=1,
            env_runner_cls=MultiAgentEnvRunner,
        )
        .framework("torch")
    )
    ppo_config.num_gpus = 1
    ppo_config.model["fcnet_hiddens"] = [
        state_size,
        state_size,
        state_size,
        actions_count,
    ]
    ppo_config.model["fcnet_activation"] = "relu"
    ppo_config.model["framestack"] = False
    ppo_config.model["exploration"] = exploration

    space = {
        "lambda": hp.uniform("lambda", 0.9, 1.0),
        "clip_param": hp.uniform("clip_param", 0.005, 0.02),
        "lr_schedule": [[0, 1e-5], [1e6, 1e-8]],
        "num_sgd_iter": hp.uniformint("num_sgd_iter", 1, 2),
        "sgd_minibatch_size": hp.uniformint("sgd_minibatch_size", 950, 2000),
        "train_batch_size": hp.uniformint("train_batch_size", 2000, 16000),
        "kl_coeff": hp.uniform("kl_coeff", 0.0, 0.3),
        "entropy_coeff": hp.uniform("entropy_coeff", 0.0, 0.3),
    }

    initial = {
        "lambda": 1.0,
        "clip_param": 0.007,
        "lr_schedule": [[0, 1e-5], [1e6, 1e-8]],
        "num_sgd_iter": 30,
        "sgd_minibatch_size": 128,
        "train_batch_size": 4000,
        # "kl_coeff": 0.23,
        "kl_coeff": 0.00,
        "entropy_coeff": 0.03,
    }
    ppo_config.lambda_ = 0.94
    ppo_config.clip_param = 0.007
    ppo_config.lr_schedule = [[0, 2e-5], [1e6, 1e-8]]
    ppo_config.num_sgd_iter = 1
    ppo_config.sgd_minibatch_size = 2000
    ppo_config.train_batch_size = 5000
    # ppo_config.kl_coeff = 0.23
    ppo_config.kl_coeff = 0.0
    ppo_config.entropy_coeff = 0.03

    initial2 = {
        "lambda": 0.94,
        "clip_param": 0.007,
        "lr_schedule": [[0, 2e-5], [1e6, 1e-8]],
        "num_sgd_iter": 1,
        "sgd_minibatch_size": 2000,
        "train_batch_size": 5000,
        # "kl_coeff": 0.23,
        "kl_coeff": 0.00,
        "entropy_coeff": 0.03,
    }

    initial3 = {
        "lambda": 0.99,
        "clip_param": 0.010,
        "lr_schedule": [[0, 2e-5], [1e6, 1e-8]],
        "num_sgd_iter": 2,
        "sgd_minibatch_size": 2000,
        "train_batch_size": 5000,
        # "kl_coeff": 0.23,
        "kl_coeff": 0.0,
        "entropy_coeff": 0.0015,
    }

    hyperopt_search = HyperOptSearch(
        space,
        "env_runners/episode_return_mean",
        mode="max",
        points_to_evaluate=[initial3],
    )
    return ppo_config, hyperopt_search
