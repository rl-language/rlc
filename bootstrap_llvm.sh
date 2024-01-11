#!/bin/bash
set -e

DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
wget -N https://github.com/llvm/llvm-project/releases/download/llvmorg-17.0.0/clang+llvm-17.0.0-x86_64-linux-gnu-ubuntu-18.04.tar.xz
tar xfv clang+llvm-17.0.0-x86_64-linux-gnu-ubuntu-18.04.tar.xz -C ./ --skip-old-files 

export PATH="$PWD/clang+llvm-16.0.0-x86_64-linux-gnu-ubuntu-18.04/bin":$PATH
export LD_LIBRARY_PATH="$PWD/clang+llvm-16.0.0-x86_64-linux-gnu-ubuntu-18.04/lib/":$LD_LIBRARY_PATH

mkdir rlc-infrastructure
cd rlc-infrastructure

# INSTALL RLC
git clone git@github.com:rl-language/rlc.git 

# SETUP PYTHON
virtualenv .venv
source .venv/local/bin/activate 
pip install -r rlc/requirements.txt
deactivate
source ./rlc/environment.sh

cd rlc
git submodule init 
git submodule update --recursive  
cd ..

python ./rlc/build.py --no-debug-llvm --cxx-compiler $PWD/../clang+llvm-17.0.0-x86_64-linux-gnu-ubuntu-18.04/bin/clang++ --c-compiler $PWD/../clang+llvm-17.0.0-x86_64-linux-gnu-ubuntu-18.04/bin/clang



