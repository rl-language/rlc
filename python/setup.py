from setuptools import setup, find_packages
import os
import shutil
import subprocess
from distutils.sysconfig import get_python_lib


def read_requirements(filename):
    with open(filename) as f:
        return f.read().splitlines()


def package_files(directories, prefix):
    paths = []
    for directory in directories:
        for path, directories, filenames in os.walk(directory):
            for filename in filenames:
                paths.append((path, [os.path.join(path, filename)]))
    return paths


def get_commit_hash():
    try:
        commit_hash = (
            subprocess.check_output(["git", "rev-parse", "HEAD"])
            .strip()
            .decode("utf-8")
        )
    except subprocess.CalledProcessError:
        commit_hash = "unknown"
    return commit_hash


def copy_binaries(source_directory, destination_directory):
    if not os.path.exists(destination_directory):
        os.makedirs(destination_directory)

    if os.path.isfile(source_directory):
        shutil.copy(source_directory, destination_directory)
        return

    for filename in os.listdir(source_directory):
        src_file = os.path.join(source_directory, filename)
        dest_file = os.path.join(destination_directory, filename)
        if os.path.isfile(src_file):
            shutil.copy(src_file, dest_file)
        else:
            copy_binaries(src_file, dest_file)


# Assuming binaries are in /path/to/cmake/install/bin and /path/to/cmake/install/lib
target_bin_dir = "bin" if os.name != "nt" else "Scripts"
exec_ext = "" if os.name != "nt" else ".exe"
lib_ext = ".a" if os.name != "nt" else ".lib"
lib_prefix = "lib" if os.name != "nt" else ""
copy_binaries(
    os.path.join("..", "..", "rlc-release", "install", "bin", "rlc" + exec_ext),
    target_bin_dir,
)
copy_binaries(
    os.path.join("..", "..", "rlc-release", "install", "bin", "rlc-doc" + exec_ext),
    target_bin_dir,
)
copy_binaries(
    os.path.join("..", "..", "rlc-release", "install", "bin", "rlc-lsp" + exec_ext),
    target_bin_dir,
)
copy_binaries(
    os.path.join(
        "..", "..", "rlc-release", "install", "lib", lib_prefix + "pyrlc" + lib_ext
    ),
    "lib",
)
copy_binaries(
    os.path.join(
        "..", "..", "rlc-release", "install", "lib", lib_prefix + "runtime" + lib_ext
    ),
    "lib",
)
copy_binaries(
    os.path.join(
        "..", "..", "rlc-release", "install", "lib", lib_prefix + "fuzzer" + lib_ext
    ),
    "lib",
)
copy_binaries(
    os.path.join("..", "..", "rlc-release", "install", "lib", "rlc"),
    os.path.join("lib", "rlc"),
)
copy_binaries(
    os.path.join("..", "..", "rlc-release", "install", "share"),
    os.path.join("share"),
)
extra_files_bin = package_files([target_bin_dir], target_bin_dir)
extra_files_lib = package_files(["lib"], "lib")
extra_files_share = package_files(["share"], "share")

site_packages_path = (
    target_bin_dir if os.name != "nt" else os.path.join("Lib", "site-packages")
)

version="0.3.4"
setup(
    name="rl_language_core",
    version=version,
    author="Massimo Fioravanti",
    author_email="massimo.fioravanti@polimi.it",
    packages=find_packages(),
    include_package_data=True,
    data_files=extra_files_bin
    + extra_files_lib
    + extra_files_share
    + [
        (
            os.path.join(site_packages_path, "impl"),
            [
                "test.py",
                "action.py",
                "learn.py",
                "play.py",
                "solve.py",
                "probs.py",
                "fix_ray.py",
                "make_report.py",
                "llmplayer.py",
            ],
        )
    ],
    entry_points={
        "console_scripts": [
            "rlc-test=impl.test:main",
            "rlc-action=impl.action:main",
            "rlc-learn=impl.learn:main",
            "rlc-play=impl.play:main",
            "rlc-random=impl.solve:main",
            "rlc-probs=impl.probs:main",
            "rlc-make-report=impl.make_report:main",
            "rlc-llmplayer=impl.llmplayer:main",
        ],
    },
    classifiers=[
        "Programming Language :: Python :: 3",
        "License :: OSI Approved :: Apache Software License",
        "Operating System :: POSIX :: Linux",
        "Operating System :: Unix",
        "Operating System :: POSIX",
        "Intended Audience :: Developers",
        "Environment :: Console",
        "Natural Language :: English",
        "Programming Language :: Python :: 3.8",
        "Programming Language :: Python :: 3.9",
        "Programming Language :: Python :: 3.10",
        "Programming Language :: Python :: 3.11",
        "Programming Language :: Python :: 3 :: Only",
        "Topic :: Software Development :: Libraries :: Python Modules",
        "Topic :: Utilities",
    ],
    python_requires=">=3.8",
    commit_hash=get_commit_hash(),
)

setup(
    name="rl_language",
    version=version,
    author="Massimo Fioravanti",
    author_email="massimo.fioravanti@polimi.it",
    install_requires=read_requirements(os.path.join("..", "run-requirements.txt")) + [f"rl_language_core=={version}"],
    packages=[],
    include_package_data=False,
    data_files=[],
    classifiers=[
        "Programming Language :: Python :: 3",
        "License :: OSI Approved :: Apache Software License",
        "Operating System :: POSIX :: Linux",
        "Operating System :: Unix",
        "Operating System :: POSIX",
        "Intended Audience :: Developers",
        "Environment :: Console",
        "Natural Language :: English",
        "Programming Language :: Python :: 3.8",
        "Programming Language :: Python :: 3.9",
        "Programming Language :: Python :: 3.10",
        "Programming Language :: Python :: 3.11",
        "Programming Language :: Python :: 3 :: Only",
        "Topic :: Software Development :: Libraries :: Python Modules",
        "Topic :: Utilities",
    ],
    commit_hash=get_commit_hash(),
)

shutil.rmtree(target_bin_dir)
shutil.rmtree("lib")
