FROM v2-kube.wdf.sap.corp:5000/debian-9_0-clang:20171124132347
ADD . /cl3
CMD cd /cl3 && CC=clang CXX=clang++ ./build-debug.sh
