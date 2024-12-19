
# from tensorboard.program import TensorBoard

from command_line import load_program_from_args, make_rlc_argparse
from rlc import Program

from ml.ppg.envs import RLCMultiEnv, exit_on_invalid_env, get_num_players
from ml.ppg.impala_cnn import ImpalaEncoder, FullyConnectedEncoder

import ml.ppg.ppg as ppg
import ml.ppg.torch_util as tu
import ml.ppg.logger as logger

from mpi4py import MPI


def train_fn(
    program,
    distribution_mode="hard",
    arch="shared",  # 'shared', 'detach', or 'dual'
    # 'shared' = shared policy and value networks
    # 'dual' = separate policy and value networks
    # 'detach' = shared policy and value networks, but with the value function gradient detached during the policy phase to avoid interference
    interacts_total=1000000,
    num_envs=6,
    n_epoch_pi=1,
    n_epoch_vf=1,
    gamma=.999,
    aux_lr=5e-4,
    lr=2e-5,
    nminibatch=8,
    aux_mbsize=4,
    clip_param=.2,
    kl_penalty=0.0,
    n_aux_epochs=6,
    n_pi=32,
    beta_clone=1.0,
    vf_true_weight=1.0,
    log_dir='/tmp/ppg',
    comm=None):
    if comm is None:
        comm = MPI.COMM_WORLD
    tu.setup_dist(comm=comm)
    tu.register_distributions_for_tree_util()

    if log_dir is not None:
        format_strs = ['csv', 'stdout'] if comm.Get_rank() == 0 else []
        logger.configure(comm=comm, dir=log_dir, format_strs=format_strs)

    venv = RLCMultiEnv(program, num=num_envs)

    enc_fn = lambda obtype: FullyConnectedEncoder(
        obtype.shape,
        outsize=obtype.shape[0],
        hidden_sizes=(obtype.shape[0], obtype.shape[0]),
    )
    model = ppg.PhasicValueModel(venv.ob_space, venv.ac_space, enc_fn, arch=arch)

    model.to(tu.dev())
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
            log_save_opts={"save_mode": "last"},
            nstep=1000,
        ),
        aux_lr=aux_lr,
        aux_mbsize=aux_mbsize,
        n_aux_epochs=n_aux_epochs,
        n_pi=n_pi,
        name2coef=name2coef,
        comm=comm,
    )



# def get_multi_train(num_players, output):
    # def train_impl(config, fixed_nets=0):
        # config = PPOConfig().update_from_dict(config)
        # model = config.build()

        # for _ in range(1000000000):
            # with open(f"list_of_fixed.txt", "wb+") as f:
                # for i in range(num_players):
                    # if not os.path.exists(f"net_p{num_players + i}_{fixed_nets}"):
                        # os.makedirs(f"net_p{num_players + i}_{fixed_nets}")
                    # model.workers.local_worker().module[f"p{i}"].save_state(
                        # f"net_p{num_players + i}_{fixed_nets}"
                    # )
                # fixed_nets = fixed_nets + 1
                # pickle.dump(fixed_nets, f)
            # for x in range(20):
                # for i in range(10):
                    # print(x, i)
                    # train.report(model.train())
                    # if fixed_nets != 0:
                        # for i in range(num_players):
                            # model.workers.local_worker().module[
                                # f"p{i + num_players}"
                            # ].load_state(
                                # f"net_p{i + num_players}_{random.choice(range(fixed_nets))}"
                            # )
                        # model.workers.sync_weights(
                            # policies=[f"p{i + num_players}" for i in range(num_players)]
                        # )
            # model.save(f"checkpoint")
            # save(model, output, num_players)
        # model.save()
        # model.stop()

    # return train_impl


# def get_trainer(output_path, total_train_iterations, num_players):
    # def single_agent_train(config):
        # config = PPOConfig().update_from_dict(config)
        # model = config.build()
        # for _ in range(math.ceil(total_train_iterations / 10)):
            # for i in range(10):
                # train.report(model.train())
            # save(model, output_path, num_players)

        # model.save()
        # model.stop()

    # return single_agent_train



# def load_initial_states(program, init_state_dir):
    # initial_states = []
    # if init_state_dir == "":
        # return initial_states

    # if not os.path.isdir(init_state_dir):
        # print(f"initial states directory {init_state_dir} is not a directory")
        # exit(-1)

    # for filename in os.listdir(init_state_dir):
        # with open(f"{init_state_dir}/{filename}", "r") as file:
            # content = "".join(file.readlines())
            # state = program.module.Game()
            # if not program.functions.from_string(state, program.to_rl_string(content)).value:
                # print(f"failed to parse {file}")
                # exit(-1)
            # initial_states.append(state)

    # return initial_states

def make_env(program_path, initial_states_path):
    program = Program(program_path)
    return RLCEnvironment(program=program, initial_states=load_initial_states(program, initial_states_path))

def main():
    parser = make_rlc_argparse("train", description="runs a action of the simulation")
    parser.add_argument(
        "--output",
        "-o",
        type=str,
        nargs="?",
        help="path where to write the output",
        default="network",
    )
    parser.add_argument("--true-self-play", action="store_true", default=False)
    parser.add_argument("--league-play", action="store_true", default=False)
    parser.add_argument("--initial-states", type=str, default="")
    parser.add_argument("--no-tensorboard", action="store_true", default=False)
    parser.add_argument("--total-train-iterations", default=100000000, type=int)
    parser.add_argument("--num-sample", default=1, type=int)
    parser.add_argument("--num-rollout-cpus", default=8, type=int, help="num of cpus taskes with playing the game while training, reduce this number to reduce ram usage, but increase trying time.")

    args = parser.parse_args()
    program = load_program_from_args(args, True)
    if not program.functions.print_enumeration_errors(program.module.AnyGameAction()):
        exit(-1)
    program.functions.emit_observation_tensor_warnings(program.functions.play())
    exit_on_invalid_env(program)
    # load_initial_states(program, args.initial_states)

    # num_players = get_num_players(program.module)
    # num_agents = 1 if args.true_self_play else get_num_players(program.module)


    # initial_args_dir = args.initial_states if args.initial_states == "" else os.path.abspath(args.initial_states)
    # if not args.no_tensorboard:
        # tb = TensorBoard()
        # tb.configure(argv=[None, "--logdir", session_dir])
        # url = tb.launch()

    comm = MPI.COMM_WORLD

    train_fn(program, comm=comm)

    program.cleanup()


if __name__ == "__main__":
    main()
