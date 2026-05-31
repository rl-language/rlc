import json
import os
import re
import sys
import tempfile
from collections import defaultdict
from random import choice
from sys import stdout
from textwrap import dedent

import openai
import xgrammar as xgr

from rlc import Program, State
from rlc.utils import rl_string_to_python, rl_vector_of_strings_to_python


class Ollama:
    def __init__(self, program: Program):
        from ollama import generate

        self.contexts = [None for x in range(program.module.get_num_players())]
        self.generate = generate

    def chat(self, message: str, player_id: int, *args, **kwargs) -> str:
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

    def chat(self, message: str, player_id: int, *args, **kwargs) -> str:
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

    def chat(self, message: str, player_id: int, *args, **kwargs) -> str:
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


class LlamaCpp:
    def __init__(self, program: Program, game_name: str, model_name: str = "model"):
        self.client = openai.Client(api_key="...", base_url="http://localhost:8000/v1")
        self.model_name = model_name
        self.program = program
        # init chat histories for each player
        self.chats = [
            [{
                "role": "system",
                "content": (f"You are an agent that plays in a reinforcement learning enviroment, you're player id is {x}, the game is {game_name}. Your goal is to win the game, by selecting the best action (among the legal ones) at each turn.\n"
                "INSTRUCTIONS:\n- First, reason about your strategy (max 300 tokens).\n- Once you decided, write the chosen action.\nOutput only the action you choose, nothing else, no explanations, no comments, just the action.\n")
            }] # this system prompt is generic because it's supposed to work for any game, the specific info it contains are player_id and game_name
            for x in range(program.module.get_num_players())
        ]

    def chat(self, message: str, player_id: int, state: State) -> str:
        """Interacts with the LLM to get the action to play given the current state.

        The interaction is done in two steps:
        1. We first send the message to the model and let it reason about the current state
        2. We then send the message again, this time with a grammar that constrains the output of the model to be one of the possible actions
        """
        chat_messages = self.chats[player_id]
        self.chats[player_id].append({"role": "user", "content": message})

        ### CLEAN HISTORY FROM OLD MESSAGES ###
        # to avoid hitting the context window limit, we keep in the history only the last max_turns_in_history interactions
        max_turns_in_history = 10
        previous_turns_in_history = 0
        for i in range(1, len(chat_messages)-1, 2):
            if chat_messages[i]["role"] == "system":
                break # found sys error of current turn, stop counting
            previous_turns_in_history += 1
        if previous_turns_in_history > max_turns_in_history:
            # keep system message and last max_turns_in_history interactions
            n_removed_interactions = previous_turns_in_history - max_turns_in_history
            chat_messages = [chat_messages[0]] + \
                [{"role": "system", "content": f"Previous {n_removed_interactions} interactions removed to keep the context within the limit."}] +\
                chat_messages[n_removed_interactions * 2 + 1:]


        ### 1. LET THE MODEL REASON ###
        # do a first call just to let the model reason about the current state and decide what action to take
        # no action is expected to be output, as we can't constrain the output yet
        chat_response_reasoning = self.client.chat.completions.create(
            model=self.model_name,
            messages=chat_messages,
            max_tokens=768 + 23, # reasononing budget tokens + reasoning budget message tokens
        )
        if hasattr(chat_response_reasoning.choices[0].message, "reasoning_content"):
            ### 2. CONSTRAIN THE MODEL OUTPUT WITH A GRAMMAR ###
            reasoning_str = chat_response_reasoning.choices[0].message.reasoning_content.strip()
            start_reasoning_str = "<|channel>thought\n"
            end_reasoning_str = "\n<channel|>"

            # prepare grammar from regex to contrain the action output of the model
            allowed_actions_regex = create_regex_for_constrained_generation(self.program.module)
            g = xgr.Grammar.from_regex(allowed_actions_regex)
            gbnf = str(g)
            extra_body = {
                "grammar": gbnf,
                "chat_template_kwargs": {
                    "enable_thinking": False,
                }
            }
            self.chats[player_id].append({"role": "assistant", "content": start_reasoning_str + reasoning_str + end_reasoning_str})
            chat_response = self.client.chat.completions.create(
                model=self.model_name,
                messages=chat_messages,
                max_tokens=1500,
                extra_body=extra_body,
            )
            answer = chat_response.choices[
                0
            ].message.content.strip()
            # remove the reasoning from the chat history
            if hasattr(chat_response_reasoning.choices[0].message, "reasoning_content"):
                self.chats[player_id].pop()
        else:
            # sometimes the model forgets to reason and just outputs the action
            answer = chat_response_reasoning.choices[0].message.content.strip()

        self.chats[player_id].append({"role": "assistant", "content": answer})

        return answer

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


def make_llm(args, program, game_name: str):
    if args.ollama_local:
        return Ollama(program)
    if args.gemini_statefull:
        return Gemini(program)
    if args.gemini_stateless:
        return GeminiStateless(program)
    if args.llamacpp:
        return LlamaCpp(program, game_name)
    return None


def run_game(llm, game_name: str, program: Program, rules: str, output=stdout, trace_output=stdout):
    if isinstance(llm, LlamaCpp):
        yield from run_game_with_llamacpp(llm, game_name, program, rules, output, trace_output)
        return
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

