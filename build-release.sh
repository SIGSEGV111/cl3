#!/bin/bash

source util/build-base.sh

cd tmp/rel && cmake ../.. -DBUILD_TYPE=release && make -j $PJOBS || make -j 1
