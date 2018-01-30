#!/bin/bash

set -e
set -x

rm -rf "tmp/package/debian"

export DEB_TARGET_ARCH="$(dpkg --print-architecture)"

export LLVM_VERSION="$(llvm-config --version)"
export LLVM_PKGVERSION="$(grep -o "^[0-9]\+\.[0-9]\+" <<<"$LLVM_VERSION")"

mkdir -p "tmp/package/debian/libcl3-core/usr/lib"
mkdir -p "tmp/package/debian/libcl3-core/DEBIAN"
cat > "tmp/package/debian/libcl3-core/DEBIAN/control" << EOF
Package: libcl3-core
Version: $CL3_VERSION
Maintainer: simon@brennecke-it.net
Architecture: $DEB_TARGET_ARCH
Depends: libllvm$LLVM_PKGVERSION
Description: The Common Library tries to a universal application base framework with implements the common needs of large applications. It tries to do so while not depending on many external libraries and if dependencyies cannot be reasonable avoided they are made optional.
EOF

mkdir -p "tmp/package/debian/libcl3-core-dev/usr/include/cl3/core"
mkdir -p "tmp/package/debian/libcl3-core-dev/DEBIAN"
cat > "tmp/package/debian/libcl3-core-dev/DEBIAN/control" << EOF
Package: libcl3-core-dev
Version: $CL3_VERSION
Maintainer: simon@brennecke-it.net
Architecture: all
Depends: libcl3-core (= $CL3_VERSION)
Description: The Common Library application development headers
EOF

mkdir -p "tmp/package/debian/libcl3-core-tests/usr/local/bin"
mkdir -p "tmp/package/debian/libcl3-core-tests/opt/cl3/data"
mkdir -p "tmp/package/debian/libcl3-core-tests/DEBIAN"
cat > "tmp/package/debian/libcl3-core-tests/DEBIAN/control" << EOF
Package: libcl3-core-tests
Version: $CL3_VERSION
Maintainer: simon@brennecke-it.net
Architecture: $DEB_TARGET_ARCH
Depends: libcl3-core (= $CL3_VERSION)
Description: The Common Library unit tests
EOF

cp "gen/rel/lib/libcl3-core.so" "tmp/package/debian/libcl3-core/usr/lib"
cp src/core/*.hpp               "tmp/package/debian/libcl3-core-dev/usr/include/cl3/core/"
cp "gen/rel/bin/cl3-core-test"  "tmp/package/debian/libcl3-core-tests/usr/local/bin"
cp -r "data/tests"  "tmp/package/debian/libcl3-core-tests/opt/cl3/data/"

dpkg-shlibdeps -e "tmp/package/debian/libcl3-core/usr/lib/libcl3-core.so" -O "tmp/package/debian/substvars"

dpkg-deb --build "tmp/package/debian/libcl3-core"       "gen/packages/libcl3-core.deb"
dpkg-deb --build "tmp/package/debian/libcl3-core-dev"   "gen/packages/libcl3-core-dev.deb"
dpkg-deb --build "tmp/package/debian/libcl3-core-tests" "gen/packages/libcl3-core-tests.deb"
