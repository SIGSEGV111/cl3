#!/bin/bash

set -e

D=$(dirname "$0")
export ROOT_DIR="$(bash -c "cd \"$D\""' && echo "$PWD"')"
export PJOBS=$(grep "^processor[[:space:]]]*:[[:space:]]*[0-9]\+$" /proc/cpuinfo | wc -l)

cd "$ROOT_DIR"

export SHELL="$ROOT_DIR/util/clean-term.sh"

mkdir -p gen/{dbg,rel}/{lib,include,bin} tmp/{dbg,rel}

GTEST_V="1.7.0"
CPPCHECK_V="1.66"

if ! test -L "tmp/gtest/build"; then
	echo -n "building google-test framework ... "
	rm -rf "tmp/gtest"
	mkdir -p "tmp/gtest"
	cd "tmp/gtest"
	unzip "../../util/gtest-$GTEST_V.zip" >/dev/null
	cd "gtest-$GTEST_V"
	CXXFLAGS="-march=native -fno-lto" ./configure >/dev/null
	make -s -j $PJOBS >/dev/null
	cd ..
	ln -nsf "gtest-$GTEST_V" build
	echo "done"
	cd "$ROOT_DIR"
fi

if ! test -L "tmp/cppcheck/build"; then
	echo -n "building cppcheck ... "
	rm -rf "tmp/cppcheck"
	mkdir -p "tmp/cppcheck"
	cd "tmp/cppcheck"
	tar -jxf "../../util/cppcheck-$CPPCHECK_V.tar.bz2"
	cd "cppcheck-$CPPCHECK_V"
	make -s -j $PJOBS SRCDIR=build CFGDIR="$ROOT_DIR/util/cppcheck-$CPPCHECK_V/cfg" HAVE_RULES=yes CXX="$(which g++)" >/dev/null
	cd ..
	ln -nsf "cppcheck-$CPPCHECK_V" build
	echo "done"
	cd "$ROOT_DIR"
fi

for m in dbg rel; do
	mkdir -p gen/$m/include/cl3
	for f in core mm db; do
		ln -nfs ../../../tmp/$m/src/$f/libcl3-${f}.so gen/$m/lib/
		ln -nfs ../../../tmp/$m/src/$f/libcl3-${f}_static.a  gen/$m/lib/
		ln -nfs ../../../../src/$f gen/$m/include/cl3/$f
		ln -nfs ../../../tmp/$m/src/$f/test/cl3-$f-test gen/$m/bin/
	done
	ln -nfs ../../../tmp/gtest/build/include/gtest gen/$m/include/gtest

	for f in tmp/gtest/build/lib/.libs/*; do
		ln -nfs ../../../"$f" gen/$m/lib/
	done
done
