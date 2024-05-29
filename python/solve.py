#
# This file is part of the RLC project.
#
# RLC is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License version 2 as published by the Free Software Foundation.
#
# RLC is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along with RLC. If not, see <https://www.gnu.org/licenses/>.
#
import argparse
import random
from loader import Simulation, compile
from shutil import which
from ml.raylib.environment import RLCEnvironment
from command_line import load_simulation_from_args, make_rlc_argparse


def main():
    parser = make_rlc_argparse("solve", description="runs a action of the simulation")
    args = parser.parse_args()
    (_, wrapper_path, directory) = load_simulation_from_args(args)

    env = RLCEnvironment(wrapper_path=wrapper_path)
    env.wrapper.functions.print(env.state)

    while True:
        obs, reward, done, truncated, info = env.step(
            [random.choice(env.legal_actions) for i in range(env.num_agents)]
        )
        if done["__all__"] or truncated["__all__"]:
            break

    env.wrapper.functions.print(env.state)

    if directory != None:
        directory.cleanup()


if __name__ == "__main__":
    main()
