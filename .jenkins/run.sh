#!/bin/bash

set -e
set -x
set -u
set -o pipefail
export -p

P="$(dirname "$(readlink -f "$0")")"
cd "$P/.."

exec infrabox run
