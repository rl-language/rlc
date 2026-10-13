import collections.vector
import string
import bounded_arg

# Recursively compares two values of the same type and appends the dot-separated
# path of every leaf field that differs to `out`.
#
# Usage:
#   let changed : Vector<String>
#   diff(before, after, changed)
#   # changed now contains paths like "board.slots.0.0", "hand.0.value", etc.

# Trait for types that have custom diff behaviour (primitives, arrays, vectors).
# Types that implement this are treated as diff leaves or have custom traversal.
trait<T> Diffable:
    fun _apply_diff(T before, T after, String path, Vector<String> out)
    fun _emit_all(T value, String path, Vector<String> out)

fun _apply_diff(Int before, Int after, String path, Vector<String> out):
    if before != after:
        out.append(path)

fun _apply_diff(Bool before, Bool after, String path, Vector<String> out):
    if before != after:
        out.append(path)

fun _apply_diff(Byte before, Byte after, String path, Vector<String> out):
    if before != after:
        out.append(path)

fun _apply_diff(Float before, Float after, String path, Vector<String> out):
    if before != after:
        out.append(path)

fun<Int min, Int max> _apply_diff(BInt<min, max> before, BInt<min, max> after, String path, Vector<String> out):
    if before.value != after.value:
        out.append(path)

fun<T, Int X> _apply_diff(T[X] before, T[X] after, String path, Vector<String> out):
    let i = 0
    while i < X:
        let child_path = path.add("."s.add(to_string(i)))
        _diff_impl(before[i], after[i], child_path, out)
        i = i + 1

fun<T> _apply_diff(Vector<T> before, Vector<T> after, String path, Vector<String> out):
    if before.size() != after.size():
        out.append(path.add(".#"s))
        let i = 0
        while i < after.size():
            let child_path = path.add("."s.add(to_string(i)))
            _emit_all_impl(after.get(i), child_path, out)
            i = i + 1
        return
    let i = 0
    while i < before.size():
        let child_path = path.add("."s.add(to_string(i)))
        _diff_impl(before.get(i), after.get(i), child_path, out)
        i = i + 1

fun<T, Int dim> _apply_diff(BoundedVector<T, dim> before, BoundedVector<T, dim> after, String path, Vector<String> out):
    if before.size() != after.size():
        out.append(path.add(".#"s))
        let i = 0
        while i < after.size():
            let child_path = path.add("."s.add(to_string(i)))
            _emit_all_impl(after.get(i), child_path, out)
            i = i + 1
        return
    let i = 0
    while i < before.size():
        let child_path = path.add("."s.add(to_string(i)))
        _diff_impl(before.get(i), after.get(i), child_path, out)
        i = i + 1

fun<Enum T> _apply_diff(T before, T after, String path, Vector<String> out):
    if before.as_int() != after.as_int():
        out.append(path)

fun<T> _diff_impl(T before, T after, String path, Vector<String> out):
    if before is Diffable:
        _apply_diff(before, after, path, out)
    else if before is Alternative:
        let changed_variant = false
        for name, field, field2 of before, after:
            using FieldType = type(field)
            if field2 is FieldType:
                if before is FieldType:
                    if !changed_variant:
                        _diff_impl(field, field2, path, out)
                        changed_variant = true
            else:
                if !changed_variant:
                    out.append(path)
                    changed_variant = true
    else:
        for name, field, field2 of before, after:
            using FieldType = type(field)
            if field2 is FieldType:
                let child_path = path.add("."s.add(s(name)))
                _diff_impl(field, field2, child_path, out)

# Appends the path of every leaf reachable from `value`, unconditionally.
# Used to describe brand-new vector elements (which have no `before` to diff).
fun _emit_all(Int value, String path, Vector<String> out):
    out.append(path)

fun _emit_all(Bool value, String path, Vector<String> out):
    out.append(path)

fun _emit_all(Byte value, String path, Vector<String> out):
    out.append(path)

fun _emit_all(Float value, String path, Vector<String> out):
    out.append(path)

fun<Int min, Int max> _emit_all(BInt<min, max> value, String path, Vector<String> out):
    out.append(path)

fun<Enum T> _emit_all(T value, String path, Vector<String> out):
    out.append(path)

fun<T, Int X> _emit_all(T[X] value, String path, Vector<String> out):
    let i = 0
    while i < X:
        let child_path = path.add("."s.add(to_string(i)))
        _emit_all_impl(value[i], child_path, out)
        i = i + 1

fun<T> _emit_all(Vector<T> value, String path, Vector<String> out):
    out.append(path.add(".#"s))
    let i = 0
    while i < value.size():
        let child_path = path.add("."s.add(to_string(i)))
        _emit_all_impl(value.get(i), child_path, out)
        i = i + 1

fun<T, Int dim> _emit_all(BoundedVector<T, dim> value, String path, Vector<String> out):
    out.append(path.add(".#"s))
    let i = 0
    while i < value.size():
        let child_path = path.add("."s.add(to_string(i)))
        _emit_all_impl(value.get(i), child_path, out)
        i = i + 1

fun<T> _emit_all_impl(T value, String path, Vector<String> out):
    if value is Diffable:
        _emit_all(value, path, out)
    else if value is Alternative:
        out.append(path)
    else:
        for name, field of value:
            let child_path = path.add("."s.add(s(name)))
            _emit_all_impl(field, child_path, out)

# Compares `before` and `after` field by field and appends the path of every
# changed leaf to `out`. Paths use dot notation: "board.slots.0.0".
fun<T> diff(T before, T after, Vector<String> out):
    for name, field, field2 of before, after:
        using FieldType = type(field)
        if field2 is FieldType:
            _diff_impl(field, field2, s(name), out)
