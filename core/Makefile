.PHONY: all clean pch

CXX=$(shell which clang)
CXX_OPT_INT=-fPIC -std=c++11 -g -O0 -Wall -Wextra -Werror -Winvalid-pch -I.tmp -DINSIDE_CL3 -Wno-unused-parameter
CXX_OPT_EXT=-fPIC -std=c++11 -g -O0 -Wall -Wextra -Werror -Winvalid-pch -I.tmp -Wno-unused-parameter
OBJS=$(shell ls -1 *.cpp | sed 's/\(.*\)\.cpp/\.tmp\/\1\.o/' -)
PCHS=$(shell ls -1 *.hpp | sed 's/\.hpp/\.hpp\.gch/' -)
DHHS=$(shell ls -1 *.hpp | sed 's/\(.*\)\.hpp/\.tmp\/\1\.dhh/' -)

all: libcl3-core.so libcl3-core.a $(PCHS)

clean:
	rm -vf libcl3-core.so .tmp/* *.hpp.gch

pch: $(PCHS)

# pull in dependencies
-include $(OBJS:.o=.dcc)
-include $(DHHS)

.tmp/%.dcc: %.cpp
	$(CXX) $(CXX_OPT_INT) -MM $< | sed 's/\(.*\):/\.tmp\/\1:/' - > $@

.tmp/%.o: %.cpp .tmp/%.dcc
	$(CXX) $(CXX_OPT_INT) -c $< -o $@

.tmp/%.dhh: %.hpp
	$(CXX) $(CXX_OPT_INT) -MM $< | sed 's/\.o:/\.hpp\.gch:/' - >  $@

%.hpp.gch: %.hpp
	$(CXX) $(CXX_OPT_EXT) $< -o $@

libcl3-core.so: $(OBJS)
	rm -f libcl3-core.so
	$(CXX) -lstdc++ -lrt -shared $(CXX_OPT_INT) $(OBJS) -o $@

libcl3-core.a: $(OBJS)
	ar rcs $@ $(OBJS)
