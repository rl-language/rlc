from setuptools import setup, find_packages
import os
import shutil
import subprocess


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
        commit_hash = subprocess.check_output(['git', 'rev-parse', 'HEAD']).strip().decode('utf-8')
    except subprocess.CalledProcessError:
        commit_hash = 'unknown'
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
copy_binaries("../../rlc-release/install/bin/rlc", "./bin")
copy_binaries("../../rlc-release/install/bin/rlc-lsp", "./bin")
copy_binaries("../../rlc-release/install/lib/libruntime.a", "./lib/")
copy_binaries("../../rlc-release/install/lib/rlc", "./lib/rlc/")
extra_files_bin = package_files(["./bin/"], "bin")
extra_files_lib = package_files(["./lib/"], "lib")

setup(
    name="rl_language",
    version="0.1.13",
    author="Massimo Fioravanti",
    author_email="massimo.fioravanti@polimi.it",
    packages=find_packages(),
    include_package_data=True,
    data_files=extra_files_bin
    + extra_files_lib
    + [("./bin/impl", ["./test.py", "./action.py", "./learn.py", "./play.py", "./solve.py", "./probs.py"])],
    install_requires=read_requirements("../run-requirements.txt"),
    entry_points={
        "console_scripts": [
            "rlc-test=impl.test:main",
            "rlc-action=impl.action:main",
            "rlc-learn=impl.learn:main",
            "rlc-play=impl.play:main",
            "rlc-random=impl.solve:main",
            "rlc-probs=impl.probs:main",
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
    commit_hash=get_commit_hash()
)

shutil.rmtree("bin/")
shutil.rmtree("lib/")
