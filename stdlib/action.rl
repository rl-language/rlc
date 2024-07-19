#
#Copyright 2024 Massimo Fioravanti
#
#Licensed under the Apache License, Version 2.0 (the "License");
#you may not use this file except in compliance with the License.
#You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
#Unless required by applicable law or agreed to in writing, software
#distributed under the License is distributed on an "AS IS" BASIS,
#WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#See the License for the specific language governing permissions and
#limitations under the License.
#

import collections.vector
import serialization.to_byte_vector
import serialization.print
import bounded_arg
import math.numeric
import algorithms.equal

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


fun<FrameType, ActionType> apply(Vector<ActionType> action, FrameType frame) -> Bool: 
    let x = 0
    while x != action.size():
        if !can apply(action.get(x), frame):
            return false
        apply(action.get(x), frame)
        x = x + 1
    return true

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

fun<AllActionsVariant> parse_actions(AllActionsVariant variant, String input) -> Vector<AllActionsVariant>:
    let to_return : Vector<AllActionsVariant>
    let counter = 0 
    while from_string(variant, input, counter):
        print(variant)
        to_return.append(variant)
    return to_return

# parses actions taking only one byte and by taking taking the reminder of the parsed number divided by the number of actions, so that no action is ever marked as invalid
fun<AllActionsVariant> parse_action_optimized(AllActionsVariant variant, Vector<Byte> input, Int read_bytes) -> Bool: 
    let num_actions : Byte
    for field of variant:
        num_actions = num_actions + byte(1)

    let counter : Byte
    if !from_byte_vector(counter, input, read_bytes):
        return false

    counter = counter % num_actions

    for field of variant:
        if counter == byte(0):
            using Type = type(field)
            let to_parse : Type
            if !from_byte_vector(to_parse, input, read_bytes):
                return false
            field = to_parse
            return true
        counter = counter - byte(1)

    return false

fun<AllActionsVariant> parse_actions(AllActionsVariant variant, Vector<Byte> input) -> Vector<AllActionsVariant>:
    return parse_actions(variant, input, 0)

fun<FrameType, AllActionsVariant> parse_and_execute(FrameType state, AllActionsVariant variant, Vector<Byte> input):
    parse_and_execute(state, variant, input, 0)

# method that bust be present in binary to ensure that all methods 
# required by rlc-learn are available
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
    from_byte_vector(x, serialized)
    parse_action_optimized(x, serialized, 0)
    enumerate(x).size()
    equal(variant, variant)
    let v : Vector<Float>
    to_observation_tensor(state, 0, v)
    print(variant)
    print(state)
    v.resize(10)
    v.size()
    observation_tensor_size(state)
    let vector : Vector<AllActionsVariant>
    load_action_vector_file("dc"s, vector)
    apply(vector, state)
    emit_observation_tensor_warnings(state)
    print_enumeration_errors(variant)

fun<ActionType> load_action_vector_file(String file_name, Vector<ActionType> out) -> Bool:
    let content : String 
    if !load_file(file_name, content):
        return false
    let variant : ActionType
    out = parse_actions(variant, content)
    return true

# trait that must be implemented by a type to enumerate all
# possible values of that types (ex: enumerate(bool) return 
# {true, false})
# 
# this is used by machine learning techniques that need to
# enumerate all possible actions.
trait<T> Enumerable:
    fun enumerate(T obj, Vector<T> output)

fun enumerate(Bool b, Vector<Bool> output):
    output.append(true)
    output.append(false)

fun<Enum T> enumerate(T b, Vector<T> output):
    let i = 0
    while i != b.max() + 1:
        let b : T 
        b.from_int(i)
        output.append(b)
        i = i + 1

fun<T> _enumerate_impl(T obj, Int current_argument, Vector<T> out, Int num_args):
    let counter = 0
    for field of obj:
        if counter == current_argument:
            if field is Enumerable:
                using Type = type(field)
                let vec : Vector<Type>
                field.enumerate(vec)

                let next_current_argument = current_argument + 1
                let is_last_one = next_current_argument == num_args
                let counter_2 = 0
                while counter_2 != vec.size():
                    field = vec.get(counter_2)
                    if is_last_one:
                        out.append(obj)
                    else:
                        _enumerate_impl(obj, next_current_argument, out, num_args)
                    counter_2 = counter_2 + 1
            return
        counter = counter + 1

trait<T> CustomEnumerationError:
    fun enumeration_error(T x, String out, Vector<String> context) 

fun enumeration_error(Int x, String out, Vector<String> context):
    out.append("ERROR: ")
    write_tensor_warning_context(out, context)
    out.append(" is of type Int, which is not enumerable. Replace it instead with a BInt with appropriate bounds or specify yourself how to enumerate it.\n")

fun enumeration_error(Float x, String out, Vector<String> context):
    out.append("ERROR: ")
    write_tensor_warning_context(out, context)
    out.append(" is of type Float, which is not enumerable. Specify yourself how to enumerate it.\n")

fun<T> enumeration_error(OwningPtr<T> x, String out, Vector<String> context):
    out.append("ERROR: ")
    write_tensor_warning_context(out, context)
    out.append(" is of type OwningPtr, which is not enumerable.\n")

