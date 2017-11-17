FROM v2-kube.wdf.sap.corp:5000/opensuse-tumbleweed-clang:20171124132347
ADD . /cl3
CMD cd /cl3 && CC=clang CXX=clang++ ./build-debug.sh
