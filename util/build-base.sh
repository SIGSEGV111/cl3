#!/bin/bash

set -e

export cl3_libs="core mm db gpio"

D=$(dirname "$0")
export ROOT_DIR="$(bash -c "cd \"$D\""' && echo "$PWD"')"
export PJOBS=$(grep "^processor[[:space:]]]*:[[:space:]]*[0-9]\+$" /proc/cpuinfo | wc -l)

cd "$ROOT_DIR"

export SHELL="$ROOT_DIR/util/clean-term.sh"

mkdir -p gen/{dbg,rel}/{lib,include,bin} tmp/{dbg,rel} gen/dbg/coverage

GTEST_V="1.8.0"

if ! test -L "tmp/gtest/build"; then
	echo -n "building google-test framework ... "
	rm -rf "tmp/gtest"
	mkdir -p "tmp/gtest"
	cd "tmp/gtest"
	tar -xf "../../util/gtest-$GTEST_V.tar.gz"
	cd "googletest-release-$GTEST_V"
	cmake . && make -j $PJOBS
	cd ..
	ln -nsf "googletest-release-$GTEST_V" build
	echo "done"
	cd "$ROOT_DIR"
fi

for m in dbg rel; do
	mkdir -p gen/$m/include/cl3
	for f in $cl3_libs; do
		ln -nfs ../../../tmp/$m/src/$f/libcl3-${f}.so gen/$m/lib/
		ln -nfs ../../../tmp/$m/src/$f/libcl3-${f}_static.a  gen/$m/lib/
		ln -nfs ../../../../src/$f gen/$m/include/cl3/$f
		ln -nfs ../../../tmp/$m/src/$f/test/cl3-$f-test gen/$m/bin/
	done
	ln -nfs ../../../tmp/gtest/build/googletest/include/gtest gen/$m/include/gtest

	for f in tmp/gtest/build/googlemock/gtest/*; do
		ln -nfs ../../../"$f" gen/$m/lib/
	done
done

for f in $cl3_libs; do
	ln -nfs ../../../tmp/dbg/src/$f/test/coverage/ gen/dbg/coverage/$f
done
