from command_line import load_program_from_args, make_rlc_argparse
from rlc import Program

from ml.ppg.envs import RLCMultiEnv, exit_on_invalid_env, get_num_players
from ml.ppg.impala_cnn import ImpalaEncoder, FullyConnectedEncoder

import ml.ppg.ppg as ppg
import numpy as np
import ml.ppg.torch_util as tu
import ml.ppg.logger as logger
import torch
from tensorboard.program import TensorBoard


class ModelSaver:
    def __init__(self, model, output, frequency=100):
        self.iteration = 0
        self.output = output
        self.model = model
        self.frequency = frequency

    def __call__(self, params):
        self.iteration = self.iteration + 1
        if self.iteration % self.frequency == 0:
            print("Saved")
            torch.save(self.model.state_dict(), self.output)
        return True


class MPIFakeObject:
    def __init__(self):
        self.size = 1
        self.rank = 0

    def Get_rank(self):
        return 0

    def allgather(self, obj):
        return [obj]

    def Allreduce(self, sendbuf, recvbuf, op=None):
        np.copyto(recvbuf, sendbuf)

    def gather(self, obj):
        return [obj]


def _make_hidde_layers(size):
    return (size, size, size)


def make_model(venv, path_to_weights="", arch="shared"):
    enc_fn = lambda obtype: FullyConnectedEncoder(
        obtype.shape,
        outsize=(
            obtype.shape[0] if venv.num_actions < obtype.shape[0] else venv.num_actions
        ),
        hidden_sizes=_make_hidde_layers(
            obtype.shape[0] if venv.num_actions < obtype.shape[0] else venv.num_actions
        ),
    )
    model = ppg.PhasicValueModel(venv.ob_space, venv.ac_space, enc_fn, arch=arch)

    if path_to_weights != "":
        dict = torch.load(path_to_weights, weights_only=False)
        model.load_state_dict(dict)

    model.to(tu.dev())
    return model


def train_fn(
    program,
    distribution_mode="hard",
    arch="shared",  # 'shared', 'detach', or 'dual'
    # 'shared' = shared policy and value networks
    # 'dual' = separate policy and value networks
    # 'detach' = shared policy and value networks, but with the value function gradient detached during the policy phase to avoid interference
    interacts_total=1000000000,
    num_envs=10,
    n_epoch_pi=1,
    n_epoch_vf=1,
    gamma=0.999,
    aux_lr=5e-4,
    lr=2e-5,
    nminibatch=2,
    aux_mbsize=4,
    clip_param=0.002,
    kl_penalty=0.0,
    n_aux_epochs=0,
    n_pi=32,
    beta_clone=1.0,
    vf_true_weight=1.0,
    model_save_frequency=1000,
    nstep=500,
    entcoef=0.0015,
    log_dir="/tmp/ppg",
    league_play_dir="",
    output="model.pt",
    path_to_weights="",
    comm=MPIFakeObject(),
):
    tu.setup_dist(comm=comm, should_init_process_group=False)
    tu.register_distributions_for_tree_util()

    if log_dir is not None:
        format_strs = ["csv", "stdout", "tensorboard"] if comm.Get_rank() == 0 else []
        logger.configure(comm=comm, dir=log_dir, format_strs=format_strs)

    venv = RLCMultiEnv(program, num=num_envs)
    model = make_model(venv, path_to_weights=path_to_weights, arch=arch)

    logger.log(tu.format_model(model))
    tu.sync_params(model.parameters())

    name2coef = {"pol_distance": beta_clone, "vf_true": vf_true_weight}

    ppg.learn(
        venv=venv,
        model=model,
        interacts_total=interacts_total,
        ppo_hps=dict(
            lr=lr,
            γ=gamma,
            λ=0.99,
            nminibatch=nminibatch,
            n_epoch_vf=n_epoch_vf,
            n_epoch_pi=n_epoch_pi,
            clip_param=clip_param,
            kl_penalty=kl_penalty,
            log_save_opts={"save_mode": "last", "num_players": venv.get_num_players()},
            nstep=nstep,
            entcoef=entcoef,
            callbacks=[ModelSaver(model, output, model_save_frequency)],
        ),
        aux_lr=aux_lr,
        aux_mbsize=aux_mbsize,
        n_aux_epochs=n_aux_epochs,
        n_pi=n_pi,
        name2coef=name2coef,
        comm=comm,
        path_to_league_play_dir=league_play_dir,
    )


def make_env(program_path, initial_states_path):
    program = Program(program_path)
    return RLCEnvironment(
        program=program,
        initial_states=load_initial_states(program, initial_states_path),
    )


def train(
    program,
    lr=1e-5,
    clip_param=0.002,
    entcoef=0.0015,
    total_steps=1000000000,
    envs=8,
    nstep=2000,
    path_to_weights="",
    output="",
    model_save_frequency=1000,
    log_dir="/tmp/ppg",
    league_play_dir="",
):
    exit_on_invalid_env(program)
    # load_initial_states(program, args.initial_states)

    # num_players = get_num_players(program.module)
    # num_agents = 1 if args.true_self_play else get_num_players(program.module)

    # initial_args_dir = args.initial_states if args.initial_states == "" else os.path.abspath(args.initial_states)

    train_fn(
        program,
        interacts_total=total_steps,
        num_envs=envs,
        nminibatch=2,
        path_to_weights=path_to_weights,
        output=output,
        model_save_frequency=model_save_frequency,
        log_dir=log_dir,
        lr=lr,
        clip_param=clip_param,
        entcoef=entcoef,
        nstep=nstep,
        league_play_dir=league_play_dir,
    )
