#
# Copyright 2025 Massimo Fioravanti
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
from command_line import (
    load_program_from_args,
    make_rlc_argparse,
    get_included_conents_from_args,
)
from rlc import State, Program, make_llm, run_game
from sys import stdout
from os import devnull


def main():
    parser = make_rlc_argparse("llmplayer", description="have a llm play the game")
    parser.add_argument(
        "message",
        type=str,
        default="",
        nargs="?",
    )
    parser.add_argument("-o", "--output", type=str, default="-", nargs="?")
    parser.add_argument("--trace-output", type=str, default="-", nargs="?")
    parser.add_argument(
        "--gemini-stateless",
        type=bool,
        default=True,
        nargs="?",
        help="Use gemini but send only the current state, and do not keep track of past knowledge",
    )
    parser.add_argument(
        "--gemini-statefull",
        action="store_true",
        help="Use gemini and keep track of past actions",
    )
    parser.add_argument(
        "--ollama-local", action="store_true", help="Use ollama locally"
    )

    args = parser.parse_args()

    output = open(args.output, "w+") if args.output != "-" else open(devnull, "w")
    trace_output = open(args.trace_output, "w+") if args.trace_output != "-" else open(devnull, "w")
    rules = get_included_conents_from_args(args)
    with load_program_from_args(args, optimize=True) as program:
        llm = make_llm(args, program)
        for action, thought in run_game(
            llm=llm,
            program=program,
            rules=rules,
            output=output,
            trace_output=trace_output,
        ):
            print(thought)
            print(program.to_string(action))


if __name__ == "__main__":
    main()
