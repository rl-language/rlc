# Building RLC from sources

This section is about **building rlc itself**, not about building things with Rulebook. If the difference is not clear to you, you are not interested in this page.

Building on windows is supported but at the moment undocumented.

## Downloading sources

Execute line by line the content of [setup.sh](https://github.com/rl-language/rlc/blob/master/setup.sh). If you are on a "normal enough" linux or mac machine you can just download the file and execute it, but the stranger is your machine configuration the more likelly is to break.

**setup.sh** will
* check that you have python3, ninja and cmake.
* it will clone rlc into rlc\_infrastructure.
* it will create a python virtual environment to not pollute your machine with python packages.
* it will install those packages.
* it will initialize python submodules (google-test, and google-benchmarks, a testing and a benchmarking library used by rlc at build time only.)

At this stage you will have a folder with the following structure
```
rlc-infrastructure/rlc # source directory
rlc-infrastructure/.venv # python cache for packages
```

## environment.sh

**IMPORTANT**  every time you open a new shell that you wish to use for RLC, you must run:
```
source rlc-infrastructure/rlc/environment.sh
```

Beside making sure that you have the correct python virtual environment enabled, this command will make sure that when we build LLVM it is available to RLC, even without installing LLVM.

## Building
A properly compiled RLC minimal installation depends on nothing except typical cpp libraries, but when building RLC itself we need a specific configuration of LLVM. For this reason we provide a build script that will configure the directories in a resonable configuration.

to do so, you can run

```
# linux
cd rlc-infrastructure
python rlc/build.py # --dry-run to see what it does without running it

# mac
cd rlc-infrastructure
python rlc/build.py --no-use-lld # --dry-run to see what it does without running it
```

What the script will do is:

* clone LLVM
* build LLVM in debug mode
* install LLVM debug inside rlc-infrastructure
* build LLVM in release mode
* install LLVM release inside rlc-infrastructure
* build rlc-debug
* test rlc-debug
* build rlc-release
* test rlc-release
* install rlc-release in rlc-infrastructure/rlc-release

at the end of this step you will have in your rlc-infrastructure-directory
```
rlc
.venv
llvm-debug
llvm-install-debug
llvm-release
llvm-install-release
rlc-debug
rlc-release
rlc-release/install # here is the installation you care about, probably
```

After this has compleated successfully, you can disregard build.py, use regular ninja commands

## Successive builds

After the first build, if you change the source code of rlc you don't need to run build.py, you can run

```
cd rlc-infrastructure/rlc-release # or rlc-debug
ninja all
ninja install # optional
```

this will build and install rlc.

## Running tests

You can can run all tests as follow
```
cd rlc-infrastructure/rlc-release # or rlc-debug
ctest --verbose # runs all tests
ctest -R name --verbose # filter tests by name
lit --verbose tool/rlc/test # run end to end compiler tests
lit --verbose tool/rlc/test --filter NAME # run end to end compiler tests
```

## Running the built rlc
If you have built rlc from sources there are two ways of doing it. You can first install it, and use it from the insall directory
```
cd rlc-infrastructure
ninja all
ninja install
./install/bin/rlc file.rl -o executable
```

of you can skip installing it every time and use the built one.

```
cd rlc-infrastructure/rlc-release # will not work in debug, there you have to write tests
ninja all
./tool/rlc/rlc file.rl -o exec -i ../rlc/stdlib --runtime-lib lib/runtime/libruntime.so # or libruntime.dylib on mac.
```

You must manually specify the location of the standard library and of the runtime lib because the directory structure of the build directory is different than the release structure, and so the built version has no idea where those things are.

## Packaging for PIP

You can make a pip package with
```
cd rlc-infrastructure/
ninja all
ninja install
ninja pip_package
```

which you can then install with
```
pip install rlc-infrastructure/rlc-release/dist/PACKAGE_NAME.whl
```
where the package name depends on the current version of rlc.

## Compiler speed
You can use
```
rlc --timing file.rl -o out
```

to see the time it takes to run each compiler pass.

## Running benchmarks

We have some benchmarks for the speed of the produced code. You can run them with

```
cd rlc-infrastructure/rlc-release
./lib/utils/benchmark/ConnectFourBenchmark
```

All benchmarks live in the `./lib/utils/benchmark` directory.
