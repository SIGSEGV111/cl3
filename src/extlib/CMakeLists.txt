cmake_minimum_required(VERSION 2.8)

file (GLOB CL3_EXTLIB_SRC
    "*.hpp"
    "*.cpp"
    "*.c"
)

set (CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS_INT_LIB})
add_library (cl3-extlib_static STATIC ${CL3_EXTLIB_SRC})
add_library (cl3-extlib SHARED ${CL3_EXTLIB_SRC})
