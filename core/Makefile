.PHONY: all clean

export CXX=clang
export CXX_OPT=-fPIC -g -DINSIDE_CL3 -O3

all: libcl3-core.so

clean:
	rm -vf libcl3-core.so *.o

%.o: %.cpp
	$(CXX) -c $(CXX_OPT) $+

libcl3-core.so: error.o error.posix.o error.windows.o io_collection_bitmask.o io_collection_list.o system_compiler.o system_context.o system_memory.o system_types_typeinfo.o system_types_typeinfo.gcc+llvm.o time.o time.posix.o time.windows.o util.o
	$(CXX) -lstdc++ -lrt -shared $(CXX_OPT) $+ -o $@

