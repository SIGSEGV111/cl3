.PHONY: core mm prereqs

all: prereqs
	rm -vf ../{core,mm}/*.gcda
	$(MAKE) -j -C core run
	$(MAKE) -j -C mm run
	$(MAKE) -j -C coverage html-report

prereqs: gtest/lib/.libs/libgtest_main.a gmock/lib/.libs/libgmock_main.a cppcheck/cppcheck

gtest/lib/.libs/libgtest_main.a:
	rm -Rf gtest-1.7.0 && unzip gtest-1.7.0.zip && cd gtest && ./configure && make -j4

gmock/lib/.libs/libgmock_main.a: gtest/lib/.libs/libgtest_main.a
	rm -Rf gmock-1.7.0 && unzip gmock-1.7.0.zip && cd gmock && ./configure --with-gtest=../gtest && make -j4

cppcheck/cppcheck:
	rm -Rf cppcheck-1.65 && tar -xf cppcheck-1.65.tar.bz2 && $(MAKE) -j2 -C cppcheck SRCDIR=build CFGDIR=$(CURDIR)/cppcheck/cfg HAVE_RULES=yes CXX=g++

core:
	$(MAKE) -j -C core run

mm:
	$(MAKE) -j -C mm run

clean:
	$(MAKE) -j -C core clean
	$(MAKE) -j -C mm clean
	$(MAKE) -j -C coverage clean
