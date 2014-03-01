.PHONY: clean core mm test

all: core mm

clean:
	$(MAKE) clean -C core
	$(MAKE) clean -C mm
	$(MAKE) clean -C test

core:
	$(MAKE) -C core

mm: core
	$(MAKE) -C mm

test: core mm
	$(MAKE) -C test
