#
# Part of the RLC Project, under the MIT license. 
# See stdlib/LICENSE.TXT for license information.
# SPDX-License-Identifier: MIT
#

import collections.vector
import serialization.to_byte_vector

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
    while read_bytes != input.size():
        from_byte_vector(variant, input, read_bytes)
        if can apply(variant, state):
            apply(variant, state)

fun<FrameType, AllActionsVariant> parse_and_execute(FrameType state, AllActionsVariant variant, Vector<Byte> input):
    parse_and_execute(state, variant, input, 0)
