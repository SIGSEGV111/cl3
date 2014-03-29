.PHONY: test-core test-mm prereqs

all: prereqs
	$(MAKE) run -C test-core
	$(MAKE) run -C test-mm

prereqs: gtest/lib/.libs/libgtest_main.a gmock/lib/.libs/libgmock_main.a

gtest/lib/.libs/libgtest_main.a:
	cd gtest && ./configure && make

gmock/lib/.libs/libgmock_main.a: gtest/lib/.libs/libgtest_main.a
	cd gmock && ./configure --with-gtest=../gtest && make

test-core:
	$(MAKE) run -C test-core

test-mm:
	$(MAKE) run -C test-mm

clean:
	$(MAKE) clean -C test-core
	$(MAKE) clean -C test-mm
