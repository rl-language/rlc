# RUN: split-file %s %t

# RUN: rlc %t/source.rl -o %t/Lib%sharedext -i %stdlib --shared
# RUN: rlc %t/source.rl -o %t/csharp.cs -i %stdlib --c-sharp

# RUN: mcs -out:%t/executable.exe %t/csharp.cs %t/main.cs -unsafe
# RUN: mono %t/executable.exe
# REQUIRES: has_mono

#--- source.rl
fun to_invoke() -> Int {true}:
  return 5

#--- main.cs
class Tester {
    public static int Main() {
        long result = RLC.to_invoke();
        return ((int)(result)) - 5;
    }
}
