# RUN: python %pyscript/test.py %s --stdlib %stdlib --rlc rlc
# RUN: python %pyscript/learn.py %s --stdlib %stdlib --rlc rlc --total-steps 200 --model-save-frequency 1 -o %t --envs 1 --steps-per-env 100
# RUN: python %pyscript/play.py %s --stdlib %stdlib --rlc rlc %t
# RUN: python %pyscript/probs.py %s %t --stdlib %stdlib --rlc rlc 

@classes
act play() -> Game:
    frm score = 0.0
    act win(Bool do_it)
    if do_it:
        score = 1.0
