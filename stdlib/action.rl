#
# Part of the RLC Project, under the MIT license. 
# See stdlib/LICENSE.TXT for license information.
# SPDX-License-Identifier: MIT
#

trait<FrameType, ActionType> ApplicableTo:
    fun apply(ActionType action, FrameType frame)


fun<FrameType, ActionType> apply(ActionType action, FrameType frame): 
    for alternative of action:
        using Type = type(alternative)
        if action is Type:
            if action is ApplicableTo<FrameType>:
                action.apply(frame)
