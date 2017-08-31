FROM v2-kube.wdf.sap.corp:5000/debian-unstable-gcc:build_57
ADD . /cl3
CMD cd /cl3 && CC=gcc CXX=g++ ./build-debug.sh
