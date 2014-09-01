.PHONY: clean core mm test

all: all-release

all-release: core-release mm-release db-release
all-devel: core-devel mm-devel db-devel

clean:
	$(MAKE) -j $(PJOBS) -C core clean
	$(MAKE) -j $(PJOBS) -C mm clean
	$(MAKE) -j $(PJOBS) -C db clean
	$(MAKE) -j $(PJOBS) -C test clean

core-release:
	$(MAKE) -j $(PJOBS) -C core release

core-devel:
	$(MAKE) -j $(PJOBS) -C core devel


mm-release: core-release
	$(MAKE) -j $(PJOBS) -C mm release
	
mm-devel: core-devel
	$(MAKE) -j $(PJOBS) -C mm devel

db-release: core-release
	$(MAKE) -j $(PJOBS) -C db release
	
db-devel: core-devel
	$(MAKE) -j $(PJOBS) -C db devel

test: all-devel
	$(MAKE) -j $(PJOBS) -C test
