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
from os import devnull
from pathlib import Path

from command_line import (
    get_included_conents_from_args,
    load_program_from_args,
    make_rlc_argparse,
)
from rlc import make_llm, run_game


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
        action="store_true",
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
    parser.add_argument("--llamacpp", action="store_true", help="Use llama.cpp")
    parser.add_argument("--no-reasoning", action="store_true", help="Do not ask the model to reason, just output the action")
    parser.add_argument("--no-regex", action="store_true", help="Do not use regex to constrain the model output")

    args = parser.parse_args()

    output = open(args.output, "w+") if args.output != "-" else open(devnull, "w")
    trace_output = open(args.trace_output, "w+") if args.trace_output != "-" else open(devnull, "w")
    rules = get_included_conents_from_args(args)
    game_name = Path(args.source_file).stem
    with load_program_from_args(args, optimize=True, extra_source_files=["stdlib/regex.rl"]) as program:
        llm = make_llm(args, program, game_name, should_reason=not args.no_reasoning, should_use_regex=not args.no_regex)
        for action, thought in run_game(
            llm=llm,
            game_name=game_name,
            program=program,
            rules=rules,
            output=output,
            trace_output=trace_output,
        ):
            print(f"thought: {thought}")
            print(f"action: {action}")


if __name__ == "__main__":
    main()
