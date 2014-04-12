#!/bin/bash

clear;clear
make -j all || (clear;clear;make -j1 all) && make test
