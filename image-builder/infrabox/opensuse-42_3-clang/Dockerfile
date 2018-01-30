FROM opensuse:42.3
MAINTAINER simon@brennecke-it.net
RUN sed -i 's/https/http/g' /etc/zypp/repos.d/*
RUN zypper in -y git gcc-c++ clang tar rpmbuild make cmake lcov python python-xml unzip ncurses-devel zlib-devel llvm-devel valgrind valgrind-devel

