FROM v2-kube.wdf.sap.corp:5000/opensuse-tumbleweed-gcc:build_57
ADD . /cl3
CMD cd /cl3 && CC=gcc CXX=g++ ./build-debug.sh
