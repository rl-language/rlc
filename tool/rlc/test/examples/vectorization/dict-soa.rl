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

fun main():
    let dic: Dict_SoA<Int,Int>
    let counter = 0
    while counter < NUM_KEYS:
        dic.insert(counter, counter * counter)
        counter = counter + 1
    
    counter = 0
    while counter < NUM_KEYS/10:
        let key = counter * 10
        if dic.contains(key):
            print("Found entry with key "s + to_string(key) + 
                "value "s + to_string(dic.get(key)))
            dic.remove(key)

            if dic.contains(key):
                print("REMOVAL FAILED")
        counter = counter + 1
    
    let keys = dic.keys()
    let values = dic.values()

    let dicExc: Dict_SoA<Int, Int>
    
    counter = 0
    while counter < keys.size():
        dicExc.insert(values.get(counter), keys.get(counter))
        counter = counter + 1


    for key in keys:
        print("dic: key, value"s + to_string(key) + 
            ", "s + to_string(dic.get(key)))

    for key in values:
        print("dicExc: key, value"s + to_string(key) + 
            ", "s + to_string(dicExc.get(key)))

    dic.clear()
    dicExc.drop()



    

