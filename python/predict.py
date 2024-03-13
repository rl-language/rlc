#
# This file is part of the RLC project.
#
# RLC is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License version 2 as published by the Free Software Foundation.
#
# RLC is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along with RLC. If not, see <https://www.gnu.org/licenses/>.
#
from command_line import load_simulation_from_args, make_rlc_argparse, load_network
import torch
from ml import run_once, next_probabilities


def main():
    parser = make_rlc_argparse("solve", description="runs a action of the simulation")
    parser.add_argument("network", type=str)
    parser.add_argument("--load", "-l", type=str, default="")
    parser.add_argument("--output", "-o", type=str, default="")
    parser.add_argument("--num-bytes", "-n", type=int, default=1)
    parser.add_argument("--probabilities", "-p", action="store_true")
    cl_args = parser.parse_args()
    sim = load_simulation_from_args(cl_args)

    transformer = load_network(cl_args.network, 256, "cpu")

    state = sim.start("play")
    if cl_args.load != "":
        state.load_binary(cl_args.load)
    print(str(state))

    input = torch.LongTensor([state.as_byte_vector()]).t()

    if cl_args.probabilities:
        all_except_last = run_once(
            transformer.model,
            input,
            iterations=cl_args.num_bytes - 1,
            randomize_pick=False,
            device=transformer.device,
            ntokens=transformer.ntokens,
        )
        prediction = next_probabilities(
            transformer.model,
            input,
            all_except_last,
            device=transformer.device,
            ntokens=transformer.ntokens,
        )
        print(prediction)
        return

    prediction = run_once(
        transformer.model,
        input,
        iterations=cl_args.num_bytes,
        randomize_pick=False,
        device=transformer.device,
        ntokens=transformer.ntokens,
    )
    print(prediction)
    score = prediction[-1]
    print(f"predicted score: {score.item()}")
    (action, args) = state.parse_action_from_raw_bytes(prediction[:-1])
    print(action.name, *args)
    if action.can_run(state, *args):
        action.run(state, *args)

    state.dump()
    if cl_args.output != "":
        state.write(cl_args.output)


if __name__ == "__main__":
    main()
