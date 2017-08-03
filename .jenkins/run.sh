#!/bin/bash

set -x
set -e
set -o pipefail

export -p
rm -vrf gen tmp

exec ./build-debug.sh 2>&1 </dev/null
