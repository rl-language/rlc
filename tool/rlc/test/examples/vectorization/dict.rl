# RUN: rlc %s -o %t -i %stdlib
# RUN: %t%exeext

import collections.dictionary
import none
import range
import serialization.print

const NUM_KEYS = 50

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
    let dic: Dict<Int,Int>
    
    for key in range(NUM_KEYS):
        dic.insert(key, key * key)
    
    for k in range(5):
        let key = k * 10
        if dic.contains(key):
            print("Found entry with key "s + to_string(key) + 
                "value "s + to_string(dic.get(key)))
            dic.remove(key)

            if dic.contains(key):
                print("REMOVAL FAILED")
    
    let keys = dic.keys()
    let values = dic.values()

    let dicExc: Dict<Int, Int>
    
    for i in range(keys.size()):
        dicExc.insert(values.get(i), keys.get(i))

    for key in keys:
        print("dic: key, value"s + to_string(key) + 
            ", "s + to_string(dic.get(key)))

    for key in values:
        print("dicExc: key, value"s + to_string(key) + 
            ", "s + to_string(dicExc.get(key)))

    dic.clear()
    dicExc.drop()

    return 0
    

