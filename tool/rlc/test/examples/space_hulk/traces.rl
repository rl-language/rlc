# RUN: python %pyscript/action.py --source %S/main.rl -i %stdlib --rlc rlc %s

# move right
begin_move {unit_id: 0}
move {absolute_direction: 1}
end_move {}

# move right
begin_move {unit_id: 0}
move {absolute_direction: 1}
end_move {}

# shoot first gsc
shoot {unit_id: 0, target_id: 1}
