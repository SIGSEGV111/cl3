#!/bin/bash

source "$(dirname "$(readlink -f "$0")")/common.sh"
CL3_LIBNAME="$1"
CL3_LIBNAME_UPPER="$(echo "$CL3_LIBNAME" | tr 'a-z' 'A-Z')"

function GenerateSourceBundle()
{
	mkdir -p "$CL3_GENDIR/include/cl3/$1"

	cp --force --reflink=auto "$CL3_GIT_ROOT/src/$1/"*.hpp "$CL3_GENDIR/include/cl3/$1/"
	cp --force --reflink=auto "$CL3_GIT_ROOT/src/$1/"*.cpp "$CL3_GENDIR/include/cl3/$1/"

	chmod 0444 "$CL3_GENDIR/include/cl3/$1/"*

	(
		cd "$CL3_GENDIR/include/cl3/"
		echo "#ifndef _include_cl3_$1_bundle_"
		echo "#define _include_cl3_$1_bundle_"
		echo "#define CL3_${CL3_LIBNAME_UPPER}_SOURCE_BUNDLE"

		for f in "$1/"*.hpp "$1/"*.cpp; do
			echo "#include \"$f\""
		done

		echo "#endif"
	) > "$CL3_GENDIR/include/cl3/$1.cpp"
}

GenerateSourceBundle "$CL3_LIBNAME"
