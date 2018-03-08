#!/bin/bash

P="$(dirname "$(readlink -f "$0")")"

set -x
set -u
cd "$P/.."

infrabox run
