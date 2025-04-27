from command_line import load_program_from_args, make_rlc_argparse
from rlc import Program
import os
import sys

from ml.ppg.envs import RLCMultiEnv, exit_on_invalid_env, get_num_players

from tensorboard.program import TensorBoard
from ml.ppg.train import train, make_model
import ml.ppg.torch_util as tu
import ml.ppg.tree_util as tree_util
import torch


def print_probs(program, model, env):
    print("--------- probs --------------")
    rnn_state = model.initial_state(env.num)
    lastrew, ob, first = tree_util.tree_map(tu.np2th, env.observe())
    action_mask = tu.np2th(env.action_mask())
    logp = model.act_logp(ob, first, rnn_state, action_mask)
    sorted, indicies = torch.sort(logp.probs[0, 0, 0, :], descending=True)
    current = 0
    for prob, index in zip(sorted, indicies):
        if prob == 0:
            break
        action = env.games[0].actions()[index.item()]
        print(f"{current}: {action} {prob.item()*100}%")
        current = current + 1

    print("------------------------------")
    return indicies


def select_action(program, model, env):
    rnn_state = model.initial_state(env.num)
    lastrew, ob, first = tree_util.tree_map(tu.np2th, env.observe())
    action_mask = tu.np2th(env.action_mask())
    act, state_out, _ = model.act(ob, first, rnn_state, action_mask)
    action_index = act[0]
    return action_index


def main():
    parser = make_rlc_argparse(
        "probs", description="print the probability of executing each action"
    )
    parser.add_argument("checkpoint", type=str, nargs="?", default=None)
    parser.add_argument("--pretty-print", "-pp", action="store_true", default=False)
    parser.add_argument("--state", "-s", default="")
    parser.add_argument("--iterations", default=1, type=int)

    args = parser.parse_args()
    with load_program_from_args(args, optimize=True) as program:
        exit_on_invalid_env(program)
        env = RLCMultiEnv(program)
        model = make_model(env, path_to_weights=args.checkpoint, arch="shared")

        os.system("cls||clear")
        for iteration in range(args.iterations):
            i = 0
            while True:
                if args.pretty_print:
                    os.system("cls||clear")
                print(f"---------- {i} : p{env.current_player_one(0)} ------------")
                if args.pretty_print:
                    env.pretty_print(0)
                else:
                    env.print(0)
                actions = print_probs(program, model, env)

                action = None
                if args.pretty_print:
                    user_input = input()
                    if user_input.isnumeric():
                        action = actions[int(user_input) : int(user_input) + 1]
                    else:
                        action = select_action(program, model, env)
                else:
                    action = select_action(program, model, env)
                print(env.games[0].actions()[action])

                env.step_one(0, action)
                i = i + 1
                if env.first_move[0]:
                    break

            print("final rewards:", env.rew)


if __name__ == "__main__":
    main()
