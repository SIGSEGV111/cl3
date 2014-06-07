.PHONY: core mm prereqs

all: prereqs
	$(MAKE) run -C core -j
	$(MAKE) run -C mm -j

prereqs: gtest/lib/.libs/libgtest_main.a gmock/lib/.libs/libgmock_main.a

gtest/lib/.libs/libgtest_main.a:
	rm -Rf gtest-1.7.0 && unzip gtest-1.7.0.zip && cd gtest && ./configure && make -j4

gmock/lib/.libs/libgmock_main.a: gtest/lib/.libs/libgtest_main.a
	rm -Rf gmock-1.7.0 && unzip gmock-1.7.0.zip && cd gmock && ./configure --with-gtest=../gtest && make -j4

core:
	$(MAKE) run -C core -j

mm:
	$(MAKE) run -C mm -j

clean:
	$(MAKE) clean -C core -j
	$(MAKE) clean -C mm -j
