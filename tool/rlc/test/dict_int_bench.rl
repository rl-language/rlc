# RUN: rlc %s -o %t -i %stdlib --shared
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
    
    fun to_key(Int x) -> Int:
        return x

cls LargeKey:
    Int content
    Int dont_care_0
    Int dont_care_1
    Int dont_care_2
    Int dont_care_3
    Int dont_care_4
    Int dont_care_5
    Int dont_care_6
    Int dont_care_7
    Int dont_care_8
    Int dont_care_9
    
    # Initialize all fields to avoid missing initializations
    fun init():
        self.content = 0
        self.dont_care_0 = 0
        self.dont_care_1 = 0
        self.dont_care_2 = 0
        self.dont_care_3 = 0
        self.dont_care_4 = 0
        self.dont_care_5 = 0
        self.dont_care_6 = 0
        self.dont_care_7 = 0
        self.dont_care_8 = 0
        self.dont_care_9 = 0
    
    fun init(Int key):
        self.content = key
        # Initialize other fields with dummy values
        self.dont_care_0 = 0
        self.dont_care_1 = 0
        self.dont_care_2 = 0
        self.dont_care_3 = 0
        self.dont_care_4 = 0
        self.dont_care_5 = 0
        self.dont_care_6 = 0
        self.dont_care_7 = 0
        self.dont_care_8 = 0
        self.dont_care_9 = 0
    
    # Explicitly implement compute_hash for better cross-platform compatibility
    fun compute_hash() -> Int:
        # Only use content for hash - that's what matters for our benchmark
        return compute_hash_of(self.content)
    
    # Explicitly implement compute_equal for better cross-platform compatibility
    fun compute_equal(LargeKey other) -> Bool:
        # Only compare content - that's what matters for our benchmark
        return self.content == other.content

cls DictLargeKeyInt:
    Dict<LargeKey, Int> dict
        
    fun init():
        self.dict.init()

    fun insert(LargeKey key, Int value) -> Bool:
        return self.dict.insert(key, value)
        
    fun get(LargeKey key) -> Int:
        return self.dict.get(key)
        
    fun contains(LargeKey key) -> Bool:
        return self.dict.contains(key)
        
    fun remove(LargeKey key) -> Bool:
        return self.dict.remove(key) 

    fun to_key(Int x) -> LargeKey:
        let key : LargeKey
        key.content = x
        return key
