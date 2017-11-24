#!/bin/bash

source util/build-base.sh

(cd tmp/dbg && cmake ../.. -DBUILD_TYPE=debug && make -j $PJOBS)

source util/build-post.sh