fun<T, Int size> enumeration_error(T[size] x, String out, Vector<String> context):
    let t : T
    context.append("[0]"s)
    _observation_tensor_warnings(t, out, context)
    context.pop()

fun<T> enumeration_error(Vector<T> x, String out, Vector<String> context):
    let t : T
    context.append("[0]"s)
    _observation_tensor_warnings(t, out, context)
    context.pop()

fun<T> get_enumeration_errors_impl(T obj, String out, Vector<String> context): 
    if obj is CustomEnumerationError:
        obj.enumeration_error(out, context)
        return 
    if obj is Enumerable:
        return 
    if obj is Alternative:
        let current = 0
        for name, field of obj:
            using Type = type(field)
            let t : Type
            context.append(s(name))
            get_enumeration_errors_impl(t, out, context)
            context.pop()
            current = current + 1
        return
    for name, field of obj:
        context.append(s(name))
        get_enumeration_errors_impl(field, out, context)
        context.pop()
    

fun<T> get_enumeration_errors(T obj) -> String:
    let to_return : String
    let context : Vector<String>
    get_enumeration_errors_impl(obj, to_return, context)

    return to_return
    
fun<T> print_enumeration_errors(T obj) -> Bool:
    let s = get_enumeration_errors(obj)
    if s.size() == 0:
        return true
    print(s)
    return false 


fun<T> enumerate(T obj) -> Vector<T>:
    let to_return : Vector<T>
    if obj is Enumerable:
        obj.enumerate(to_return)
    else if obj is Alternative:
        for field of obj:
            using Type = type(field)
            let field : Type 
            let alternatives = enumerate(field)
            let counter = 0
            while counter < alternatives.size():
                obj = alternatives.get(counter)
                to_return.append(obj) 
                counter = counter + 1
    else:
        let num_fields = 0
        for field of obj:
            num_fields = num_fields + 1
        if num_fields == 0:
            to_return.append(obj)
            return to_return
        _enumerate_impl(obj, 0, to_return, num_fields)

    return to_return

# trait that must be implemented to specify how
# a given type is to be converted into a tensor
# for machine learning consumptions. The encoding
# should be, when possible, one-hot encoding.
trait<T> Tensorable:
    fun write_in_observation_tensor(T obj, Int observer_id, Vector<Float> output, Int counter) 
    fun size_as_observation_tensor(T obj) -> Int

trait<T> Enum:
    fun max(T obj) -> Int
    fun is_enum(T obj) -> Bool 
    fun as_int(T obj) -> Int
    fun from_int(T obj, Int new_value)

fun write_in_observation_tensor(Int value, Int min, Int max, Vector<Float> output, Int index):
    let counter = min
    while counter < max:
        if counter == value:
            output.get(index) = 1.0
        else:
            output.get(index) = 0.0
        index = index + 1
        counter = counter + 1


fun write_in_observation_tensor(Int obj, Int observer_id, Vector<Float> output, Int index) :
    return

fun size_as_observation_tensor(Int obj) -> Int :
    return 0

fun write_in_observation_tensor(Float obj, Int observer_id, Vector<Float> output, Int index) {false}:
    return

fun size_as_observation_tensor(Float obj) -> Int {false}:
    return 0

fun write_in_observation_tensor(Bool obj, Int observer_id, Vector<Float> output, Int index):
    if obj:
        output.get(index) = 1.0
    else:
        output.get(index) = 0.0
    index = index + 1

fun size_as_observation_tensor(Bool obj) -> Int:
    return 1

fun write_in_observation_tensor(Byte obj, Int observer_id, Vector<Float> output, Int index):
    write_in_observation_tensor(int(obj), -128, 127, output, index)

fun size_as_observation_tensor(Byte obj) -> Int:
    return 256

fun<T, Int X> write_in_observation_tensor(T[X] obj, Int observer_id, Vector<Float> output, Int index):
    let counter = 0
    while counter < X:
        _to_observation_tensor(obj[counter], observer_id, output, index)
        counter = counter + 1

fun<T, Int X> size_as_observation_tensor(T[X] obj) -> Int:
    return _size_as_observation_tensor_impl(obj[0]) * X

fun<T> write_in_observation_tensor(Vector<T> obj, Int observer_id, Vector<Float> output, Int index):
    return

fun<T> size_as_observation_tensor(Vector<T> obj) -> Int:
    return 0

fun<T, Int max_size> write_in_observation_tensor(BoundedVector<T, max_size> obj, Int observer_id, Vector<Float> output, Int index):
    let counter = 0
    while counter < obj.size():
        _to_observation_tensor(obj.get(counter), observer_id, output, index)
        counter = counter + 1

fun<T, Int max_size> size_as_observation_tensor(BoundedVector<T, max_size> obj) -> Int:
    return _size_as_observation_tensor_impl(obj.get(0)) * max_size

fun<Int min, Int max> write_in_observation_tensor(BInt<min, max> obj, Int observer_id, Vector<Float> output, Int index):
    write_in_observation_tensor(obj.value, min, max, output, index)

