.PHONY: all clean run

GTEST_DIR=$(CURDIR)/../gtest
CXX=$(shell which clang)
CXX_OPT=-fPIC -std=c++11 -g -O1 -Wall -Wextra -Werror -Winvalid-pch -I../include -fvisibility=hidden
OBJS=$(shell ls -1 *.cpp | sed 's/\.cpp/\.o/' -)

all: run

clean:
	rm -vf exe *.o *.dhh *.dcc *.hpp.gch

# pull in dependencies
-include $(OBJS:.o=.dcc)

%.dcc: %.cpp
	$(CXX) $(CXX_OPT) -MM $< > $@

%.o: %.cpp
	$(CXX) $(CXX_OPT) -c $< -o $@

exe: $(OBJS)
	$(MAKE) libcl3-mm.so -C ../../mm
	$(CXX) -L../lib $(CXX_OPT) -rdynamic $(OBJS) -lcl3-core -lcl3-mm -lstdc++ -lrt -pthread -lgtest ../lib/libgtest_main.a -o $@

run: exe
	LD_LIBRARY_PATH=$(CURDIR)/../lib valgrind -q --leak-check=summary --show-reachable=yes --track-origins=yes ./exe
