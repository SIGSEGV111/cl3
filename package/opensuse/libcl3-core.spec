Summary: Common Library v3 Core
Name: libcl3-core
Version: 3.0
Release: 1
License: GPL v3
Group: Development/Libraries/C and C++
Vendor: Simon Brennecke <simon@brennecke-it.net>
Packager: Simon Brennecke <simon@brennecke-it.net>

%description
The Common Library is a universal application base framework with implements the common needs of large applications.

%prep
ln -vsnf "$CL3_SOURCE_DIR" "$RPM_SOURCE_DIR/$RPM_PACKAGE_NAME"
ln -vsnf "$CL3_SOURCE_DIR" "$RPM_BUILD_DIR/$RPM_PACKAGE_NAME"

%build

%install
mkdir -p "$RPM_BUILD_ROOT/usr/lib64"
cp "$RPM_BUILD_DIR/$RPM_PACKAGE_NAME/gen/rel/lib/libcl3-core.so" "$RPM_BUILD_ROOT/usr/lib64/libcl3-core.so"

%files
/usr/lib64/libcl3-core.so
