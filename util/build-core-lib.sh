#!/bin/bash

P="$(dirname "$(readlink -f "$0")")"

exec "$P/build-lib.sh" "core" "-ldl"
