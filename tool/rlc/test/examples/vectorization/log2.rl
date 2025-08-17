import serialization.print
import none
import math.numeric
const _stride = 17
const _bits = 64

fun pow2(Int index) -> Int:
    let exp = index
    # print("pow of 2 to the "s + to_string(exp))
    if exp == 0:
        return 1
    
    if exp == 1:
        return 2
    
    let pow = 2
    while exp > 1:
        pow = pow * 2
        exp = exp - 1
    
    # print("= "s + to_string(pow))
    return pow


fun main() -> Int:
    let counter:Int
    let index = 0
    let pow = 0
    # counter = 131070
    # while counter < 131073:
    #     let acc_found = counter & ((counter ^ (-counter)) + 1)
    #     print("counter = "s + to_string(counter) + 
    #         ", -counter = "s + to_string((counter ^ (-counter)) + 1) + 
    #         ", acc_found = "s + to_string(acc_found) + 
    #         ", abs(counter) = "s + to_string(abs(counter)) + 
    #         ", abs(-131071) = "s + to_string(abs(-131071)) + 
    #         ", -counter = "s + to_string(-counter))
    #     counter = counter + 1
    counter = 1
    while counter < 10000:
        # while counter < 100000:
        index = log2l(counter)
        pow = pow2(index)

        if pow != counter:
            print("WRONG case: index = "s + to_string(index) + 
                ", pow = "s + to_string(pow) + 
                ", counter = "s + to_string(counter))
        # else: 
        #     print("index = "s + to_string(index) + 
        #         ", counter = "s + to_string(counter))

        counter = counter * 2
    log2(131071)
    log2l(131071)
    log2(131070)
    log2l(131070)
    return 0

# fun log2dumb(Int counter)

fun log2l(Int counter) -> Int:
    let next_shift: Int
    next_shift = _bits
    let shifts = 0
    let shifted = counter
    while next_shift != 1:
        next_shift = next_shift >> 1
        let tmp = shifted << next_shift
        # print("shifted = "s + to_string(shifted) +
        #     " ,tmp = "s + to_string(tmp) +
        #     " ,next_shift = "s + to_string(next_shift) +
        #     ", shifts = "s + to_string(shifts))

        if tmp != 0:
            shifts = shifts + next_shift
            shifted = tmp
        
    let index = 64 - shifts - 1
    print("counter = "s + to_string(counter) + 
        ", index = "s + to_string(index))
    return index



fun log2(Int counter) -> Int:
    # get only the first 1
    # eg: 1111100 -> 0000100
    let acc_found = counter & (-counter)
    # Binary search like base 2 log

    # let upper = _getConst()
    # let upper = _stride
    print("acc_found = "s + to_string(acc_found))
    let upper: Int
    upper = _stride
    let tmp = 0
    let rel_index = 0 # relative index between 0 and _stride
    let index = 0
    while tmp != 1:
        rel_index = upper >> 1
        tmp = acc_found >> rel_index
        print("rel_index = "s + to_string(rel_index) + 
                ", tmp = "s + to_string(tmp) + 
                ", acc_found = "s + to_string(acc_found) + 
                ", upper = "s + to_string(upper) +
                ", index = "s + to_string(index))
        if tmp > 1:
            acc_found = tmp
            index = index + rel_index
        # else if tmp == 1:
        upper = rel_index
    
    print("index + rel_index = "s + to_string(index + rel_index))
    return (index + rel_index)





fun _getConst() -> Int:
    return _stride
