#
# This file is part of the RLC project.
#
# RLC is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License version 2 as published by the Free Software Foundation.
#
# RLC is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along with RLC. If not, see <https://www.gnu.org/licenses/>.
#
from command_line import load_simulation_from_args, make_rlc_argparse, load_dataset


def main():
    parser = make_rlc_argparse("run", description="runs a action of the simulation")
    parser.add_argument("dataset", type=str)
    parser.add_argument("--summary-only", "-s", action="store_true")
    cl_args = parser.parse_args()
    sim = load_simulation_from_args(cl_args)

    dataset = load_dataset(cl_args.dataset)

    total_actions = 0
    total_score = 0.0
    for (state_bytes, actions_and_scores) in dataset:
        state = sim.start(["play"])
        state.from_byte_vector(state_bytes)
        if not cl_args.summary_only:
            state.dump()
        for (action, score) in actions_and_scores:
            total_actions = total_actions + 1
            total_score = total_score + score
            (action, args) = state.parse_action_from_raw_bytes(action)
            if not cl_args.summary_only:
                print(
                    f"\t{action.name}({', '.join(str(arg) for arg in args)}): valid: {action.can_run(state, *args)}, score: {str(score)}"
                )

    print("summary:")
    print(f"\tstate_count: {str(len(dataset))}")
    print(f"\taction_count: {str(total_actions)}")
    print(f"\taverage_score: {str(total_score / total_actions)}")


if __name__ == "__main__":
    main()
