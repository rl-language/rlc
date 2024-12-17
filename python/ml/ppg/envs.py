import numpy as np
import gym3
from procgen import ProcgenGym3Env

import tic_tac_toe as ttt

def get_procgen_venv(*, env_id, num_envs, rendering=False, **env_kwargs):
    if rendering:
        env_kwargs["render_human"] = True

    if env_id == "tic_tac_toe":
        return ttt.TicTacToeEnv(num=num_envs)

    env = ProcgenGym3Env(num=num_envs, env_name=env_id, **env_kwargs)

    env = gym3.ExtractDictObWrapper(env, "rgb")

    if rendering:
        env = gym3.ViewerWrapper(env, info_key="rgb")
    return env

def get_venv(num_envs, env_name, **env_kwargs):
    venv = get_procgen_venv(num_envs=num_envs, env_id=env_name, **env_kwargs)

    return venv
