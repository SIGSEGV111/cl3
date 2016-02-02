#!/bin/bash
while true; do unbuffer ./build-debug.sh | less -r; sleep 1; done
