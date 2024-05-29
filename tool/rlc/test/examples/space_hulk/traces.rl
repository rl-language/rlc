# RUN: python %pyscript/action.py %S/main.rl -i %stdlib --rlc rlc %s

# move right
place_blip {index: 0}
place_blip {index: 0}
toggle_door {unit_id: 1}
begin_move {unit_id: 1}
move {absolute_direction: 1}
end_move {}
do_nothing {}

# move right
begin_move {unit_id: 1}
move {absolute_direction: 1}
end_move {}
do_nothing {}

