from shutil import which
from os.path import exists, isdir
from os import mkdir, path, chdir, scandir
from subprocess import run
import argparse

dry_run = False


def is_empty(dir_name: str) -> bool:
    """
    Returns True if the directory exists and contains item(s) else False
    """
    try:
        if any(scandir(dir_name)):
            return False
    except (NotADirectoryError, FileNotFoundError):
        pass
    return True


def program_in_path(program_name: str) -> bool:
    return which(program_name) is not None


def assert_in_path(program_name: str) -> str:
    assert program_in_path(program_name), "{} not in path".format(program_name)
    return which(program_name)


def try_make_dir(dir_path: str) -> bool:
    actual_path = path.abspath(dir_path)
    if dry_run:
        return actual_path
    if exists(dir_path):
        return actual_path
    mkdir(dir_path)
    return actual_path


def build_rlc(
    execution_dir: str,
    cmake_path,
    rlc_source_dir,
    install_dir,
    build_shared: bool,
    build_type: str,
    llvm_install_dir,
    clang_path: str,
    python_path: str
):
    assert_run_program(
        execution_dir,
        cmake_path,
        "{}".format(rlc_source_dir),
        "-DCMAKE_BUILD_TYPE={}".format(build_type),
        "-DCMAKE_INSTALL_PREFIX={}".format(install_dir),
        "-DCMAKE_EXPORT_COMPILE_COMMANDS=True",
        "-G",
        "Ninja",
        "-DMLIR_DIR={}/lib/cmake/mlir".format(llvm_install_dir),
        "-DLLVM_DIR={}/lib/cmake/llvm".format(llvm_install_dir),
        f"-DCMAKE_C_COMPILER={path.abspath(clang_path)}",
        f"-DCMAKE_CXX_COMPILER={path.abspath(clang_path)}++",
        "-DBUILD_SHARED_LIBS={}".format("ON" if build_shared else "OFF"),
        "-DCMAKE_BUILD_WITH_INSTALL_RPATH={}".format("OFF" if build_shared else "ON"),
        "-DHAVE_STD_REGEX=ON",
        "-DRUN_HAVE_STD_REGEX=1",
        "-DPython_EXECUTABLE:FILEPATH={}".format(python_path),
        "-DCMAKE_EXE_LINKER_FLAGS=-static-libgcc -static-libstdc++" if build_type == "Release" else "",
    )


def build_llvm(
    execution_dir: str,
    cmake_path,
    llvm_source_dir,
    install_dir,
    build_shared: bool,
    build_type: str,
    clang: str,
    clang_plus_plus: str,
    use_lld: bool,
):
    assert_run_program(
        execution_dir,
        cmake_path,
        "{}/llvm".format(llvm_source_dir),
        "-DLLVM_INSTALL_UTILS=True",
        "-DCMAKE_BUILD_TYPE={}".format(build_type),
        "-DCMAKE_INSTALL_PREFIX={}".format(install_dir),
        "-DLLVM_ENABLE_PROJECTS=clang;clang-tools-extra;mlir;compiler-rt;",
        "-DLLVM_USE_LINKER=lld" if use_lld else "",
        "-DCMAKE_EXPORT_COMPILE_COMMANDS=True",
        "-DLLVM_ENABLE_RUNTIMES=libcxx;libcxxabi;libunwind",
        f"-DCMAKE_C_COMPILER={clang}",
        f"-DCMAKE_CXX_COMPILER={clang_plus_plus}",
        "-G",
        "Ninja",
        "-DBUILD_SHARED_LIBS={}".format("ON" if build_shared else "OFF"),
    )


def install(execution_dir: str, ninja_path, run_tests=False):
    assert_run_program(execution_dir, ninja_path, "all")
    assert_run_program(execution_dir, ninja_path, "install")
    if run_tests:
        assert_run_program(execution_dir, ninja_path, "test")


def assert_run_program(execution_dir: str, command: str, *args: str):
    print("cd {}".format(execution_dir))
    print("{} {}".format(command, " ".join(args)))

    if dry_run:
        return
    filtered_args = [arg for arg in args if arg != ""]

    chdir(execution_dir)
    result = run([command] + filtered_args)
    assert result.returncode == 0


