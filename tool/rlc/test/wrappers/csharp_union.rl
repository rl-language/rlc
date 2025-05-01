# RUN: split-file %s %t

# RUN: rlc %t/source.rl -o %t/Lib%sharedext -i %stdlib --shared
# RUN: rlc %t/source.rl -o %t/csharp.cs -i %stdlib --c-sharp

# RUN: mcs -out:%t/executable.exe %t/csharp.cs %t/main.cs -unsafe
# RUN: mono %t/executable.exe
# REQUIRES: has_mono

#--- source.rl
cls Pair:
    Int | Float x 


cls Outer:
    Pair inner

#--- main.cs
using System;
using System.IO;
using System.Reflection;
class Tester {
    public static int Main() {
        RLCNative.setup(Path.GetDirectoryName(Assembly.GetEntryAssembly().Location) + "/Lib" + RLCNative.SharedLibExtension);
        Outer pair = new Outer();
        pair.inner.x.assign(2.2);
        if (pair.inner.x.get_long != null) {
            return -10;
        }
        return (int)(pair.inner.x.get_double - 2.2);
    }
}
