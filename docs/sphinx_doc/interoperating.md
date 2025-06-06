# Interoperation with engines and languages

This page describes how you can connect Rulebook to other languages. It is not particularly hard to implement a new wrapper generator for RLC, the [python wrapper generator](https://github.com/rl-language/rlc/blob/master/lib/conversions/src/RLCToPython.cpp) takes ~1000 lines of code. We could have implemented the wrappers for any language, we just picked those that seemed more relevant to us. If you do need support for a new language or some language feature, contact us.

## C Interop

We achieve interoperability with C almost trivially since Rulebook has the same ABI as C.

### Calling rulebook from C
* Each Rulebook class is mapped onto a C struct. Each alternative is mapped on a struct with a union and a integer inside. You have to manually invoke the init, assign and drop method whenever you allocate, copy or destroy a rlc dastructure (unless the method is not available at all, which means that the datastruture is trivially initializable, copiable or destructible).
* Every enum is mapped on a struct with a integer inside.
* Every Action Function frame is mapped on a struct with the frm variables and a integer inside.
* All arguments of functions are taken by pointer, the return value is passed as first argument. The return value does not need to be initialized by the caller, but it must be destroyed by the caller.
* Preconditions of functions are mapped onto a function with the same name, except prefixed with can.

Here is a simple example
```rlc
# file.rl
fun to_invoke() -> Int {true}:
  return 5
```
You can emit the compiled library and wrapper with the following RLC commands:
```bash
rlc file.rl -o lib.a --compile # uses linux naming conventions
rlc file.rl -o header.h --header
```

Then you can use the header from C by inlcuding it, with a couple of macro definitions to specify what you want in particular from the header.
```c
// file.c
#include <stdint.h>
#include <stdbool.h>
#define RLC_GET_FUNCTION_DECLS // required or the header does not emit function delcs
#define RLC_GET_TYPE_DEFS // required or the header does not emit the types.
#include "./header.h"

int main() {
  int64_t result;
  rl_to_invoke__r_int64_t(&result);
  return 5 - result;
}
```

Finally you can compile with any compiler, in this example clang
```bash
clang file.c lib.a -o executable
./executable
```

Here is a example that uses the most imporant features of rlc:
```rlc
# file.rl
import collections.vector
import serialization.print

cls Context:
    Int x
    Int y

act sequence(ctx Context context) -> Sequence:
    frm accumulator : Vector<Int>  # drop this and Sequence will become trivially constructible (that is, no mallocs triggered)
    while true:
        act add(Int z)
        accumulator.append(context.x + context.y + z)
        print(accumulator)

```

```c
// file.c

#include <stdbool.h>
#include <stdint.h>
#define RLC_GET_FUNCTION_DECLS // required or the header does not emit function
                               // delcs
#define RLC_GET_TYPE_DECLS
#define RLC_GET_TYPE_DEFS // required or the header does not emit the wrapper.
#include "./header.h"

int main() {
  Context context;
  context.content.x = 2;
  context.content.y = 3;

  Sequence sequence;
  rl_sequence__Context_r_Sequence(&sequence, &context);

  int64_t z = 10;
  bool canDoIt;
  rl_m_can_add__Sequence_Context_int64_t_r_bool(&canDoIt, &sequence, &context,
                                                &z);
  if (canDoIt)
    rl_m_add__Sequence_Context_int64_t(&sequence, &context, &z);
  rl_m_drop__Sequence(&sequence);

  return 0;
}
```
compile with
```bash
rlc /tmp/file.rl -o /tmp/header.h --header
clang /tmp/file.c -o /tmp/lib.a -c
rlc /tmp/file.rl /tmp/lib.a -o /tmp/exec
/tmp/exec # [15]
```

This example shows most of the functinalities you may need in C, there is only one missing thing, which is how do you call C from rulebook.

### Calling C from rulebook

In rulebook, you can declare a function with extern to specify that it is a external function to be found at link time. Here is how you do it:

Declare a function extern
```rlc
import serialization.print

ext fun call_external(Int x) -> Int

fun main() -> Int:
    print(call_external(10))
    return 0

```
generate the header so you get checks that you have not messed up the signature in C.
```
rlc /tmp/file.rl -o /tmp/header.h --header
```

write the c

```c
#include <stdbool.h>
#include <stdint.h>
#define RLC_GET_FUNCTION_DECLS // required or the header does not emit function
                               // delcs
#define RLC_GET_TYPE_DECLS
#define RLC_GET_TYPE_DEFS // required or the header does not emit the wrapper.
#include "./header.h"

void rl_call_external__int64_t_r_int64_t(int64_t *result, int64_t *arg) {
  *result = 10 * *arg;
}
```

compile and run
```
clang /tmp/file.c -o /tmp/lib.a -c
rlc /tmp/file.rl -o /tmp/exec /tmp/lib.a
/tmp/exec # 100
```

## Interop with CPP

The interop with cpp is identical to the one with C, except the generated header detects cpp is available so it will remove the need for manual managment of constructors and destructors.

Let us revisig the previous example in cpp.

```rlc
# file.rl
import collections.vector
import serialization.print

cls Context:
    Int x
    Int y

act sequence(ctx Context context) -> Sequence:
    frm accumulator : Vector<Int>  # drop this and Sequence will become trivially constructible (that is, no mallocs triggered)
    while true:
        act add(Int z)
        accumulator.append(context.x + context.y + z)
        print(accumulator)

```

```cpp
// file.cpp

#include <stdbool.h>
#include <stdint.h>
#define RLC_GET_FUNCTION_DECLS // required or the header does not emit function
                               // delcs
#define RLC_GET_TYPE_DECLS
#define RLC_GET_TYPE_DEFS // required or the header does not emit the wrapper.
#include "./header.h"

int main() {
  Context context;
  context.content.x = 2;
  context.content.y = 3;

  Sequence seq = sequence(context);

  int64_t z = 10;
  if (seq.can_add(context, z))
    seq.add(context, z);

  return 0;
}
```
compile with
```bash
rlc /tmp/file.rl -o /tmp/header.h --header
clang++ /tmp/file.cpp -o /tmp/lib.a -c -fno-exceptions
rlc /tmp/file.rl /tmp/lib.a -o /tmp/exec
/tmp/exec # [15]
```

This makes using interactive sequences from cpp very easy.

## Interop with Python.

Since Rulebook has the same ABI as C, using Rulebook from python is similar to using C, except we provide wrappers to simplify the experience.

### Using Rulebook from python.

RLC can emit a ctypes based wrapper for python. Continuing with the previous example, to use Rulebook from python you can do the following.

```rlc
# file.rl
import collections.vector
import serialization.print

cls Context:
    Int x
    Int y

act sequence(ctx Context context) -> Sequence:
    frm accumulator : Vector<Int>  # drop this and Sequence will become trivially constructible (that is, no mallocs triggered)
    while true:
        act add(Int z)
        accumulator.append(context.x + context.y + z)
        print(accumulator)

```

```python
# file.py
import wrapper
context = wrapper.Context()
context.x = 2
context.y = 3
state = wrapper.sequence(context)
if state.can_add(context, 10):
    state.add(context, 10)
```

You can execute the program with the following commands
```
rlc /tmp/file.rl --python -o /tmp/wrapper.py                                                                                   master ✭ ✱ ◼
rlc /tmp/file.rl --shared -o /tmp/lib.so                                                                                       master ✭ ✱ ◼
(.venv) ~/r/rlc ❯❯❯ python /tmp/file.py
```

### Invoking python from rulebook

To be able to invoke python from rulebook, rulebook must link against libpython. You can do so by passing `--pylib` to the compilation commands.

```rlc
import collections.vector
import serialization.print
import python


cls PythonAllocatedCallBack:
    PyObject object

    fun pass_int_to_python(Int x):
        self.object.call("receive_int", to_pyobject(x))

fun send_10_to_python(PythonAllocatedCallBack callback):
    callback.pass_int_to_python(10)
```

```python
# file.py
import wrapper
class PythonReceiver:
    def receive_int(self, x):
        print(x)

callback = wrapper.PythonAllocatedCallBack()
callback.object = PythonReceiver()
wrapper.send_10_to_python(callback)
```

```bash
rlc /tmp/file.rl --shared -o /tmp/lib.so --pylib
rlc /tmp/file.rl --python -o /tmp/wrapper.py
python /tmp/file.py
```

## Interop with Unity and CSharp

We have interoperability with CShpart and thus with Unity. Since unity heavily relies on reloading CSharp every time it changes to provide a scripting language like experience, we have added that ability to the wrapper exported for C# so that when Rulebook changes, the unity editor updates automatically.

Unity integration requires a plugin in the editor to trigger the updates at the right times.

You can checkout the Unity TicTacToe example where we have unity typesafelly accessing rulebook, and reloading rulebook at every change.
Relevant files are:
* [The editor script](https://github.com/rl-language/rlc-unity-example/blob/master/Assets/Editor/OnRLFileChanged.cs) that watches for changes in the rl files
* [The global script](https://github.com/rl-language/rlc-unity-example/blob/master/Assets/Scripts/Example.cs) that contains the game state.
* [The board slot](https://github.com/rl-language/rlc-unity-example/blob/master/Assets/Scripts/BoardSlot.cs) that knows how to trigger actions.


## Interop with godot

Our large example [4Hammer](./4hammer.md) shows interop with godot and cmake.


## Interop with unreal

Unfortunatelly there is no way for a unreal engine plugin to hook inside the the build system of unreal. We have no way to package Rulebook in a way that can be hot reloaded from Unreal without the unreal programmer providing some code themselves. For unreal you have to start from the CPP interop and build from there.

## CMake

We expose typical actions one my wish to perform from cmake as a cmake config file. This includes correct managment of imported files in rulebook for incremental builds, the various wrappers generators and so on. You can see examples in the [godot plugin cmake file](https://github.com/rl-language/4Hammer/blob/master/CMakeLists.txt) of 4hammer.

## Cross compiling

Cross compiling is possible but requires the user to compile the native standard library of the language for every target platform. This is trivial because the entirety of the native standard library is 200 lines long and relies on a couple of libc functions only. The source file of standard library is shipped with the headers of the compiler too, so you can just include it in another cpp library of your tool and you are done. RLC accepts the triple of the target platform.
