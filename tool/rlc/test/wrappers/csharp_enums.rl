# RUN: split-file %s %t

# RUN: rlc %t/source.rl -o %t/Lib%sharedext -i %stdlib --shared
# RUN: rlc %t/source.rl -o %t/csharp.cs -i %stdlib --c-sharp

# RUN: mcs -out:%t/executable.exe %t/csharp.cs %t/main.cs -unsafe
# RUN: mono %t/executable.exe
# REQUIRES: has_mono

#--- source.rl
enum Signal:
    rock
    paper
    scizzor 

#--- main.cs
using System;
class Tester {
    public static int Main() {
        Signal pair = Signal.paper();
        return (int)(pair.value - 1); 
    }
}

