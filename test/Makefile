.PHONY: core mm prereqs

all: prereqs
	rm -vf ../{core,mm,db}/*.gcda
	$(MAKE) -j $(PJOBS) -C core run
	$(MAKE) -j $(PJOBS) -C mm run
	$(MAKE) -j $(PJOBS) -C db run
	$(MAKE) -j $(PJOBS) -C coverage html-report

prereqs: gtest/lib/.libs/libgtest_main.a gmock/lib/.libs/libgmock_main.a cppcheck/cppcheck

gtest/lib/.libs/libgtest_main.a:
	rm -Rf gtest-1.7.0 && unzip gtest-1.7.0.zip && cd gtest && CXXFLAGS="-march=native -fno-lto" ./configure
	$(MAKE) -j $(PJOBS) -C gtest

gmock/lib/.libs/libgmock_main.a: gtest/lib/.libs/libgtest_main.a
	rm -Rf gmock-1.7.0 && unzip gmock-1.7.0.zip && cd gmock && CXXFLAGS="-march=native -fno-lto" ./configure --with-gtest=../gtest
	$(MAKE) -j $(PJOBS) -C gmock

cppcheck/cppcheck:
	rm -Rf cppcheck-1.65 && tar -xf cppcheck-1.65.tar.bz2
	$(MAKE) -j $(PJOBS) -C cppcheck SRCDIR=build CFGDIR=$(CURDIR)/cppcheck/cfg HAVE_RULES=yes CXX="$(shell which g++)"

core:
	$(MAKE) -j $(PJOBS) -C core run

mm:
	$(MAKE) -j $(PJOBS) -C mm run

db:
	$(MAKE) -j $(PJOBS) -C db run

clean:
	$(MAKE) -j $(PJOBS) -C core clean
	$(MAKE) -j $(PJOBS) -C mm clean
	$(MAKE) -j $(PJOBS) -C db clean
	$(MAKE) -j $(PJOBS) -C coverage clean
