#
# Part of the RLC Project, under the MIT license. 
# See stdlib/LICENSE.TXT for license information.
# SPDX-License-Identifier: MIT
#

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
