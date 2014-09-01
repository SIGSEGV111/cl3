.PHONY: all clean pch test

CXX?=$(shell which clang)
OBJS=$(shell ls -1 *.cpp | sed 's/\.cpp/\.o/' -)
PCHS=$(shell ls -1 *.hpp | sed 's/\.hpp/\.hpp\.gch/' -)
DHHS=$(shell ls -1 *.hpp | sed 's/\.hpp/\.dhh/' -)

CXX_OPT_COMMON=-fvisibility-inlines-hidden -std=c++11 -fvisibility=hidden -fPIC -I../.. $(CXX_USR_OPT_LIB)
CXX_OPT_INT=$(CXX_OPT_COMMON) -march=native -O2 -fPIC -DINSIDE_CL3
CXX_OPT_EXT=$(CXX_OPT_COMMON) -march=native -O2
devel: CXX_OPT_INT=$(CXX_OPT_COMMON) -g -O0 -Wall -Wextra -Werror -Wno-unused-parameter --coverage -DINSIDE_CL3
devel: CXX_OPT_EXT=$(CXX_OPT_COMMON) -g -O0 -Wall -Wextra -Werror -Wno-unused-parameter --coverage

all: release

release devel: libcl3-mm.so libcl3-mm.a $(PCHS)

clean:
	rm -f *.so *.a *.o *.dhh *.dcc *.hpp.gch *.gcda *.gcno

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
	+$(MAKE) -j libcl3-core.so -C ../core
	$(CXX) -Llib -lcl3-core -lstdc++ -lrt -shared -rdynamic $(CXX_OPT_INT) $(OBJS) -o $@

libcl3-mm.a: $(OBJS)
	ar rcs $@ $(OBJS)
