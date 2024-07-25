# RUN: python %pyscript/test.py %s --stdlib %stdlib --rlc rlc
# RUN: python %pyscript/learn.py %s --stdlib %stdlib --rlc rlc --total-train-iterations=1 -o %t
# RUN: python %pyscript/play.py %s --stdlib %stdlib --rlc rlc %t
# RUN: python %pyscript/probs.py %s --stdlib %stdlib --rlc rlc %t
act play() -> Game:
    frm score = 0.0
    act win(Bool do_it)
    if do_it:
        score = 1.0
