#!/bin/bash

source util/build-base.sh

cd tmp/dbg && cmake ../.. -DBUILD_TYPE=debug && (make -j $PJOBS || make -j 1)
