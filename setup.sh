#!/bin/bash
set -e
if ! [ -x "$(command -v python3)" ]; then
  echo 'Error: python3 is not installed.' >&2
  exit 1
fi

if ! [ -x "$(command -v cmake)" ]; then
  echo 'Error: cmake is not installed.' >&2
  exit 1
fi

if ! [ -x "$(command -v ninja)" ]; then
  echo 'Error: ninja is not installed.' >&2
  exit 1
fi

mkdir rlc-infrastructure
cd rlc-infrastructure

# INSTALL RLC
git clone https://github.com/rl-language/rlc.git

# SETUP PYTHON
python3 -m pip install virtualenv --user
python3 -m virtualenv .venv
source .venv/bin/activate 
pip install -r rlc/requirements.txt
deactivate

cd rlc
source ./rlc/environment.sh
git submodule init 
git submodule update --recursive  
cd ..

