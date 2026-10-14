# RUN: python %pyscript/solve.py %s --stdlib %stdlib --rlc rlc
import rng

@classes
act play() -> Game:
  subaction* config_rng = configure_rng()
  assert(config_rng.seed != 0, "rng cannot be zero")

