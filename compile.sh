#!/bin/bash

exec </dev/null

set +x
set -e
set -u
set -o pipefail
trap 'code=$?; set +x; echo "ERROR code $code at line $LINENO"; exit $code' ERR

set +u
if test -z "$CXX"; then
	export CXX="$(which g++)"
	export CC="$(which gcc)"
fi

if test -z "$BUILD_TYPE"; then
	export BUILD_TYPE="debug"
fi
set -u

case "$BUILD_TYPE" in
	debug)
		CL3_GEN_NAME="dbg"
		;;
	release)
		CL3_GEN_NAME="rel"
		;;
esac

export CL3_ROOT="$(dirname "$(readlink -f "$0")")"
export CL3_GENDIR="$PWD/gen/$CL3_GEN_NAME"
export CL3_WORKDIR="$PWD/tmp/$CL3_GEN_NAME"
export CL3_PJOBS=$(grep "^processor[[:space:]]]*:[[:space:]]*[0-9]\+$" /proc/cpuinfo | wc -l)

export CFLAGS="  -fPIC -rdynamic -g -O0 -march=native"
export CXXFLAGS="-fPIC -rdynamic -g -O0 -march=native"

rm -rf gen tmp
mkdir -p gen/{dbg,rel}/{lib,include,bin} tmp/gtest tmp/musl
export -p > "tmp/env.log"

echo "Building gtest ... "
(
	mkdir -p "tmp/gtest/build"
	cd "tmp/gtest/build"
	cmake "$CL3_ROOT/util/gtest"
	make -j $CL3_PJOBS
) > "tmp/gtest/build.log" 2>&1

# echo "Building musl ... "
# (
# 	mkdir -p "tmp/musl/build"
# 	cd "tmp/musl/build"
# 	CFLAGS="-O3 -fPIC -flto -march=native" "$CL3_ROOT/src/musl/configure" --disable-shared --enable-visibility --enable-debug
# 	make -j $CL3_PJOBS
# ) > "tmp/musl/build.log" 2>&1

