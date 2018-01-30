Summary: Common Library v3 Core unit tests
Name: libcl3-core-tests
Version: 3.0
Release: 1
License: GPL v3
Group: Development/Libraries/C and C++
Vendor: Simon Brennecke <simon@brennecke-it.net>
Packager: Simon Brennecke <simon@brennecke-it.net>
Requires: libcl3-core == 3.0

%description
This is a unit test package for cl3. This is only for the cl3 developers. Not for the users of cl3.

%prep
ln -vsnf "$CL3_SOURCE_DIR" "$RPM_SOURCE_DIR/$RPM_PACKAGE_NAME"
ln -vsnf "$CL3_SOURCE_DIR" "$RPM_BUILD_DIR/$RPM_PACKAGE_NAME"

%build

%install
mkdir -p "$RPM_BUILD_ROOT/usr/local/bin"
mkdir -p "$RPM_BUILD_ROOT/opt/cl3/data"
cp "$RPM_BUILD_DIR/$RPM_PACKAGE_NAME/gen/rel/bin/cl3-core-test" "$RPM_BUILD_ROOT/usr/local/bin/cl3-core-test"
cp -rv "$RPM_BUILD_DIR/$RPM_PACKAGE_NAME/data/tests" "$RPM_BUILD_ROOT/opt/cl3/data/"

%files
/usr/local/bin/cl3-core-test
/opt/cl3
