DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
source $DIR/../.venv/local/bin/activate
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:`realpath $DIR/../llvm-install-release/lib/`
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:`realpath $DIR/../llvm-install-debug/lib/`
export PATH=$PATH:`realpath $DIR/../llvm-install-release/bin/`