def main():
    parser = argparse.ArgumentParser(
        "run", description="runs a action of the simulation"
    )
    parser.add_argument(
        "--no-debug-llvm",
        help="does not build debug llvm",
        action="store_true",
    )
    parser.add_argument(
        "--dry-run",
        help="only prints the command that would be executing",
        action="store_true",
    )
    parser.add_argument(
        "--no-use-lld",
        help="do not set up rlc",
        action="store_true",
    )
    parser.add_argument(
        "--llvm-only",
        help="do not set up rlc",
        action="store_true",
    )
    parser.add_argument(
        "--llvm-dir",
        help="use the provided LLVM installation and skip other LLVM steps",
        type=str,
        default="",
    )
    parser.add_argument(
        "--cxx-compiler", help="path to cxx compiler", type=str, default="clang++"
    )
    parser.add_argument(
        "--c-compiler", help="path to c compiler", type=str, default="clang"
    )
    parser.add_argument(
        "--rlc-shared",
        help="if LLVM is provided by command line, this option allow to decide if rlc should be built as a shared or static libray. This is needed if LLVM is itself shared or static.  Defaults to false. ",
        action="store_true",
    )
    args = parser.parse_args()
    global dry_run
    dry_run = args.dry_run
    debug_llvm = not args.no_debug_llvm
    if args.llvm_dir != "":
        args.llvm_dir = path.abspath(args.llvm_dir)
        assert exists(args.llvm_dir), "provided a non existing llvm dir"

    # check needed programs are in path
    cmake = assert_in_path("cmake")
    if args.no_use_lld == False:
        ldd = assert_in_path("lld")
    ninja = assert_in_path("ninja")
    git = assert_in_path("git")
    python = assert_in_path("python")

    # create root dir
    rlc_infrastructure = path.abspath("./")

    # create dirs
    rlc_dir = path.abspath("rlc")
    assert isdir(
        rlc_dir
    ), "could not find rlc folder, did you executed this script from the wrong folder?"

    llvm_source_dir = path.abspath("llvm-project")
    rlc_build_dir = try_make_dir("rlc-debug")
    rlc_release_dir = try_make_dir("rlc-release")

    # build debug llvm
    llvm_install_debug_dir = path.abspath("llvm-install-debug")
    llvm_install_release_dir = path.abspath("llvm-install-release")

    # clone llvm
    if not exists(llvm_source_dir) and args.llvm_dir == "":
        assert_run_program(
            rlc_infrastructure,
            git,
            "clone",
            "https://github.com/llvm/llvm-project.git",
            "--depth",
            "1",
            "-b",
            "release/18.x",
        )

    if debug_llvm and not exists(llvm_install_debug_dir) and args.llvm_dir == "":
        llvm_build_debug_dir = try_make_dir(rlc_infrastructure + "/llvm-debug")
        build_llvm(
            execution_dir=llvm_build_debug_dir,
            cmake_path=cmake,
            llvm_source_dir=llvm_source_dir,
            install_dir=llvm_install_debug_dir,
            build_shared=True,
            build_type="Debug",
            clang=args.c_compiler,
            clang_plus_plus=args.cxx_compiler,
            use_lld=not args.no_use_lld,
        )
        install(execution_dir=llvm_build_debug_dir, ninja_path=ninja)

    # build release llvm
    if not exists(llvm_install_release_dir) and args.llvm_dir == "":
        llvm_build_release_dir = try_make_dir(rlc_infrastructure + "/llvm-release")
        build_llvm(
            execution_dir=llvm_build_release_dir,
            cmake_path=cmake,
            llvm_source_dir=llvm_source_dir,
            install_dir=llvm_install_release_dir,
            build_shared=False,
            build_type="Release",
            clang=args.c_compiler,
            clang_plus_plus=args.cxx_compiler,
            use_lld=not args.no_use_lld,
        )
        install(execution_dir=llvm_build_release_dir, ninja_path=ninja)

    if args.llvm_only:
        return

    llvm_dir = args.llvm_dir
    build_shared = debug_llvm
    if llvm_dir == "":
        llvm_dir = llvm_install_debug_dir if debug_llvm else llvm_install_release_dir
    else:
        build_shared = args.rlc_shared

    # build debug
    build_rlc(
        execution_dir=rlc_build_dir,
        cmake_path=cmake,
        rlc_source_dir=rlc_dir,
        install_dir="./install",
        build_shared=build_shared,
        build_type="Debug",
        llvm_install_dir=llvm_dir,
        clang_path=f"{llvm_install_release_dir}/bin/clang",
        python_path=python
    )
    install(execution_dir=rlc_build_dir, ninja_path=ninja, run_tests=True)

    build_rlc(
        execution_dir=rlc_release_dir,
        cmake_path=cmake,
        rlc_source_dir=rlc_dir,
        install_dir="./install",
        build_shared=False,
        build_type="Release",
        llvm_install_dir=llvm_install_release_dir,
        clang_path=f"{llvm_install_release_dir}/bin/clang",
        python_path=python

    )
    install(execution_dir=rlc_release_dir, ninja_path=ninja, run_tests=True)

    assert_run_program(
        rlc_dir,
        python,
        "python/solve.py",
        "./tool/rlc/test/tic_tac_toe.rl",
        "--rlc",
        "{}/install/bin/rlc".format(rlc_build_dir),
    )


if __name__ == "__main__":
    main()
