.PHONY: clean core mm test

all: core mm

clean:
	$(MAKE) clean -C core
	$(MAKE) clean -C mm
	$(MAKE) clean -C test

core:
	$(MAKE) -C core

mm:
	$(MAKE) -C mm

test:
	$(MAKE) -C test
