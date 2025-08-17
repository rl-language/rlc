# RUN: rlc %s -o %t -i %stdlib
# RUN: %t%exeext

import collections.SoA_dictionary
import none
import range
import serialization.print

const NUM_KEYS = 5000

# DICTIONARY FUNCTIONS
# fun insert(KeyType key, ValueType value) -> Bool:
# fun get(KeyType key) -> ValueType:
# fun contains(KeyType key) -> Bool:
# fun remove(KeyType key) -> Bool:
# fun keys() -> Vector<KeyType>:
# fun values() -> Vector<ValueType>:
# fun empty() -> Bool:
# fun size() -> Int:
# fun clear():
# fun drop():


# Loop containing functions
# init()
# get()
# contains()
# remove()
# keys()
# values()
# clear()
# drop()

fun main() -> Int:
    let dic: Dict_SoA<Int,Int>
    let counter = 0
    # print("start insert loop")
    while counter < NUM_KEYS:
        dic.insert(counter, counter * counter)
        counter = counter + 1
    # print("ended insert loop")
    counter = 0
    dic.print_dict()
    # print("start contains-remove loop")
    while counter < NUM_KEYS/10:
        # print("contains-remove loop starting iteration: "s + to_string(counter))
        let key = counter * 10
        if dic.contains(key):
            # print("Found entry with key "s + to_string(key))
            let value = dic.get(key)
            # print("Its value is "s + to_string(value))
            dic.remove(key)

            if dic.contains(key):
                print("REMOVAL FAILED")
        # print("contains-remove loop ended iteration: "s + to_string(counter))
        counter = counter + 1
    # print("ended contains-remove loop")
    dic.print_dict()
    let keys = dic.keys()
    let values = dic.values()

    print("KEYS")
    counter = 0
    while counter < keys.size():
        print(to_string(keys.get(counter)))
        counter = counter + 1
    # for key in keys:
    # print(to_string(key))
    print("VALUES")
    for val in values:
        print(to_string(val))

    let dicExc: Dict_SoA<Int, Int>
    
    # print("start dicExc insert loop")
    counter = 0
    while counter < keys.size():
        dicExc.insert(values.get(counter), keys.get(counter))
        counter = counter + 1

    
    # for key in keys:
    #     print("dic: key, value"s + to_string(key) + 
    #         ", "s + to_string(dic.get(key)))

    # for key in values:
    #     print("dicExc: key, value"s + to_string(key) + 
    #         ", "s + to_string(dicExc.get(key)))
    dicExc.print_dict()
    # print("ended dicExc insert loop")
    dic.clear()
    dicExc.drop()
    return 0

    