def run_game_with_llamacpp(llm, game_name: str, program: Program, rules: str, output=stdout, trace_output=stdout):
    prompt_message = "The following is the current state, followed by the actions you can take.\n"
    num_players = program.module.get_num_players()
    state = program.start()
    for x in solve_randomness(program, state, trace_output):
        yield x
    
    output.write(rules + "\n")
    # for x in range(num_players):
    #     message = (
    #         f"Here are the rules of the game: read them carefully. You will be prompted to play a game as player {x} against the opponent.\n```"
    #         + rules + "\n```\n\nSummarize the rules (be concise) and formulate a strategy to play the game. Keep it short."
    #     )
    #     answer_to_rules = llm.chat(message=message, player_id=x)
    #     output.write(answer_to_rules)

    action_format_str = dedent("""
    ```json
    {
        "action_name": "$ACTION_NAME",
        "parameters": {
            "$PARAM_NAME1_FLOAT": $VALUE1,
            "$PARAM_NAME2_STR": "$VALUE2",
            "$PARAM_NAME3_BOOL": $VALUE3
        }
    }
    ```""")

    output.write(f"starting game {game_name}\n")
    turn = 0

    while not state.is_done():
        current_player = program.module.get_current_player(state.state)
        output.write(f"---------- TURN {turn}, PLAYER {current_player} ----------\n")
        output.write(capture_stdout(state.pretty_print) + "\n")

        output.write("CURRENT_PLAYER " + str(current_player) + "\n")
        state_str = capture_stdout(state.pretty_print)
        message = prompt_message + "\nCURRENT STATE:\n" + state_str + "\n"
        message += "\nLEGAL ACTIONS:\n"
        message += json.dumps(list(map(json.loads, rl_vector_of_strings_to_python(program.module.describe_actions()))), indent=4) + "\n"
        
        message += f"\nSelect your action by answering with one of above actions, using the format:\n{action_format_str}"
        
        output.write(message + "\n")
        output.flush()

        answer = llm.chat(message=message, player_id=current_player, state=state)
        action_index = get_action_index_from_llamacpp_answer(answer, state)
        output.write(answer + "\n")
        output.flush()
        n_attempts = 1
        max_attempts = 20
        while action_index == -1 or not state.can_apply(state.actions[action_index]):
            n_attempts += 1
            if n_attempts > max_attempts:
                raise Exception(f"LLM failed to provide a valid action after {max_attempts} attempts, aborting the game.")
            error_msg = "Failed to apply action, "
            if action_index == -1:
                error_msg += f"unable to parse answer. Please answer with format:\n{action_format_str}"
            else:
                 error_msg += "the action you selected is not legal in the current state."
            output.write(error_msg + "\n")
            llm.chats[current_player].append({"role": "system", "content": error_msg})
            answer = llm.chat(message=message, player_id=current_player, state=state)
            output.write(answer + "\n")
            output.flush()
            action_index = get_action_index_from_llamacpp_answer(answer, state)

        action = state.actions[action_index]
        trace_output.write(str(action) + "\n")
        trace_output.flush()

        output.write(f"player {current_player} chose action {action_index}: {str(action).strip()} ({n_attempts} attempts)\n")
        output.flush()

        state.step(action)
        yield (action, answer)
        for x in solve_randomness(program, state, trace_output):
            yield x

        if n_attempts > 1:
            # clear wrong attempts from the chat history
            for _ in range(3 * (n_attempts - 1)): # for each failed attempt, remove system, user and assistant messages
                llm.chats[current_player].pop(-3) # leave last two messages there
        turn += 1
    
    output.write(f"game {game_name} ended\n")
    output.write(
        "FINAL SCORE: "
        + str([program.module.score(state.state, x) for x in range(num_players)])
    )
    output.flush()


def capture_stdout(callable, *args, **kwargs) -> str:
    original_stdout_fd = os.dup(sys.stdout.fileno())
    with tempfile.TemporaryFile(mode="w+b") as tmp:
        try:
            # Redirect stdout to the temporary file
            os.dup2(tmp.fileno(), sys.stdout.fileno())

            callable(*args, **kwargs)
            sys.stdout.flush()

            # Seek to the beginning and read
            tmp.seek(0)
            output_str = tmp.read().decode()
            return output_str
        finally:
            # Always restore original stdout, even if error happens
            os.dup2(original_stdout_fd, sys.stdout.fileno())


def create_regex_for_constrained_generation(program_module):
    """Creates a regex that matches the possible actions for the game, to be used for constrained generation with LlamaCpp.
    
    The idea is to generete a regex for the single action starting from the description returned from rlc, than combine the regexes of all the actions into one, using the OR operator."""
    actions_json_list = rl_vector_of_strings_to_python(program_module.describe_actions())
    actions_regex_list = []
    for action_json in actions_json_list:
        action = json.loads(action_json)
        params = action['parameters_description']
        new_line = ",\n                    "
        action_regex = dedent(f"""
        ```json
        {{
            "action_name": "{action['action_name']}",
            "parameters": {{
                {new_line.join(f'"{param["name"]}": {param["regex"]}' for param in params)}
            }}
        }}
        ```
        """).lstrip().replace("{", r"\{").replace("}", r"\}")
        actions_regex_list.append(action_regex)
    regex = "(" + "|".join(set(actions_regex_list)) + ")"
    return regex

def get_action_index_from_llamacpp_answer(answer: str, state) -> int:
    answer_json = json.loads(answer.split("```json")[-1].split("```")[0])
    chosen_action_name = answer_json["action_name"]
    chosen_action_params = answer_json.get("parameters", {})
    try:
        # find the index of what action was chosen by the LLM
        action_index = -1
        for i, action_i in enumerate(state.actions):
            action_i_name = str(action_i).split("{")[0].strip()
            action_i_params_str = (re.search("({.*})", str(action_i)).group(0))
            action_i_params_json_str = re.sub(r"([\w_]+):", r'"\1":', action_i_params_str) # add double quotes
            action_i_params = json.loads(action_i_params_json_str)
            if action_i_name == chosen_action_name and action_i_params == chosen_action_params:
                action_index = i
                break
    except ValueError:
        action_index = -1
    return action_index