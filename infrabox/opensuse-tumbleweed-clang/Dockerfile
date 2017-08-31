FROM v2-kube.wdf.sap.corp:5000/opensuse-tumbleweed-clang:build_57
ADD . /cl3
CMD cd /cl3 && CC=clang CXX=clang++ ./build-debug.sh
