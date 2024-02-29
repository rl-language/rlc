# RUN: rlc %s -o %t -i %stdlib --sanitize
# RUN: %t

trait<T> CustomTrait:
    fun to_call(T x) -> Bool

fun<T> template(T to_visit):
    if to_visit is Alternative:
        for name, field of to_visit:
            if field is CustomTrait:
                let bool = field.to_call()

fun main() -> Int:
    let x : Int | Bool
    template(x)

    return 0
