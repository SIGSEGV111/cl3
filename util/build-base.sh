#!/bin/bash

set -e

export cl3_libs="core mm db gpio"

D=$(dirname "$0")
export ROOT_DIR="$(bash -c "cd \"$D\""' && echo "$PWD"')"
export PJOBS=$(grep "^processor[[:space:]]]*:[[:space:]]*[0-9]\+$" /proc/cpuinfo | wc -l)

cd "$ROOT_DIR"

export SHELL="$ROOT_DIR/util/clean-term.sh"

mkdir -p gen/{dbg,rel}/{lib,include,bin} tmp/{dbg,rel} gen/dbg/coverage

cd "$ROOT_DIR"
mkdir -p "tmp/gtest/build"
cd "tmp/gtest/build"
cmake "$ROOT_DIR/util/gtest"
make -j $PJOBS

cd "$ROOT_DIR"
for m in dbg rel; do
	mkdir -p gen/$m/include/cl3
	for f in $cl3_libs; do
		ln -nfs ../../../tmp/$m/src/$f/libcl3-${f}.so gen/$m/lib/
		ln -nfs ../../../tmp/$m/src/$f/libcl3-${f}_static.a  gen/$m/lib/
		ln -nfs ../../../../src/$f gen/$m/include/cl3/$f
		ln -nfs ../../../tmp/$m/src/$f/test/cl3-$f-test gen/$m/bin/
	done

	ln -nfs ../../../util/gtest/googletest/include/gtest gen/$m/include/gtest

	for f in tmp/gtest/build/googlemock/gtest/*; do
		ln -nfs ../../../"$f" gen/$m/lib/
	done
done

for f in $cl3_libs; do
	ln -nfs ../../../tmp/dbg/src/$f/test/coverage/ gen/dbg/coverage/$f
done
