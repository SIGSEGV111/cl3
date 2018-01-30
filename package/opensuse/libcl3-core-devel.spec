Summary: Common Library v3 Core headers and development fileas
Name: libcl3-core-devel
Version: 3.0
Release: 1
License: GPL v3
Group: Development/Libraries/C and C++
Vendor: Simon Brennecke <simon@brennecke-it.net>
Packager: Simon Brennecke <simon@brennecke-it.net>
Requires: libcl3-core == 3.0

%description
Common Library application development headers.

%prep
ln -vsnf "$CL3_SOURCE_DIR" "$RPM_SOURCE_DIR/$RPM_PACKAGE_NAME"
ln -vsnf "$CL3_SOURCE_DIR" "$RPM_BUILD_DIR/$RPM_PACKAGE_NAME"

%build

%install
mkdir -p "$RPM_BUILD_ROOT/usr/include/cl3/core"
cp "$RPM_SOURCE_DIR/$RPM_PACKAGE_NAME"/src/core/*.hpp "$RPM_BUILD_ROOT/usr/include/cl3/core"

%files
/usr/include/cl3/core
