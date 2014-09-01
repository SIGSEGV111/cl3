#!/bin/bash

export PJOBS=$(grep "^processor[[:space:]]]*:[[:space:]]*[0-9]\+$" /proc/cpuinfo | wc -l)

clear;clear
if test -z "$CXX"; then
	export CXX=$(which g++)
fi
make -j $PJOBS all-devel || (clear;clear;make -j1 all-devel) && make test
