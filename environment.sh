DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
source $DIR/../.venv/bin/activate
export LD_LIBRARY_PATH="$DIR/../llvm-install-release/lib/":$LD_LIBRARY_PATH
export LD_LIBRARY_PATH="$DIR/../llvm-install-debug/lib/":$LD_LIBRARY_PATH
export PATH="$DIR/../llvm-install-release/bin/":$PATH
if [ "$(uname)" == "Darwin" ]; then
    export PATH="/opt/homebrew/opt/ruby@3.3/bin:$PATH"
    export SDKROOT=$(xcrun --sdk macosx --show-sdk-path)
fi 
