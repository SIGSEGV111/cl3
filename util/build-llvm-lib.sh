#!/bin/bash

P="$(dirname "$(readlink -f "$0")")"

LLVM_LDFLAGS="$(llvm-config --ldflags)"
LLVM_CXXFLAGS="$(llvm-config --cppflags) -Wno-unused-parameter"
LLVM_LIBS="$(llvm-config --libs all)"
LLVM_VER="$(llvm-config --version)"
LLVM_SYSLIBS="$(llvm-config --system-libs)"

exec "$P/build-lib.sh" "llvm" "-lcl3-core $LLVM_CXXFLAGS $LLVM_LDFLAGS $LLVM_LIBS $LLVM_SYSLIBS"
