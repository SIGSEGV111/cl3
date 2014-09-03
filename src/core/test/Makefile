.PHONY: all clean run

GTEST_DIR=$(CURDIR)/../gtest
CXX?=$(shell which clang)
CXX_OPT=-std=c++11 -g -O0 -Wall -Wextra -Werror -Winvalid-pch -I../include -Wno-unused-parameter -Wno-attributes -fvisibility-inlines-hidden -fPIC $(CXX_USR_OPT_EXE)
OBJS=$(shell ls -1 *.cpp | sed 's/\.cpp/\.o/' -)

all: run

clean:
	rm -f exe *.o *.dhh *.dcc *.hpp.gch valgrind.log *.tmp

# pull in dependencies
-include $(OBJS:.o=.dcc)

%.dcc: %.cpp
	$(CXX) $(CXX_OPT) -MM $< > $@

%.o: %.cpp
	$(CXX) $(CXX_OPT) -c $< -o $@

exe: $(OBJS)
	+$(MAKE) -j libcl3-core.so -C ../../core
	$(CXX) -L../lib $(CXX_OPT) -rdynamic $(OBJS) -lcl3-core -lstdc++ -lrt -pthread -lgtest -lgmock ../lib/libgtest_main.a ../lib/libgmock_main.a -o $@

gtest: exe
	LD_LIBRARY_PATH=$(CURDIR)/../lib valgrind -q --leak-check=full --show-reachable=yes --track-origins=yes --num-callers=30 --trace-children=no --vgdb=yes --vgdb-error=1 --track-fds=yes --log-file=./valgrind.log --error-exitcode=1 ./exe dummy1 dummy2 dummy3 || (tail -n60 ./valgrind.log && false)

cppcheck:
	../cppcheck/cppcheck --enable=all --force -I /usr/include -q ../../core/{*.cpp,*.hpp}

run: gtest cppcheck
