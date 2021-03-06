#!/bin/bash

exec </dev/null

set +x
set -e
set -u
set -o pipefail
trap 'code=$?; set +x; echo "ERROR code $code at line $LINENO" 1>&2; wait || true; exit $code' ERR

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
	set -x
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
# 		ln -nfs "$CL3_ROOT/src/$f" "gen/$m/include/cl3/$f"
		mkdir -p "tmp/$m/$f/lib" "tmp/$m/$f/tests"
	done

	ln -nfs "$CL3_ROOT/util/gtest/googletest/include/gtest" "gen/$m/include/gtest"

	for f in tmp/gtest/build/lib/*.a; do
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

ln -vsnf "$CL3_GENDIR" "$CL3_ROOT/gen/last-build" || true

OPTS_ARCH="-march=native"
OPTS_BASE="$OPTS_ARCH -fvisibility-inlines-hidden -fvisibility=hidden -std=c++11 -fdata-sections -ffunction-sections -flto -lpthread -Wno-multichar -I $CL3_GENDIR/include -L $CL3_GENDIR/lib"
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

function GenerateHeaderBundle()
{
	dst_dir="$CL3_GENDIR/include/cl3/$1"
	mkdir -p "$dst_dir"

	cp "$CL3_ROOT/src/$1/"*.hpp "$dst_dir/"

	(
		cd "$dst_dir"
		for file in *.hpp; do
			echo "#include \"$1/$file\""
		done
	) > "$CL3_GENDIR/include/cl3/$1.hpp"
}

function GenerateSourceBundle()
{
	dst_dir="$CL3_GENDIR/src/cl3/$1"
	mkdir -p "$dst_dir"

	cp "$CL3_ROOT/src/$1/"*.cpp "$dst_dir"

	for f in "$CL3_ROOT/src/$1/"*.hpp; do
		ln -snf "$f" "$dst_dir/"
	done

	(
		cd "$dst_dir"
		echo "#ifndef _include_cl3_core_bundle_"
		echo "#define _include_cl3_core_bundle_"
		echo "#define CL3_SOURCE_BUNDLE"

		for file in *.cpp; do
			echo "#include \"../../src/cl3/$1/$file\""
		done

		echo "#endif"
	) > "$CL3_GENDIR/include/cl3/$1.cpp"
}

function TestSourceBundle()
{
	cd "$CL3_WORKDIR"
	time "$CXX" -o "test-source-bundle-$1" "$CL3_ROOT/util/test-source-bundle-$1.cpp" $OPTS_EXE $2
	"./test-source-bundle-$1"
}

function GenerateLibAmalgam()
{
	for f in "$CL3_GENDIR/src/cl3/$1/"*.cpp; do
# 		f="$(readlink -f "$f")"
		echo "#include \"$f\""
	done > "$CL3_WORKDIR/$1-lib.cpp"
}

function GenerateTestAmalgam()
{
	for f in "$CL3_ROOT/src/$1/test/"*.cpp; do
# 		f="$(readlink -f "$f")"
		echo "#include \"$f\""
	done > "$CL3_WORKDIR/$1-test.cpp"
}

GenerateSourceBundle core
GenerateSourceBundle llvm

GenerateHeaderBundle core
GenerateHeaderBundle llvm

TestSourceBundle core "-ldl -lm" &
# TestSourceBundle llvm "-ldl -lm $LLVM_CXXFLAGS $LLVM_LDFLAGS $LLVM_LIBS" &

GenerateLibAmalgam core
GenerateLibAmalgam llvm

GenerateTestAmalgam core
GenerateTestAmalgam llvm

make -C "$CL3_ROOT/examples/how-to-use-source-bundle" clean
time make -C "$CL3_ROOT/examples/how-to-use-source-bundle" example & p3=$!

cd "$CL3_WORKDIR/core/lib"
time "$CXX" -o "$CL3_GENDIR/lib/libcl3-core.so" "$CL3_WORKDIR/core-lib.cpp" $OPTS_CL3CORELIB & p1=$! # "$CL3_ROOT/tmp/musl/build/lib/libc.a"
time "$CXX" -o "$CL3_GENDIR/lib/libcl3-core_noamalgam.so" "$CL3_GENDIR/src/cl3/core/"*.cpp $OPTS_CL3CORELIB &
wait $p1

cd "$CL3_WORKDIR/core/tests"
time "$CXX" -o "$CL3_GENDIR/bin/cl3-core-tests" "$CL3_WORKDIR/core-test.cpp" "$CL3_GENDIR/lib/libgtest"*.a $OPTS_CL3CORETESTS & p2=$!

cd "$CL3_WORKDIR/llvm/lib"
time "$CXX" -o "$CL3_GENDIR/lib/libcl3-llvm.so" "$CL3_WORKDIR/llvm-lib.cpp" $OPTS_CL3LLVMLIB & p1=$!
time "$CXX" -o "$CL3_GENDIR/lib/libcl3-llvm_noamalgam.so" "$CL3_GENDIR/src/cl3/llvm/"*.cpp $OPTS_CL3LLVMLIB &
wait $p1

cd "$CL3_WORKDIR/llvm/tests"
time "$CXX" -o "$CL3_GENDIR/bin/cl3-llvm-tests" "$CL3_WORKDIR/llvm-test.cpp" "$CL3_GENDIR/lib/libgtest"*.a $OPTS_CL3LLVMTESTS & p1=$!

cd "$CL3_WORKDIR"

export LD_LIBRARY_PATH="$CL3_GENDIR/lib"

wait $p2
RunValgrind "$CL3_GENDIR/bin/cl3-core-tests" '--gtest_filter=-system_memory*' --gtest_output=xml:gtest.xml dummy1 dummy2 dummy3
time "$CL3_GENDIR/bin/cl3-core-tests" '--gtest_filter=system_memory*' --gtest_output=xml:gtest.xml dummy1 dummy2 dummy3

wait $p1
wait $p3

RunValgrind "$CL3_GENDIR/bin/cl3-llvm-tests" --gtest_output=xml:gtest.xml

time make -C "$CL3_ROOT/examples/how-to-use-source-bundle" run

if test "$BUILD_TYPE" == "debug" && ((INFRABOX==0)); then
	(
		set -x
		mkdir -p "$CL3_GENDIR/coverage"
		lcov --capture --rc lcov_branch_coverage=1 --rc geninfo_auto_base=1 \
			--directory "$CL3_WORKDIR/core/lib" --directory "$CL3_WORKDIR/core/tests" \
			--directory "$CL3_WORKDIR/llvm/lib" --directory "$CL3_WORKDIR/llvm/tests" \
			--output-file coverage.info

		lcov --remove coverage.info '*gtest*' '/usr/include/*' '*/extern_*' '*/test/*' -o coverage.info
		genhtml coverage.info --output-directory "$CL3_GENDIR/coverage"
	) > "$CL3_WORKDIR/lcov.log" 2>&1
	echo "coverage URL: $(readlink -f "$CL3_GENDIR/coverage/src/cl3/core/index-sort-l.html")"
fi

if ((${BASH_VERSINFO[0]} > 4 || (${BASH_VERSINFO[0]} == 4 && ${BASH_VERSINFO[1]} >= 3))); then
	trap - ERR
	set +e
	while true; do
		wait -n
		code=$?
		if(($code == 127)); then
			break;
		elif(($code != 0)); then
			echo "ERROR: background task failed with code $code" 1>&2
			exit $code
		fi
	done
	set -e
else
	wait
fi

echo "Build successful"