fun<Int min, Int max> size_as_observation_tensor(BInt<min, max> obj) -> Int:
    return max - min 

fun<T> _size_as_observation_tensor_impl(T obj) -> Int:
    if obj is Tensorable:
        return obj.size_as_observation_tensor()
    else if obj is Enum:
        return obj.max() + 1
    else if obj is Alternative:
        let alternative_count = 0
        for field of obj:
            alternative_count = alternative_count + 1
        let max_size = 0
        for field of obj:
            using Type = type(field)
            let fake : Type
            max_size = max(_size_as_observation_tensor_impl(fake), max_size)
        return max_size + alternative_count
    else:
        let to_return = 0
        for field of obj:
            to_return = to_return + _size_as_observation_tensor_impl(field)
        return to_return

fun<T> _to_observation_tensor(T obj, Int observer_id, Vector<Float> output, Int index):
    if obj is Tensorable:
        obj.write_in_observation_tensor(observer_id, output, index)
        return
    else if obj is Enum:
        write_in_observation_tensor(obj.as_int(), 0, obj.max() + 1, output, index)
        return
    else if obj is Alternative:
        let alternative_count = 0
        for field of obj:
            alternative_count = alternative_count + 1
        let current = 0
        let largest_field = _size_as_observation_tensor_impl(obj) - alternative_count
        for field of obj:
            using Type = type(field)
            if obj is Type:
                let zeros_to_add = largest_field - _size_as_observation_tensor_impl(obj)
                write_in_observation_tensor(current, 0, alternative_count, output, index)
                _to_observation_tensor(field, observer_id, output, index)
                while zeros_to_add != 0:
                    zeros_to_add = zeros_to_add - 1
                    output.get(index) = 0.0
                    index = index + 1 
                return
            current = current + 1
    else:
        for field of obj:
            _to_observation_tensor(field, observer_id, output, index)

fun<T> to_observation_tensor(T obj, Int observer_id, Vector<Float> output):
    _to_observation_tensor(obj, observer_id, output, 0)

fun<T> to_observation_tensor(T obj, Int observer_id, Vector<Float> output, Int written_bytes):
    _to_observation_tensor(obj, observer_id, output, written_bytes)

fun<T> to_observation_tensor(T obj, Int observer_id) -> Vector<Float>:
    let output : Vector<Float> 
    let x = _size_as_observation_tensor_impl(obj)
    while x != 0:
        x = x - 1
        output.append(0.0)
    _to_observation_tensor(obj, observer_id, output, 0)
    return output

fun<T> observation_tensor_size(T obj) -> Int:
    return _size_as_observation_tensor_impl(obj)

trait<T> CustomTensorWarnings:
    fun tensorable_warning(T x, String out, Vector<String> context) 

fun write_tensor_warning_context(String out, Vector<String> context):
    let i = 0
    while i != context.size():
        out.append(context[i])
        if i+1 != context.size():
            out.append(".") 
        i = i + 1

fun tensorable_warning(Int x, String out, Vector<String> context):
    out.append("WARNING: ")
    write_tensor_warning_context(out, context)
    out.append(" is of type Int, which is not tensorable. Replace it instead with a BInt with appropriate bounds or specify yourself how to serialize it, or wrap it in a Hidden object. It will be ignored by machine learning.\n")

fun tensorable_warning(Float x, String out, Vector<String> context):
    out.append("WARNING: ")
    write_tensor_warning_context(out, context)
    out.append(" is of type Float, which is not tensorable. Specify yourself how to serialize it, or wrap it in a Hidden object. It will be ignored by machine learning\n")

fun<T> tensorable_warning(OwningPtr<T> x, String out, Vector<String> context):
    out.append("WARNING: ")
    write_tensor_warning_context(out, context)
    out.append(" is of type OwningPtr, which is not tensorable. Specify yourself how to serialize it, or wrap it in a Hidden object. It will be ignored by machine learning.\n")

fun<T, Int size> tensorable_warning(T[size] x, String out, Vector<String> context):
    let t : T
    context.append("[0]"s)
    _observation_tensor_warnings(t, out, context)
    context.pop()

fun<T> tensorable_warning(Vector<T> x, String out, Vector<String> context):
    let t : T
    context.append("[0]"s)
    _observation_tensor_warnings(t, out, context)
    context.pop()


fun<T> _observation_tensor_warnings(T obj, String out, Vector<String> context):
    if obj is CustomTensorWarnings:
        obj.tensorable_warning(out, context)
        return
    if obj is Tensorable:
        return 
    if obj is Enum:
        return
    if obj is Alternative:
        let current = 0
        for field of obj:
            using Type = type(field)
            let t : Type
            _observation_tensor_warnings(t, out, context)
            current = current + 1
        return
    for name, field of obj:
        if s(name) != "resume_index":
            context.append(s(name))
            _observation_tensor_warnings(field, out, context)
            context.pop()


fun<T> to_observation_tensor_warnings(T obj) -> String:
    let x : Vector<String>
    x.append("obj"s)
    let s : String
    _observation_tensor_warnings(obj, s, x)
    return s

fun<T> emit_observation_tensor_warnings(T obj):
    print(to_observation_tensor_warnings(obj))
