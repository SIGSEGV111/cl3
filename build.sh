#!/bin/bash

clear;clear
if test -z "$CXX"; then
	export CXX=$(which clang)
fi
make -j all-devel || (clear;clear;make -j1 all-devel) && make test
