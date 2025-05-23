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

    fun init():
        self.x = 2
        self.y = 1

cls Outer:
    Pair inner


fun to_invoke(Outer self, Int to_add) -> Int {true}:
    return self.inner.x + self.inner.y + to_add

#--- main.cs
using System;
using System.IO;
using System.Reflection;
class Tester {
    public static int Main() {
        RLCNative.setup(Path.GetDirectoryName(Assembly.GetEntryAssembly().Location) + "/Lib" + RLCNative.SharedLibExtension);
        Outer pair = new Outer();
        Console.WriteLine(pair.inner.x);
        
        return ((int) RLC.to_invoke(pair, 2)) - 5;
    }
}

