
def agent_to_module_mapping_fn(agent_id, episode, **kwargs):
    modulo_id = hash(episode.id_) % 5
    if agent_id == 0:
        #if modulo_id == 1 or modulo_id == 0:
        if modulo_id == 1:
            return "p3"
        else:
            return "p1"
    elif agent_id == 1:
        if modulo_id == 0:
            return "p4"
        else:
            return "p2"
    assert False

def get_config():
    # Step 1: Configure PPO to run 64 parallel workers to collect samples from the env.
    ppo_config = (
        PPOConfig()
        .multi_agent(
            policies={"p1", "p2", "p3", "p4"},
            policy_mapping_fn=agent_to_module_mapping_fn,
            policies_to_train=["p1", "p2"],
        )
        .experimental(_enable_new_api_stack=True, _disable_preprocessor_api=True)
        .rl_module(
            rl_module_spec=MultiAgentRLModuleSpec(
                module_specs={
                    "p1": SingleAgentRLModuleSpec(
                        TorchActionMaskRLM, observation_space=RLCEnvironment().unwrapper_space
                    ),
                    "p2": SingleAgentRLModuleSpec(
                        TorchActionMaskRLM, observation_space=RLCEnvironment().unwrapper_space
                    ),
                    "p3": SingleAgentRLModuleSpec(
                        TorchActionMaskRLM, observation_space=RLCEnvironment().unwrapper_space
                    ),
                    "p4": SingleAgentRLModuleSpec(
                        TorchActionMaskRLM, observation_space=RLCEnvironment().unwrapper_space
                    ),
                    # "random": SingleAgentRLModuleSpec(module_class=RandomRLModule)
                },
            ),
        )
        .training(lr=2e-5)
        .evaluation(evaluation_interval=20, evaluation_parallel_to_training=True, evaluation_config=PPOConfig.overrides(policy_mapping_fn=agent_to_module_mapping_fn), evaluation_num_workers=1)
        .resources(
            num_gpus=1,
            num_gpus_per_learner_worker=1,
            num_cpus_per_worker=1,
            num_cpus_for_local_worker=1,
        )
        .environment(
            env=RLCEnvironment,
            env_config={"observation_space": RLCEnvironment().observation_space},
        )
        .rollouts(
            num_rollout_workers=8, num_envs_per_worker=1, env_runner_cls=MultiAgentEnvRunner
        )
        .framework("torch")
    )
    ppo_config.num_gpus = 1
    ppo_config.model["fcnet_hiddens"] = [2024, 2024, 1024, 1024]
    ppo_config.model["fcnet_hiddens"] = [2024, 2024, 1024, 1024]
    ppo_config.model["fcnet_activation"] = "relu"
    ppo_config.model["framestack"] = False

    stop = {
        "timesteps_total": 1e15,
        # "episode_reward_mean": 2,  # divide by num_agents for actual reward per agent
    }


    space = {
        "lambda": hp.uniform("lambda", 0.9, 1.0),
        "clip_param": hp.uniform("clip_param", 0.005, 0.02),
        "lr_schedule": [[0, 2e-4], [1e6, 1e-8]],
        "num_sgd_iter": hp.uniformint("num_sgd_iter", 1, 2),
        "sgd_minibatch_size": hp.uniformint("sgd_minibatch_size", 950, 2000),
        "train_batch_size": hp.uniformint("train_batch_size", 2000, 16000),
        "kl_coeff": hp.uniform("kl_coeff", 0.0, 0.3),
        "entropy_coeff": hp.uniform("entropy_coeff", 0.0, 0.3),
    }

    initial = {
        "lambda": 1.0,
        "clip_param": 0.007,
        "lr_schedule": [[0, 2e-5], [1e6, 1e-8]],
        "num_sgd_iter": 30,
        "sgd_minibatch_size": 128,
        "train_batch_size": 4000,
        #"kl_coeff": 0.23,
        "kl_coeff": 0.00,
        "entropy_coeff": 0.03,
    }
    ppo_config.lambda_ = 0.94
    ppo_config.clip_param = 0.007
    ppo_config.lr_schedule = [[0, 2e-5], [1e6, 1e-8]]
    ppo_config.num_sgd_iter = 1
    ppo_config.sgd_minibatch_size = 2000
    ppo_config.train_batch_size = 5000
    #ppo_config.kl_coeff = 0.23
    ppo_config.kl_coeff = 0.0
    ppo_config.entropy_coeff = 0.03

    initial2 = {
        "lambda": 0.94,
        "clip_param": 0.007,
        "lr_schedule": [[0, 2e-5], [1e6, 1e-8]],
        "num_sgd_iter": 1,
        "sgd_minibatch_size": 2000,
        "train_batch_size": 5000,
        #"kl_coeff": 0.23,
        "kl_coeff": 0.00,
        "entropy_coeff": 0.03,
    }

    hyperopt_search = HyperOptSearch(
        space, "episode_reward_mean", mode="max", points_to_evaluate=[initial2]
    )
    return ppo_config, hyperopt_search
