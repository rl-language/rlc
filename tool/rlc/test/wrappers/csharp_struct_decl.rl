# RUN: split-file %s %t

# RUN: rlc %t/source.rl -o %t/Lib%sharedext -i %stdlib --shared
# RUN: rlc %t/source.rl -o %t/csharp.cs -i %stdlib --c-sharp

# RUN: mcs -out:%t/executable.exe %t/csharp.cs %t/main.cs -unsafe
# RUN: mono %t/executable.exe
# REQUIRES: has_mono

#--- source.rl
cls Pair:
    Int x 
    Int y

    fun to_invoke() -> Int {true}:
        return self.x + self.y

cls Outer:
    Pair inner

    fun to_invoke() -> Int {true}:
        return self.inner.x + self.inner.y

#--- main.cs
using System;
class Tester {
    public static int Main() {
        Outer pair = new Outer();
        pair.inner.x = 2;
        pair.inner.y = 1;
        
        return ((int) pair.inner.to_invoke()) - 3;
    }
}

