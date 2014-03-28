#!/bin/bash
clear;clear
make -j test || (clear;clear;make -j1 test) && (sleep 2;clear;ssh sles11 "sudo su -l sb1adm -c 'python testscripts/testGisPairwise_STD.py'")
