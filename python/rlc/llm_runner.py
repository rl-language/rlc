from rlc import State, Program
from sys import stdout
from random import choice


class Ollama:
    def __init__(self, program: Program):
        from ollama import generate

        self.contexts = [None for x in range(program.module.get_num_players())]
        self.generate = generate

    def chat(self, message: str, player_id: int) -> str:
        answer = self.generate(
            model="deepseek-r1:14b", prompt=message, context=self.contexts[player_id]
        )
        self.contexts[player_id] = answer["context"]
        return answer["response"]


class Gemini:
    def __init__(self, program: Program, model="gemini-2.0-flash"):
        from google import genai

        self.client = genai.Client()
        self.model = model
        self.chats = [
            self.client.chats.create(
                model=self.model,
                config={
                    "system_instruction": f"You are a player of reinforcement learning enviroments, you will recieve the rules of the environment, your player id is {x}, the state of the environment, and the actions you take in the game. The game rules are correct, if you think there is a error, it is because you made a mistake in reading the code."
                },
            )
            for x in range(program.module.get_num_players())
        ]

    def chat(self, message: str, player_id: int) -> str:
        response = self.chats[player_id].send_message(
            f"You are player {player_id}. Notice the game code may imply that your id is mapped onto other numbers in the game state. "
            + message
        )
        return response.text


class GeminiStateless:
    def __init__(self, program: Program, model="gemini-2.5-flash-preview-04-17", first_message=None):
        from google import genai

        self.client = genai.Client()
        self.model = model
        self.chats = [
            {
                "system_instruction": f"You are a player of reinforcement learning enviroments, you will recieve the rules of the environment, your player id is {x}, the state of the environment, and the actions you take in the game. The game rules are correct, if you think there is a error, it is because you made a mistake in reading the code. You are player {x}. Notice the game code may imply that your id is mapped onto other numbers in the game state. "
            }
            for x in range(program.module.get_num_players())
        ]
        self.first_messages = [first_message for x in range(program.module.get_num_players())]

    def chat(self, message: str, player_id: int) -> str:
        from google import genai
        if self.first_messages[player_id] == None:
            self.first_messages[player_id] = message

            response = self.client.models.generate_content(
                model=self.model,
                contents=message,
            )
        else:
            response = self.client.models.generate_content(
                model=self.model,
                contents=self.first_messages[player_id] + "\n" + message,
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
    except:
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
    current_player = program.module.get_current_player(state.state)
    while len(state.legal_actions) == 1 or current_player == -1 and not state.is_done():
        action = choice(state.legal_actions)
        trace_output.write(str(action) + "\n")
        trace_output.flush()
        state.step(action)
        current_player = program.module.get_current_player(state.state)
        yield (action, "")


def make_llm(args, program):
    if args.ollama_local:
        return Ollama(program)
    if args.gemini_statefull:
        return Gemini(program)
    if args.gemini_stateless:
        return GeminiStateless(program)
    return None


def run_game(llm, program: Program, rules: str, output=stdout, trace_output=stdout):
    prompt_message = "The following is the current state, follwed by the actions you can take. Terminate your message with the number of the action you want to take, with the following sintax ACTION: INDEX. Explain your decisions."
    num_players = program.module.get_num_players()
    state = program.start()
    for x in solve_randomness(program, state, trace_output):
        yield x
    output.write(rules)
    for x in range(num_players):
        message = (
            f"Here are the rules of a game, read them, understand them and formulate a strategy to play the game. You will be prompted to play a game as player {x} against the opponent.\n "
            + rules
        )
        output.write(llm.chat(message=message, player_id=x))

    while not state.is_done():
        current_player = program.module.get_current_player(state.state)

        output.write("CURRENT_PLAYER " + str(current_player))
        message = prompt_message + "\n" + str(state) + "\n"
        for index in state.legal_actions_indicies:
            action = state.actions[index]
            message = message + str(index) + ": " + str(action) + "\n"

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
        trace_output.write(str(action) + "\n")
        trace_output.flush()
        state.step(action)
        yield (action, decision)
        for x in solve_randomness(program, state, trace_output):
            yield x
    output.write(
        "FINAL SCORE: "
        + str([program.module.score(state.state, x) for x in range(num_players)])
    )
