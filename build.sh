#!/bin/bash

clear;clear
if test -z "$CXX"; then
	export CXX=$(which clang)
fi
make -j all || (clear;clear;make -j1 all) && make test
