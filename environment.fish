source ../.venv/bin/activate.fish
set -x LD_LIBRARY_PATH (realpath ../llvm-install-release/lib):$LD_LIBRARY_PATH
set -x LD_LIBRARY_PATH (realpath ../llvm-install-debug/lib):$LD_LIBRARY_PATH
set -x PATH (realpath ../llvm-install-release/bin/):$PATH
