#!/bin/bash
set -e
if ! [ -x "$(command -v virtualenv)" ]; then
  echo 'Error: virtualenv is not installed.' >&2
  exit 1
fi

mkdir rlc-infrastructure
cd rlc-infrastructure

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

