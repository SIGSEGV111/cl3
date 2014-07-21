.PHONY: all clean pch

CXX?=$(shell which clang)
OBJS=$(shell ls -1 *.cpp | sed 's/\.cpp/\.o/' -)
PCHS=$(shell ls -1 *.hpp | sed 's/\.hpp/\.hpp\.gch/' -)
DHHS=$(shell ls -1 *.hpp | sed 's/\.hpp/\.dhh/' -)

CXX_OPT_INT=-fPIC -std=c++11 -O2 -fvisibility=hidden -DINSIDE_CL3 $(CXX_USR_OPT_LIB)
CXX_OPT_EXT=      -std=c++11 -O2 -fvisibility=hidden $(CXX_USR_OPT_LIB)
devel: CXX_OPT_INT=  -fPIC -std=c++11 -g -O0 -Wall -Wextra -Werror -fvisibility=hidden -Wno-unused-parameter -DINSIDE_CL3 --coverage $(CXX_USR_OPT_LIB)
devel: CXX_OPT_EXT=        -std=c++11 -g -O0 -Wall -Wextra -Werror -fvisibility=hidden -Wno-unused-parameter              --coverage $(CXX_USR_OPT_LIB)

all: release

release devel: libcl3-core.so libcl3-core.a $(PCHS)

clean:
	rm -vf *.so *.a *.o *.dhh *.dcc *.hpp.gch *.gcda *.gcno

pch: $(PCHS)

# pull in dependencies
-include $(OBJS:.o=.dcc)
-include $(DHHS)

%.dcc: %.cpp
	$(CXX) $(CXX_OPT_INT) -MM $< > $@

%.o: %.cpp %.dcc
	$(CXX) $(CXX_OPT_INT) -c $< -o $@

%.dhh: %.hpp
	$(CXX) $(CXX_OPT_INT) -MM $< | sed 's/\.o:/\.hpp\.gch:/' - >  $@

%.hpp.gch: %.hpp
	$(CXX) $(CXX_OPT_EXT) $< -o $@

libcl3-core.so: $(OBJS)
	$(CXX) -lstdc++ -lrt -shared -rdynamic $(CXX_OPT_INT) $(OBJS) -o $@

libcl3-core.a: $(OBJS)
	ar rcs $@ $(OBJS)
