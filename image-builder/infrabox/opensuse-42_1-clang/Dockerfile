FROM opensuse:42.1
MAINTAINER simon@brennecke-it.net
RUN sed -i 's/https/http/g' /etc/zypp/repos.d/*
RUN export http_proxy=http://147.204.6.136:8080; zypper in -y git gcc-c++ clang tar make cmake python python-xml unzip ncurses-devel zlib-devel llvm-devel valgrind valgrind-devel

