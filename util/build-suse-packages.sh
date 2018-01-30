#!/bin/bash

set -e
set -x

export CL3_SOURCE_DIR="$PWD"
rpmbuild -ba package/opensuse/libcl3-core.spec
rpmbuild -ba package/opensuse/libcl3-core-devel.spec
rpmbuild -ba package/opensuse/libcl3-core-tests.spec

if test -d "$HOME/rpmbuild"; then
	export RPMBUILD_DIR="$HOME/rpmbuild"
else
	export RPMBUILD_DIR="/usr/src/packages"
fi

cp -v "$RPMBUILD_DIR/SRPMS/"libcl3-core-3.0*.src.rpm "$RPMBUILD_DIR/RPMS/x86_64"/*.rpm "gen/packages/"
