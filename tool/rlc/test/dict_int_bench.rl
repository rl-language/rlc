import collections.dictionary

cls DictIntInt:
    Dict<Int, Int> dict
    
    fun insert(Int key, Int value) -> Bool:
        return self.dict.insert(key, value)
        
    fun get(Int key) -> Int:
        return self.dict.get(key)
        
    fun contains(Int key) -> Bool:
        return self.dict.contains(key)
        
    fun remove(Int key) -> Bool:
        return self.dict.remove(key) 