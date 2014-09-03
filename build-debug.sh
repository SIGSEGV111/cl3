#!/bin/bash

source util/build-base.sh

export SHELL="$PWD/util/clean-term.sh"

cd tmp/dbg && cmake ../.. -DBUILD_TYPE=debug && make -j 1
