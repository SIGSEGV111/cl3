#!/bin/bash

source "$(dirname "$(readlink -f "$0")")/common.sh"
CL3_LIBNAME="$1"
shift 1

mkdir -p "bin"

(
	set -x
	mkdir -p "tmp/gtest/build"
	cd "tmp/gtest/build"
	export CXX="$CL3_CXX"
	export CC="$CL3_CC"
	unset CPPFLAGS
	export CFLAGS="-fPIE"
	export CXXFLAGS="-fPIE"
	cmake "$CL3_GIT_ROOT/util/gtest"
	make -j $CL3_PJOBS
) >/dev/null

(
	for f in "$CL3_GIT_ROOT/src/$CL3_LIBNAME/test/"*.cpp; do
		echo "#include \"$f\""
	done
) > "tmp/gtest-$CL3_LIBNAME.cpp"

set -x
"$CL3_CXX" $CL3_CXX_OPTS $CL3_CXX_EXE $@ -I "$CL3_GIT_ROOT/util/gtest/googletest/include" "tmp/gtest-$CL3_LIBNAME.cpp" "tmp/gtest/build/googlemock/gtest/"*.a -o "bin/gtest-$CL3_LIBNAME"
