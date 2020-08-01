#!/bin/bash

source "$(dirname "$(readlink -f "$0")")/common.sh"
CL3_LIBNAME="$1"
shift 1

"$CL3_GIT_ROOT/util/build-source-bundle.sh" "$CL3_LIBNAME"

mkdir -p "tmp/$CL3_LIBNAME" "lib" "bin"

CL3_LIBNAME_UPPER="$(echo "$CL3_LIBNAME" | tr 'a-z' 'A-Z')"
CL3_LIBOPT="-DINSIDE_CL3$CL3_LIBNAME_UPPER $@"
CL3_LIBOUTFILE="lib/libcl3-$CL3_LIBNAME.so"

if test "$CL3_AMALGAM" == "yes"; then
	set -x
	"$CL3_CXX" $CL3_CXX_OPTS $CL3_CXX_LIB $CL3_LIBOPT "-DCL3_${CL3_LIBNAME_UPPER}_AMALGAM" "include/cl3/$CL3_LIBNAME.cpp" -o "$CL3_LIBOUTFILE"
else
	for sf in "include/cl3/$CL3_LIBNAME/"*.cpp; do
		bn="$(basename "$sf")"
		of="tmp/$CL3_LIBNAME/${bn:0:$((${#bn}-4))}.o"

		if test "$sf" -nt "$of"; then
			"$CL3_CXX" $CL3_CXX_OPTS $CL3_CXX_LIB $CL3_LIBOPT -c "$sf" -o "$of" &
		fi
	done
	wait

	"$CL3_CXX" $CL3_CXX_OPTS $CL3_CXX_LIB $CL3_LIBOPT "tmp/$CL3_LIBNAME/"*.o -o "$CL3_LIBOUTFILE"
fi

if test "$CL3_STRIP" == "yes"; then
	strip "$CL3_LIBOUTFILE"
fi
