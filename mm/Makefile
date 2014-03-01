.PHONY: all clean pch test

CXX=$(shell which clang)
CXX_OPT_INT=-fPIC -g -O3 -Wall -Wextra -Werror -Winvalid-pch -I.tmp -Iinclude -DINSIDE_CL3
CXX_OPT_EXT=-fPIC -g -O3 -Wall -Wextra -Werror -Winvalid-pch -I.tmp -Iinclude
OBJS=$(shell ls -1 *.cpp | sed 's/\(.*\)\.cpp/\.tmp\/\1\.o/' -)
PCHS=$(shell ls -1 *.hpp | sed 's/\.hpp/\.hpp\.gch/' -)
DHHS=$(shell ls -1 *.hpp | sed 's/\(.*\)\.hpp/\.tmp\/\1\.dhh/' -)

all: libcl3-mm.so libcl3-mm.a $(PCHS)

clean:
	rm -vf libcl3-mm.so .tmp/* *.hpp.gch

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

libcl3-mm.so: $(OBJS)
	$(CXX) -Llib -lcl3-core -lstdc++ -lrt -shared $(CXX_OPT_INT) $(OBJS) -o $@

libcl3-mm.a: $(OBJS)
	ar rcs $@ $(OBJS)

test:
	$(MAKE) test-mm -C ../test