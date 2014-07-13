.PHONY: all clean pch test

CXX?=$(shell which clang)
CXX_OPT_INT=-fPIC -std=c++11 -g -O0 -Wall -Wextra -Werror -fvisibility=hidden -Wno-unused-parameter -DINSIDE_CL3 -I../.. $(CXX_USR_OPT_LIB)
CXX_OPT_EXT=-fPIC -std=c++11 -g -O0 -Wall -Wextra -Werror -fvisibility=hidden -Wno-unused-parameter              -I../.. $(CXX_USR_OPT_LIB)
OBJS=$(shell ls -1 *.cpp | sed 's/\.cpp/\.o/' -)
PCHS=$(shell ls -1 *.hpp | sed 's/\.hpp/\.hpp\.gch/' -)
DHHS=$(shell ls -1 *.hpp | sed 's/\.hpp/\.dhh/' -)

all: libcl3-mm.so libcl3-mm.a $(PCHS)

clean:
	rm -vf *.so *.a *.o *.dhh *.dcc *.hpp.gch

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

libcl3-mm.so: $(OBJS)
	$(MAKE) libcl3-core.so -C ../core
	$(CXX) -Llib -lcl3-core -lstdc++ -lrt -shared -rdynamic $(CXX_OPT_INT) $(OBJS) -o $@

libcl3-mm.a: $(OBJS)
	ar rcs $@ $(OBJS)
