# RUN: split-file %s %t

# RUN: rlc %t/source.rl -o %t/Lib%sharedext -i %stdlib --shared
# RUN: rlc %t/source.rl -o %t/csharp.cs -i %stdlib --c-sharp

# RUN: mcs -out:%t/executable.exe %t/csharp.cs %t/main.cs -unsafe
# RUN: mono %t/executable.exe
# REQUIRES: has_mono

#--- source.rl
import collections.vector 
cls Pair:
    Vector<Int> x 

fun asd():
    let x : Pair
    x.x.size()


#--- main.cs
using System;
using System.IO;
using System.Reflection;
class Tester {
    public static int Main() {
        RLCNative.setup(Path.GetDirectoryName(Assembly.GetEntryAssembly().Location) + "/Lib" + RLCNative.SharedLibExtension);
        VectorTint64_tT pair = new VectorTint64_tT();
        long x = 2;
        pair.append(x);
        pair.append(x);
        pair.append(x);
        return (int)(pair.size() - 3);
    }
}
