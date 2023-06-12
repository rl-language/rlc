from shutil import which
from os.path import exists, isdir
from os import mkdir, path, chdir
from subprocess import Popen


def program_in_path(program_name: str) -> bool:
    return which(program_name) is not None


def assert_in_path(program_name: str) -> str:
    assert program_in_path(program_name), "{} not in path".format(
        program_name
    )
    return which(program_name)


def try_make_dir(dir_path: str) -> bool:
    if exists(dir_path):
        return False
    mkdir(dir_path)
    return True


def assert_make_dir(dir_path: str):
    path = path.abspath(dir_path)
    if not try_make_dir(path):
        assert False, "trying to create {} dir, but it already exists".format(dir_path)
    return path


def build_rlc(
    execution_dir: str,
    cmake_path,
    rlc_source_dir,
    install_dir,
    build_shared: bool,
    build_type: str,
    llvm_install_dir,
):
    assert_run_program(
        execution_dir,
        cmake_path,
        "-DCMAKE_BUILD_TYPE",
        build_type,
        "-DCMAKE_INSTALL_PREFIX",
        install_dir,
        "../rlc -DCMAKE_EXPORT_COMPILE_COMMANDS",
        "True",
        "-G",
        "Ninja",
        "-DMLIR_DIR",
        llvm_install_dir / "lib/cmake/mlir",
        "-DLLVM_DIR",
        llvm_install_dir / "lib/cmake/llvm",
        "-DCMAKE_C_COMPILER",
        "clang",
        "-DCMAKE_CXX_COMPILER",
        "clang++",
        "-DBUILD_SHARED_LIBS",
        "ON" if build_shared else "OFF",
    )


def build_llvm(
    execution_dir: str,
    cmake_path,
    llvm_source_dir,
    install_dir,
    build_shared: bool,
    build_type: str,
):
    assert_run_program(
        execution_dir,
        cmake_path,
        "-DLLVM_INSTALL_UTILS",
        "True",
        "-DCMAKE_BUILD_TYPE",
        build_type,
        "-DCMAKE_INSTALL_PREFIX",
        install_dir,
        "-DLLVM_ENABLE_PROJECTS",
        '"clang;clang-tools-extra;mlir;"',
        "-DLLVM_USE_LINKER",
        "lld",
        llvm_source_dir,
        "-DCMAKE_EXPORT_COMPILE_COMMANDS",
        "True",
        "-G",
        "Ninja",
        "-DBUILD_SHARED_LIBS",
        "ON" if build_shared else "OFF",
    )


def install(execution_dir: str, ninja_path, run_tests=False):
    assert_run_program(execution_dir, ninja_path, "all")
    assert_run_program(execution_dir, ninja_path, "install")
    assert_run_program(execution_dir, ninja_path, "test")


def assert_run_program(execution_dir: str, command: str, *args: str):
    chdir(execution_dir)
    result = Popen([command] + args)


if __name__ == "__main__":
    # check needed programs are in path
    cmake = assert_in_path("cmake")
    ldd = assert_in_path("lld")
    ninja = assert_in_path("ninja")
    git = assert_in_path("git")
    python = assert_in_path("python")

    # create root dir
    rlc_infrastructure = path.abspath("./")

    # create dirs
    llvm_build_debug_dir = assert_make_dir("llvm-debug")
    llvm_install_debug_dir = assert_make_dir("llvm-install-debug")
    llvm_build_release_dir = assert_make_dir("llvm-release")
    llvm_install_release_dir = assert_make_dir("llvm-install-release")
    llvm_source_dir = assert_make_dir("llvm-project")
    rlc_dir = path.abspath("rlc")
    rlc_build_dir = path.abspath("rlc-debug")

    # clone llvm
    assert_run_program(
        execution_dir=rlc_infrastructure,
        command=git,
        args=[
            "clone",
            "git@github.com:llvm/llvm-project.git",
            "--depth",
            "1",
            "-b",
            "release/16.x",
        ],
    )

    # build debug llvm
    build_llvm(
        execution_dir=llvm_build_debug_dir,
        cmake_path=cmake,
        llvm_source_dir=llvm_source_dir,
        install_dir=llvm_install_debug,
        build_shared=True,
        build_type="Debug",
    )
    install(execution_dir=llvm_build_debug_dir, ninja_path=ninja)

    # build release llvm
    build_llvm(
        execution_dir=llvm_build_release_dir,
        cmake_path=cmake,
        llvm_source_dir=llvm_source_dir,
        install_dir=llvm_install_release,
        build_shared=False,
        build_type="Release",
    )
    install(execution_dir=llvm_build_release_dir, ninja_path=ninja)

    # build debug
    build_rlc(
        execution_dir=rlc_build_dir,
        cmake_path=cmake,
        rlc_source_dir=rlc_dir,
        install_dir="./install",
        build_shared=True,
        build_type="Debug",
        llvm_install_dir=llvm_install_debug_dir,
    )
    install(execution_dir=rlc_build_dir, ninja_path=ninja, run_tests=True)
    assert_run_program(
        execution_dir=rlc_dir,
        command=python,
        args=[
            "python/solve.py",
            "--source",
            "./tool//rlc/test/tris.rl",
            "--rlc",
            rlc_build_dir / "install/bin/rlc",
        ],
    )
