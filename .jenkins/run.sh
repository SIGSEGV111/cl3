#!/bin/bash

set -x
set -e
set -o pipefail

export -p
rm -vrf gen tmp

./build-debug.sh 2>&1 </dev/null
./build-release.sh 2>&1 </dev/null
