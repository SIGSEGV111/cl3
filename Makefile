.PHONY: clean core mm test
.SILENT:

all: all-release

all-release: core-release mm-release db-release
all-devel: core-devel mm-devel db-devel

clean:
	+$(MAKE) -j -C core clean
	+$(MAKE) -j -C mm clean
	+$(MAKE) -j -C db clean
	+$(MAKE) -j -C test clean

core-release:
	+$(MAKE) -j -C core release

core-devel:
	+$(MAKE) -j -C core devel


mm-release: core-release
	+$(MAKE) -j -C mm release
	
mm-devel: core-devel
	+$(MAKE) -j -C mm devel

db-release: core-release
	+$(MAKE) -j -C db release
	
db-devel: core-devel
	+$(MAKE) -j -C db devel

test: all-devel
	+$(MAKE) -j -C test
