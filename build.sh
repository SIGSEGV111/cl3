#!/bin/bash
clear;clear
make all || (clear;clear;make -j1 all) && make test
