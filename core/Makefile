.PHONY: all clean pch

CXX=$(shell which g++)
CXX_OPT_INT=-fPIC -std=c++11 -g -O0 -Wall -Wextra -Werror -DINSIDE_CL3 -Wno-unused-parameter
CXX_OPT_EXT=-fPIC -std=c++11 -g -O0 -Wall -Wextra -Werror -Wno-unused-parameter
OBJS=$(shell ls -1 *.cpp | sed 's/\.cpp/\.o/' -)
PCHS=$(shell ls -1 *.hpp | sed 's/\.hpp/\.hpp\.gch/' -)
DHHS=$(shell ls -1 *.hpp | sed 's/\.hpp/\.dhh/' -)

all: libcl3-core.so libcl3-core.a $(PCHS)

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

libcl3-core.so: $(OBJS)
	$(CXX) -lstdc++ -lrt -shared $(CXX_OPT_INT) $(OBJS) -o $@

libcl3-core.a: $(OBJS)
	ar rcs $@ $(OBJS)
