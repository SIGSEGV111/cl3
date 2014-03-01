.PHONY: all clean run

GTEST_DIR=$(CURDIR)/../gtest
CXX=$(shell which clang)
CXX_OPT=-fPIC -g -O0 -Wall -Wextra -Werror -Winvalid-pch -I.tmp -I../include
OBJS=$(shell ls -1 *.cpp | sed 's/\(.*\)\.cpp/\.tmp\/\1\.o/' -)

all: run

clean:
	rm -vf exe .tmp/*

# pull in dependencies
-include $(OBJS:.o=.dcc)

.tmp/%.dcc: %.cpp
	$(CXX) $(CXX_OPT) -MM $< > $@

.tmp/%.o: %.cpp $(PCHS)
	$(CXX) $(CXX_OPT) -c $< -o $@

exe: $(OBJS)
	$(MAKE) libcl3-core.so -C ../../core
	$(CXX) -L../lib -lstdc++ -lrt -pthread $(CXX_OPT) $(OBJS) -lcl3-core -lgtest ../lib/libgtest_main.a -o $@

run: exe
	LD_LIBRARY_PATH=$(CURDIR)/../lib ./exe