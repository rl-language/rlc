DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:"$DIR/../rlc-release/install/lib/"
export PATH=$PATH:"$DIR/../rlc-release/install/bin/"

