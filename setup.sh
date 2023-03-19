#!/bin/bash
set -e
if ! [ -x "$(command -v cmake)" ]; then
  echo 'Error: cmake is not installed.' >&2
  exit 1
fi

if ! [ -x "$(command -v lld)" ]; then
  echo 'Error: lld is not installed.' >&2
  exit 1
fi

if ! [ -x "$(command -v ninja)" ]; then
  echo 'Error: ninja is not installed.' >&2
  exit 1
fi

if ! [ -x "$(command -v virtualenv)" ]; then
  echo 'Error: virtualenv is not installed.' >&2
  exit 1
fi

mkdir rlc-infrastructure
cd rlc-infrastructure

# INSTALL LLVM
git clone git@github.com:llvm/llvm-project.git --depth=1 -b release/16.x
mkdir llvm-install-debug
mkdir llvm-debug
mkdir llvm-install-release
mkdir llvm-release
cd llvm-debug
cmake -DLLVM_INSTALL_UTILS=True -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=../llvm-install-debug/ -DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra;mlir;" -DLLVM_USE_LINKER=lld ../llvm-project/llvm -DCMAKE_EXPORT_COMPILE_COMMANDS=True -G Ninja -DBUILD_SHARED_LIBS=ON
ninja all
ninja install
cd ../
cd llvm-release
cmake -DLLVM_INSTALL_UTILS=True -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=../llvm-install-release/ -DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra;mlir;" -DLLVM_USE_LINKER=lld ../llvm-project/llvm -DCMAKE_EXPORT_COMPILE_COMMANDS=True -G Ninja
ninja all
ninja install
cd ../

# INSTALL RLC
git clone git@github.com:drblallo/rlc.git 

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
mkdir rlc-install-debug
mkdir rlc-debug
cd rlc-debug
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=../rlc-install-debug/ ../rlc -DCMAKE_EXPORT_COMPILE_COMMANDS=True -G Ninja -DMLIR_DIR=../llvm-install-debug/lib/cmake/mlir -DLLVM_DIR=../llvm-install-debug/lib/cmake/llvm -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DBUILD_SHARED_LIBS=ON
ninja all
ninja test
ninja install

# RUN 
python python/main.py --source ./tool/rlc/test/tris.rl
echo "ALL DONE!"


