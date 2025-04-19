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
from command_line import load_program_from_args, make_rlc_argparse, get_included_conents_from_args
from rlc import State, Program
from sys import stdout
from random import choice


class Ollama:
    def __init__(self, program: Program):
        from ollama import generate

        self.contexts = [None for x in range(program.functions.get_num_players())]
        self.generate = generate

    def chat(self, message: str, player_id: int) -> str:
        answer = self.generate(
            model="deepseek-r1:14b", prompt=message, context=self.contexts[player_id]
        )
        self.contexts[player_id] = answer["context"]
        return answer["response"]


class Gemini:
    def __init__(self, program: Program):
        from google import genai

        self.client = genai.Client()
        self.model = "gemini-2.0-flash"
        self.chats = [
            self.client.chats.create(
                model=self.model,
                config={
                    "system_instruction": f"You are a player of reinforcement learning enviroments, you will recieve the rules of the environment, your player id is {x}, the state of the environment, and the actions you take in the game. The game rules are correct, if you think there is a error, it is because you made a mistake in reading the code."
                },
            )
            for x in range(program.functions.get_num_players())
        ]

    def chat(self, message: str, player_id: int) -> str:
        response = self.chats[player_id].send_message(
            f"You are player {player_id}. Notice the game code may imply that your id is mapped onto other numbers in the game state. "
            + message
        )
        return response.text


class GeminiStateless:
    def __init__(self, program: Program):
        from google import genai

        self.client = genai.Client()
        self.model = "gemini-2.0-flash"
        self.chats = [
            {
                "system_instruction": f"You are a player of reinforcement learning enviroments, you will recieve the rules of the environment, your player id is {x}, the state of the environment, and the actions you take in the game. The game rules are correct, if you think there is a error, it is because you made a mistake in reading the code. You are player {x}. Notice the game code may imply that your id is mapped onto other numbers in the game state. "
            }
            for x in range(program.functions.get_num_players())
        ]

    def chat(self, message: str, player_id: int) -> str:
        from google import genai

        response = self.client.models.generate_content(
            model=self.model,
            contents=message,
        )
        return response.text


def extract_index(string: str):
    position = string.rfind("action:")
    if position == -1:
        return None
    position = position + 7

    while string[position] == " ":
        position = position + 1

    end = position + 1
    while end < len(string) and string[end].isnumeric():
        end = end + 1
    try:
        return int(string[position:end])
    except e:
        return None


def get_action_from_string(string: str, state):
    index = extract_index(string.lower())
    if index == None:
        return False
    if index >= len(state.actions) or 0 > index:
        return False
    action = state.actions[index]
    if not state.can_apply(action):
        return False
    return action


def solve_randomness(program: Program, state: State, trace_output):
    current_player = program.functions.get_current_player(state.state)
    while len(state.legal_actions) == 1 or current_player == -1 and not state.is_done():
        action = choice(state.legal_actions)
        trace_output.write(program.to_string(action) + "\n")
        trace_output.flush()
        state.step(action)
        current_player = program.functions.get_current_player(state.state)


def make_llm(args, program):
    if args.ollama_local:
        return Ollama(program)
    if args.gemini_statefull:
        return Gemini(program)
    if args.gemini_stateless:
        return GeminiStateless(program)
    return None


def main():
    parser = make_rlc_argparse("llmplayer", description="have a llm play the game")
    parser.add_argument(
        "message",
        type=str,
        default="The following is the current state of the game tic tac toe, follwed by the actions you can take. Terminate your message with the number of the action you want to take, with the following sintax ACTION: INDEX. Explain your decisions.",
        nargs="?",
    )
    parser.add_argument("--o", type=str, default="-", nargs="?")
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

    output = open(args.o, "w+") if args.o != "-" else stdout
    trace_output = open(args.trace_output, "w+") if args.trace_output != "-" else stdout
    with load_program_from_args(args, optimize=True) as program:
        num_players = program.functions.get_num_players()
        rules = get_included_conents_from_args(args)
        state = program.start()
        solve_randomness(program, state, trace_output)
        llm = make_llm(args, program)
        output.write(rules)
        for x in range(num_players):
            message = (
                f"Here are the rules of a game, read them, understand them and formulate a strategy to play the game. You will be prompted to play a game as player {x} against the opponent.\n "
                + rules
            )
            output.write(llm.chat(message=message, player_id=x))

        while not state.is_done():
            current_player = program.functions.get_current_player(state.state)

            output.write("CURRENT_PLAYER " + str(current_player))
            message = args.message + "\n" + state.to_string() + "\n"
            for index in state.legal_actions_indicies:
                action = state.actions[index]
                message = message + str(index) + ": " + program.to_string(action) + "\n"

            output.write(message)

            decision = llm.chat(message=message, player_id=current_player)
            output.write(decision)
            while not get_action_from_string(decision, state):
                error_msg = (
                    "Failed to find ACTION: INDEX in the message, please answer with ACTION: INDEX\n"
                    + message
                )
                output.write(error_msg)
                decision = llm.chat(message=message, player_id=current_player)
                output.write(decision)

            action = get_action_from_string(decision, state)
            trace_output.write(program.to_string(action) + "\n")
            trace_output.flush()
            state.step(action)
            solve_randomness(program, state, trace_output)
        output.write(
            "FINAL SCORE: "
            + str([program.functions.score(state.state, x) for x in range(num_players)])
        )


if __name__ == "__main__":
    main()
