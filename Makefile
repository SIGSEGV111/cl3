.PHONY: clean core mm test

all: all-release

all-release: core-release mm-release
all-devel: core-devel mm-devel

clean:
	$(MAKE) -j -C core clean
	$(MAKE) -j -C mm clean
	$(MAKE) -j -C test clean

core-release:
	$(MAKE) -j -C core release

core-devel:
	$(MAKE) -j -C core devel


mm-release: core-release
	$(MAKE) -j -C mm release
	
mm-devel: core-devel
	$(MAKE) -j -C mm devel

test: all-devel
	$(MAKE) -j -C test
