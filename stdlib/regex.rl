import string
import bounded_arg

import collections.vector
import enum_utils  # for s(name) if not already in scope

import serialization.print
import range


cls ParamInfo:
    String type_name
    String regex


fun describe_actions() -> Vector<String>:
    let any_action : AnyGameAction
    # AnyGameAction gets all possible actions for the game
    return create_vector_of_actions_descriptions(any_action)


# produces a vector whose elements are JSON strings describing the action schema (see also describe_single_action)
fun<AllActionsVariant> create_vector_of_actions_descriptions(AllActionsVariant variant) -> Vector<String>:
    let out : Vector<String>

    # Case 1: we have a union like AnyGameAction
    if variant is Alternative:
        for alt_name, alt_field of variant:
            # alt_name is a StringLiteral with the lowercase action name
            # Get the actual type name using get_type_name() method
            let action_name : String
            if alt_field is CustomGetTypeName:
                action_name.append(alt_field.get_type_name())
            else:
                action_name.append(alt_name)

            let line = describe_single_action(alt_field, action_name)
            out.append(line)
        return out

    # Case 2: not a union, just a single action type – treat it as one action
    let dummy : AllActionsVariant
    let line = describe_single_action(dummy, "Action"s)
    out.append(line)
    return out

# produces a JSON string describing the action schema, e.g.:
# {
#     "action_name": "$ACTION_NAME",
#     "parameters_description": [
#         {
#             "name": "$parameter_name1",
#             "type": "$parameter_type1",
#             "regex": "$VALUE1"
#         },
#         {
#             "name": "$parameter_name2",
#             "type": "$parameter_type2",
#             "regex": "$VALUE2"
#         }
#     ]
# }
fun<ActionType> describe_single_action(ActionType action, String action_name) -> String:
    let result : String
    result.append("{\n")
    result.append("  \"action_name\": \""s + action_name + "\",\n"s)
    result.append("  \"parameters_description\": [\n"s)

    let first = true
    for field_name, field of action:
        if !first:
            result.append(",\n")
        first = false

        let info = describe_param(field)
        result.append("    {\n"s)
        result.append("      \"name\": \""s + s(field_name) + "\",\n"s)   # convert field_name to String
        result.append("      \"type\": \""s + info.type_name + "\",\n"s)   # get the type of the field
        result.append("      \"regex\": \""s + info.regex + "\"\n"s)
        result.append("    }"s)
        
    result.append("\n  ]\n"s)
    result.append("}")
    return result

# this function is needed to avoid the fallback to the generic implementation
# unfortunately, for BInt right now it's required to explicitly call describe_param with the exact numbers
fun ensure_describe_param_implementations_are_instantiated() -> Int:
    # Int
    let x_int : Int
    let _ = describe_param(x_int)

    # BInt TODO find a better way to do this, instead of enumerating combinations
    let x : BInt<0, 3>
    let _ = describe_param(x)
    let x : BInt<0, 7>
    let _ = describe_param(x)
    let x : BInt<0, 9>
    let _ = describe_param(x)
    let x : BInt<0, 10>
    let _ = describe_param(x)
    let x : BInt<0, 14>
    let _ = describe_param(x)
    let x : BInt<0, 52> # range 0-51 for blackjack
    let _ = describe_param(x)
    let x : BInt<1, 10> # range 0-9 for sudoku
    let _ = describe_param(x)
    
    # ...
    return 0


# For an integer, return the regex "\d" and the name "Int"
fun describe_param(Int x) -> ParamInfo:
    let info : ParamInfo
    info.type_name = ""s
    info.type_name.append("Int"s)
    info.regex = ""s
    info.regex.append("\\d"s)
    return info


# For BInt<min,max>, valid values are from min to max-1
fun<Int min, Int max> describe_param(BInt<min, max> x) -> ParamInfo:
    let info : ParamInfo
    info.type_name = ""s
    info.type_name.append("BInt<"s)
    info.type_name.append(to_string(min))
    info.type_name.append(","s)
    info.type_name.append(to_string(max))
    info.type_name.append(">"s)

    info.regex = ""s
    info.regex.append("["s)
    info.regex.append(to_string(min))
    info.regex.append("-"s)
    info.regex.append(to_string(max - 1))
    info.regex.append("]"s)

    # TODO handle numbers with more than 1 digit
    return info



# Fallback: anything we don't know how to serialize -> ".*"
fun<T> describe_param(T x) -> ParamInfo:
    let info : ParamInfo
    info.type_name = ""s
    info.type_name.append("unknown"s)
    info.regex = ""s
    info.regex.append(".*"s)
    return info

