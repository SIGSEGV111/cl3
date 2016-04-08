#!/bin/bash
while true; do unbuffer ./build-debug.sh | less -r -K; sleep 1; done
