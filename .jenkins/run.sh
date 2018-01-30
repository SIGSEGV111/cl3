#!/bin/bash

P="$(dirname "$(readlink -f "$0")")"
cd "$P/.."

exec ./compile.sh
