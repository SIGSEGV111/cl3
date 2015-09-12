#!/bin/bash
exec >/dev/null 2>&1 </dev/null

kate $(find src -iname "*.cpp" -or -iname "*.hpp") &
