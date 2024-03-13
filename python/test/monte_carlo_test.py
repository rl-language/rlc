#
# This file is part of the RLC project.
#
# RLC is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License version 2 as published by the Free Software Foundation.
#
# RLC is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along with RLC. If not, see <https://www.gnu.org/licenses/>.
#
from solvers import ml_mcts, play_game
from random import uniform
from copy import deepcopy


class ActionStub:
    def __init__(self, implementation, x, y):
        self.implementation = implementation
        self.x = x
        self.y = y

    def can_run(self, state):
        assert self.x < 3 and self.x >= 0 and self.y < 3 and self.y >= 0
        return state.board[self.x][self.y] == 0

    def run(self, state):
        state.mark(self.x, self.y)
        return state


class ActionStubType:
    def __init__(self, implementation):
        self.implementation = implementation

    def get_arg_min_maxs(self):
        return [(0, 3), (0, 3)]

    def materialize(self, *args):
        return ActionStub(self.implementation, args[0], args[1])

class Tris:
    def __init__(self):
        self.board = [[0 for _ in range(3)] for _ in range(3)]
        self.turn_count = 0
        self.current_player = 1
        self.simulation = self

    def copy(self):
        return deepcopy(self)

    def mark(self, x, y):
        if self.is_done():
            return None
        assert self.board[x][y] == 0
        self.board[x][y] = self.current_player
        self.current_player = 1 if self.current_player == 2 else 2
        self.turn_count = self.turn_count + 1
        return self

    def is_done(self) -> bool:
        if self.turn_count == 9:
            return True
        for row in self.board:
            if all(x == 1 or x == 2 for x in row):
                return True
        return False

    def score(self) -> int:
        return 9 - self.turn_count

    @property
    def actions(self):
        return [ActionStubType(self)]


class TrisMoveOracle:
    def __call__(self, states: Tris):
        return [self.single_oracle(state) for state in states]

    def single_oracle(self, state):
        for y in range(3):
            for x in range(3):
                if state.board[x][y] == 0:
                    return ActionStub(state, x, y)
        assert False


def test_monte_carlo():
    oracle = TrisMoveOracle()
    results = play_game(Tris(), oracle, 9)
    (state, move_and_score) = results[0]
    assert max(x.score for x in move_and_score) == 4