for m in dbg rel; do
	mkdir -p gen/$m/include/cl3
	for f in core llvm; do
		ln -nfs "$CL3_ROOT/src/$f" "gen/$m/include/cl3/$f"
		mkdir -p "tmp/$m/$f/lib" "tmp/$m/$f/tests"
	done

	ln -nfs "$CL3_ROOT/util/gtest/googletest/include/gtest" "gen/$m/include/gtest"

	for f in tmp/gtest/build/googlemock/gtest/*.a; do
		ln -nfs "../../../$f" "gen/$m/lib/"
	done
done

if ! test -d "data"; then
	ln -snf "$CL3_ROOT/data"
fi

function RunValgrind()
{
	VALGRIND_OPTS="--quiet --leak-check=full --show-reachable=no --track-origins=yes --num-callers=30 --trace-children=no --track-fds=yes --error-exitcode=1 --suppressions=$CL3_ROOT/util/valgrind.supp --gen-suppressions=all --log-file=valgrind.log"

	if ! time valgrind $VALGRIND_OPTS "$@"; then
		cat valgrind.log
		return 1
	fi
}

function DetectInfrabox()
{
	if export -p | grep -qF "INFRABOX_CLI=" || export -p | grep -qF "INFRABOX_JOB_ID="; then
		echo 1
	else
		echo 0
	fi
}

INFRABOX=$(DetectInfrabox)

if (($INFRABOX)); then
	echo "Running inside of Infrabox"
	export -p | sort | grep -i "infrabox"
else
	echo "Running outside of Infrabox"
fi

LLVM_LDFLAGS="$(llvm-config --ldflags)"
LLVM_CXXFLAGS="$(llvm-config --cppflags) -Wno-unused-parameter"
LLVM_LIBS="$(llvm-config --libs all)"
LLVM_VER="$(llvm-config --version)"
LLVM_SYSLIBS="$(llvm-config --system-libs)"

echo "Using LLVM version: $LLVM_VER"

OPTS_ARCH="-march=native"
OPTS_BASE="$OPTS_ARCH -fvisibility-inlines-hidden -fvisibility=hidden -std=c++11 -lpthread -Wno-multichar -I $CL3_GENDIR/include -L $CL3_GENDIR/lib"
OPTS_QA="-Wno-unused-parameter -Wno-deprecated-declarations -Wno-unused-function -Wall -Wextra -Werror"

OPTS_DEBUG="-g -O0 -DCL3_DEBUG --coverage -rdynamic"
OPTS_RELEASE=" -O3 -DCL3_RELEASE"

OPTS_LIB="-fPIC -shared"
OPTS_EXE="-fPIE"

case "$BUILD_TYPE" in
	debug)
		OPTS_LIB="$OPTS_BASE $OPTS_LIB $OPTS_DEBUG"
		OPTS_EXE="$OPTS_BASE $OPTS_EXE $OPTS_DEBUG"
		;;

	release)
		OPTS_LIB="$OPTS_BASE $OPTS_LIB $OPTS_RELEASE"
		OPTS_EXE="$OPTS_BASE $OPTS_EXE $OPTS_RELEASE"
		;;
esac

OPTS_CL3CORELIB="$OPTS_LIB $OPTS_QA -ldl -lm -DINSIDE_CL3CORE"
OPTS_CL3LLVMLIB="$OPTS_LIB $OPTS_QA $LLVM_CXXFLAGS $LLVM_LDFLAGS $LLVM_LIBS $LLVM_SYSLIBS -lcl3-core -DINSIDE_CL3LLVM"

OPTS_CL3CORETESTS="$OPTS_EXE $OPTS_QA -lcl3-core"
OPTS_CL3LLVMTESTS="$OPTS_EXE $OPTS_QA $LLVM_CXXFLAGS $LLVM_LDFLAGS $LLVM_LIBS -lcl3-core -lcl3-llvm"

for f in "$CL3_GENDIR/include/cl3/core/"*.cpp "$CL3_ROOT/src/extlib/"*.cpp; do
	echo "#include \"$f\""
done > "$CL3_WORKDIR/core.cpp"

for f in "$CL3_GENDIR/include/cl3/llvm/"*.cpp; do
	echo "#include \"$f\""
done > "$CL3_WORKDIR/llvm.cpp"

set -x

cd "$CL3_WORKDIR/core/lib"
time "$CXX" -o "$CL3_GENDIR/lib/libcl3-core.so" "$CL3_WORKDIR/core.cpp" "$CL3_GENDIR/include/cl3/core/"*.S $OPTS_CL3CORELIB # "$CL3_ROOT/tmp/musl/build/lib/libc.a"

time "$CXX" -o "$CL3_GENDIR/lib/libcl3-core_noamalgam.so" "$CL3_GENDIR/include/cl3/core/"*.cpp "$CL3_GENDIR/include/cl3/core/"*.S "$CL3_ROOT/src/extlib/"*.cpp $OPTS_CL3CORELIB

cd "$CL3_WORKDIR/core/tests"
time "$CXX" -o "$CL3_GENDIR/bin/cl3-core-tests" "$CL3_GENDIR/include/cl3/core/test/"*.cpp "$CL3_GENDIR/lib/libgtest"*.a $OPTS_CL3CORETESTS

cd "$CL3_WORKDIR/llvm/lib"
time "$CXX" -o "$CL3_GENDIR/lib/libcl3-llvm.so" "$CL3_WORKDIR/llvm.cpp" $OPTS_CL3LLVMLIB

time "$CXX" -o "$CL3_GENDIR/lib/libcl3-llvm_noamalgam.so" "$CL3_GENDIR/include/cl3/llvm/"*.cpp $OPTS_CL3LLVMLIB

cd "$CL3_WORKDIR/llvm/tests"
time "$CXX" -o "$CL3_GENDIR/bin/cl3-llvm-tests" "$CL3_GENDIR/include/cl3/llvm/test/"*.cpp "$CL3_GENDIR/lib/libgtest"*.a $OPTS_CL3LLVMTESTS

set +x
cd "$CL3_WORKDIR"

export LD_LIBRARY_PATH="$CL3_GENDIR/lib"

RunValgrind "$CL3_GENDIR/bin/cl3-core-tests" '--gtest_filter=-system_memory*' --gtest_output=xml:gtest.xml dummy1 dummy2 dummy3
time "$CL3_GENDIR/bin/cl3-core-tests" '--gtest_filter=system_memory*' --gtest_output=xml:gtest.xml dummy1 dummy2 dummy3

RunValgrind "$CL3_GENDIR/bin/cl3-llvm-tests" --gtest_output=xml:gtest.xml

if test "$BUILD_TYPE" == "debug" && ((INFRABOX==0)); then
	mkdir -p "$CL3_GENDIR/coverage"
	lcov --capture --rc lcov_branch_coverage=1 \
		--directory "$CL3_WORKDIR/core/lib" --directory "$CL3_WORKDIR/core/tests" \
		--directory "$CL3_WORKDIR/llvm/lib" --directory "$CL3_WORKDIR/llvm/tests" \
		--output-file coverage.info

	lcov --remove coverage.info '*gtest*' '/usr/include/*' '*extlib*' -o coverage.info
	genhtml coverage.info --output-directory "$CL3_GENDIR/coverage"
	echo "coverage URL: $(readlink -f "$CL3_GENDIR/coverage/index.html")"
fi > "$CL3_WORKDIR/lcov.log" 2>&1
