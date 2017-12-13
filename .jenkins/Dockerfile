FROM opensuse:latest
MAINTAINER simon@brennecke-it.net
RUN sed -i 's/https/http/g' /etc/zypp/repos.d/*
RUN zypper in -y git gcc-c++ clang make cmake unzip llvm-devel valgrind valgrind-devel tar ncurses-devel zlib-devel
