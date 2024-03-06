#
# Part of the RLC Project, under the MIT license. 
# See stdlib/LICENSE.TXT for license information.
# SPDX-License-Identifier: MIT
#

import collections.vector
import serialization.to_byte_vector
import serialization.print

trait<FrameType, ActionType> ApplicableTo:
    fun apply(ActionType action, FrameType frame)

fun<FrameType, ActionType> can_apply_impl(ActionType action, FrameType frame) -> Bool:
    for alternative of action:
        using Type = type(alternative)
        if action is Type:
            if action is ApplicableTo<FrameType>:
                return can action.apply(frame)
    return false

fun<FrameType, ActionType> apply(ActionType action, FrameType frame) { can_apply_impl(action, frame) }: 
    for alternative of action:
        using Type = type(alternative)
        if action is Type:
            if action is ApplicableTo<FrameType>:
                action.apply(frame)


fun<FrameType, AllActionsVariant> parse_and_execute(FrameType state, AllActionsVariant variant, Vector<Byte> input, Int read_bytes):
    while read_bytes + 8 <= input.size():
        if from_byte_vector(variant, input, read_bytes):
            if can apply(variant, state):
                apply(variant, state)

fun<AllActionsVariant> parse_actions(AllActionsVariant variant, Vector<Byte> input, Int read_bytes) -> Vector<AllActionsVariant>:
    let to_return : Vector<AllActionsVariant>
    while read_bytes + 8 < input.size():
        if from_byte_vector(variant, input, read_bytes):
            to_return.append(variant)
    return to_return

fun<AllActionsVariant> parse_actions(AllActionsVariant variant, Vector<Byte> input) -> Vector<AllActionsVariant>:
    return parse_actions(variant, input, 0)

fun<FrameType, AllActionsVariant> parse_and_execute(FrameType state, AllActionsVariant variant, Vector<Byte> input):
    parse_and_execute(state, variant, input, 0)

fun<FrameType, AllActionsVariant> gen_python_methods(FrameType state, AllActionsVariant variant):
    let state : FrameType
    let serialized = as_byte_vector(state)
    from_byte_vector(state, serialized)
    let x : AllActionsVariant
    apply(x, state)
    to_string(state)
    to_string(x)
    from_string(x, ""s)
    from_string(state, ""s)
    parse_actions(x, serialized)
