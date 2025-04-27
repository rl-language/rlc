from command_line import load_program_from_args, make_rlc_argparse
from rlc import Program
import sys

from ml.ppg.envs import RLCMultiEnv, exit_on_invalid_env, get_num_players

from tensorboard.program import TensorBoard
from ml.ppg.train import train, make_model
import ml.ppg.torch_util as tu
import ml.ppg.tree_util as tree_util


def make_action(model, env, rnn_state):
    game = env.games[0]
    if game.get_current_player() == -1:
        action_index = game.random_valid_action_index()
        game.step(action_index)
        action = game.actions()[action_index]
        return action, rnn_state

    lastrew, ob, first = tree_util.tree_map(tu.np2th, env.observe())
    action_mask = tu.np2th(env.action_mask())
    act, state_out, _ = model.act(ob, first, rnn_state, action_mask)
    action_index = act[0, 0]
    game.step(action_index)
    action = game.actions()[action_index]

    return action, state_out


def play_out(program, env, model, print_scores, iterations, output, print_progress):
    out = open(output, "w+") if output != "-" else sys.stdout
    for i in range(iterations):
        rnn_state = model.initial_state(env.num)
        if print_progress:
            print(f"iteration {i}/{iterations}")
        out.write(f"# game: {i}\n")
        while not env.games[0].is_done_underling():
            action, state_out = make_action(model, env, rnn_state)
            out.write(str(action) + "\n")
        if print_scores:
            out.write("# score: ")
            out.write(str(env.current_score))
            out.write("\n")


def main():
    parser = make_rlc_argparse("play", description="runs a action of the simulation")
    parser.add_argument(
        "--output",
        "-o",
        type=str,
        nargs="?",
        help="path where to write the output",
        default="-",
    )
    parser.add_argument("checkpoint", type=str)
    parser.add_argument("--print-scores", action="store_true", default=False)
    parser.add_argument("--iterations", default=1, type=int)
    parser.add_argument("--progress", action="store_true", default=False)

    args = parser.parse_args()
    with load_program_from_args(args, optimize=True) as program:
        env = RLCMultiEnv(program, solve_randomess=False)
        model = make_model(env, path_to_weights=args.checkpoint, arch="shared")
        play_out(
            program,
            env,
            model,
            args.print_scores,
            iterations=args.iterations,
            output=args.output,
            print_progress=args.progress,
        )


if __name__ == "__main__":
    main()
