
# from tensorboard.program import TensorBoard

from command_line import load_program_from_args, make_rlc_argparse
from rlc import Program

from ml.ppg.envs import RLCMultiEnv, exit_on_invalid_env, get_num_players

from tensorboard.program import TensorBoard
from ml.ppg.train import train


def main():
    parser = make_rlc_argparse("train", description="Train a network on the provided rl file")
    parser.add_argument(
        "--output",
        "-o",
        type=str,
        nargs="?",
        help="path where to write the trained network",
        default="network",
    )
    parser.add_argument("--no-tensorboard", action="store_true", default=False)
    parser.add_argument("--total-steps", default=100000000, type=int)
    parser.add_argument("--load", default="", type=str)
    parser.add_argument("--num-sample", default=1, type=int)
    parser.add_argument("--model-save-frequency", default=100, type=int,help="number of iterations before a model is saved")
    parser.add_argument("--envs", default=8, type=int, help="num of cpus taskes with playing the game while training, reduce this number to reduce ram usage, but increase trying time.")
    args = parser.parse_args()
    program = load_program_from_args(args, True)

    if not args.no_tensorboard:
        tb = TensorBoard()
        tb.configure(argv=[None, "--logdir", "/tmp/ppg/"])
        url = tb.launch()

    train(program, total_steps=args.total_steps, envs=args.envs, path_to_weights=args.load, output=args.output, model_save_frequency=args.model_save_frequency)

    program.cleanup()


if __name__ == "__main__":
    main()
