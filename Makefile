.PHONY: clean core mm test

all: core mm

clean:
	$(MAKE) clean -C core -j
	$(MAKE) clean -C mm -j
	$(MAKE) clean -C test -j

core:
	$(MAKE) -C core -j

mm: core
	$(MAKE) -C mm -j

test: core mm
	$(MAKE) -C test -j
