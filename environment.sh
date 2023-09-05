DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
source $DIR/../.venv/local/bin/activate
export LD_LIBRARY_PATH="$DIR/../llvm-install-release/lib/":$LD_LIBRARY_PATH
export LD_LIBRARY_PATH="$DIR/../llvm-install-debug/lib/":$LD_LIBRARY_PATH
export PATH="$DIR/../llvm-install-release/bin/":$PATH
