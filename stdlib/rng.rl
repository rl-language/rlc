import collections.vector

cls RNG:
    Int[4] s

    fun _rotl(Int x, Int k) -> Int:
        return (x<<k) | (x>>(64-k))

    fun randint(Int min, Int max) -> Int:
        assert(min < max, "min cannot be greater or equal than max")
        let int_max = 0x0fffffffffffffff
        let span = max - min

        let limit = int_max - (int_max% span)
        let r = self.next() & int_max
        while r >= limit:
            r = self.next() & int_max
        return min + (r % span)

    fun _splitmax(Int value) -> Int:
        value = value + 0x9e3779b97f4a7c15
        let z = value
        z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9
        z = (z ^ (z >> 27)) * 0x94d049bb133111eb
        return z ^ (z >> 31)

    fun set_seed(Int new_seed):
        let i = 0
        while i != 4:
            self.s[i] = self._splitmax(new_seed)
            i = i + 1
        ref s = self.s
        if (s[0] | s[1] | s[2] | s[3]) == 0:
            s[0] = 1
        
    fun next() -> Int: 
        ref s = self.s
        let result = self._rotl(s[1]*5, 7)*9
        let t = s[1] << 17
        s[2] = s[2] ^ s[0]
        s[3] = s[3] ^ s[1]  
        s[1] = s[1] ^ s[2]  
        s[0] = s[0] ^ s[3]
        s[2] = s[2] ^ t
        s[3] = self._rotl(s[3], 45)
        return result

fun make_rng(Int seed) -> RNG:
    let rng : RNG
    rng.set_seed(seed)
    return rng

act configure_rng() -> ConfigureRNG:
    frm rng : RNG
    frm seed : Int
    frm i : Int
    frm entry : Int
    while i != 64:
        act set_rng_bit(Bool bit)
        seed = seed << 1
        seed = seed | int(bit)
        i = i + 1
    rng.set_seed(seed)
        

fun size_as_observation_tensor(RNG obj) -> Int:
    return 0 

fun size_as_observation_tensor(ConfigureRNG obj) -> Int:
    return 0 

fun write_in_observation_tensor(RNG obj, Int observer_id, Vector<Float> output, Int counter) :
    return

fun write_in_observation_tensor(ConfigureRNG obj, Int observer_id, Vector<Float> output, Int counter) :
    return
