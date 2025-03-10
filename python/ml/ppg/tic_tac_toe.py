import numpy as np
from gym3 import Env, types, Wrapper


class TicTacToeGame:
    EMPTY = 0
    X = 1
    O = 2

    def __init__(self, seed=None):
        self.rng = np.random.default_rng(seed)
        self.board = None
        self.reset()

    def reset(self):
        self.board = [self.EMPTY] * 9
        self.current_player = self.X  # X always starts

    def get_state(self):
        # Return the board as a numpy array, shape (9,)
        return np.array(self.board, dtype=np.int64).reshape((9, 1, 1))

    def action_mask(self):
        return np.array([self.valid_move(i) for i in range(9)], dtype=np.int8).reshape(
            (9)
        )

    def valid_move(self, position):
        if position < 0 or position >= 9:
            return False
        if self.board[position] != self.EMPTY:
            return False
        return True

    def make_move(self, position):
        """
        Attempt to place current_player's mark at position.
        Returns:
            valid: bool indicating if the move was valid
        """
        # assert self.valid_move(position)
        self.board[position] = self.current_player
        self.switch_player()
        return True

    def switch_player(self):
        self.current_player = self.O if self.current_player == self.X else self.X

    def check_winner(self, mark):
        wins = [
            (0, 1, 2),
            (3, 4, 5),
            (6, 7, 8),  # rows
            (0, 3, 6),
            (1, 4, 7),
            (2, 5, 8),  # cols
            (0, 4, 8),
            (2, 4, 6),  # diagonals
        ]
        for a, b, c in wins:
            if (
                self.board[a] == mark
                and self.board[b] == mark
                and self.board[c] == mark
            ):
                return True
        return False

    def is_draw(self):
        return all(cell != self.EMPTY for cell in self.board)

    def get_available_moves(self):
        return [i for i, cell in enumerate(self.board) if cell == self.EMPTY]

    def opponent_move(self):
        # Opponent is always O.
        # Simple random move
        moves = self.get_available_moves()
        if moves:
            move = self.rng.choice(moves)
            self.board[move] = self.O


class TicTacToeEnv(Env):
    def __init__(self, num=1, seed=None):
        self.num = num

        # Observation and action spaces
        self.ob_space = types.TensorType(types.Discrete(n=3), shape=(9, 1, 1))
        self.ac_space = types.TensorType(types.Discrete(n=9), shape=(1,))

        self.games = [TicTacToeGame(seed=seed) for _ in range(num)]
        self.done = np.ones(self.num, dtype=bool)
        self.rew = np.zeros(self.num, dtype=np.float32)

        super().__init__(ob_space=self.ob_space, ac_space=self.ac_space, num=self.num)

    def action_mask(self):
        return np.array([g.action_mask() for g in self.games])

    def observe(self):
        obs = np.array([g.get_state() for g in self.games])
        return self.rew, obs, self.done

    def act(self, ac):
        self.step(ac)

    def step(self, ac):
        for i, action in enumerate(ac):
            game = self.games[i]
            valid = game.make_move(action[0])

            if game.check_winner(TicTacToeGame.O):
                self.rew[i] = 1.0
                self.done[i] = True
                game.reset()
            elif game.check_winner(TicTacToeGame.X):
                self.rew[i] = 0.0
                self.done[i] = True
                game.reset()
            elif game.is_draw():
                self.done[i] = True
                self.rew[i] = 0.0
                game.reset()
            else:
                self.rew[i] = 0.0
                self.done[i] = False

        return self.observe()


if __name__ == "__main__":
    env = TicTacToeEnv(num=1)
    done = False
    obs = env.observe()

    # Example interaction:
    for _ in range(20):
        # Random actions for demonstration
        action = env.games[0].rng.integers(0, 9, size=(1,))
        rew, obs, done = env.step(action)
        print("Action:", action)
        print("Reward:", rew)
        print("Done:", done)
        print("Board:\n", obs[0].reshape(3, 3))
        print("----")
        if done[0]:
            print("Episode finished, resetting...")
            print("----")
