.PHONY: all clean pch libcl3-core

GTEST_DIR=$(CURDIR)/gtest
CXX=$(shell which clang)
CXX_OPT=-fPIC -g -O0 -Wall -Wextra -Werror -Winvalid-pch -I.tmp -Iinclude
OBJS=$(shell ls -1 *.cpp | sed 's/\(.*\)\.cpp/\.tmp\/\1\.o/' -)
PCHS=$(shell ls -1 *.hpp | sed 's/\(.*\)\.hpp/\.tmp\/\1\.hpp\.gch/' -)

all: run

clean:
	rm -vf test-core .tmp/*

pch: $(PCHS)

# pull in dependencies
-include $(OBJS:.o=.dcc)
-include $(PCHS:.hpp.gch=.dhh)

.tmp/%.dcc: %.cpp
	$(CXX) $(CXX_OPT) -MM $< > $@

.tmp/%.o: %.cpp $(PCHS)
	$(CXX) $(CXX_OPT) -c $< -o $@

.tmp/%.dhh: %.hpp
	$(CXX) $(CXX_OPT) -MM $< | sed 's/\(.*\)\.o:/\.tmp\/\1\.hpp\.gch:/' - > $@

.tmp/%.hpp.gch: %.hpp
	$(CXX) $(CXX_OPT) $< -o $@

lib/libcl3-core.so: $(shell ls -1 ../core/*.cpp ../core/*.hpp)
	$(MAKE) -C ../core

test-core: $(OBJS) $(PCHS) lib/libcl3-core.so
	$(CXX) -Llib -lstdc++ -lrt -pthread $(CXX_OPT) $(OBJS) -lcl3-core -lgtest lib/libgtest_main.a -o $@

run: test-core
	LD_LIBRARY_PATH=$(CURDIR)/lib ./test-core