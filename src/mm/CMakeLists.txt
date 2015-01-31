cmake_minimum_required(VERSION 2.8)

file (GLOB CL3_MM_SRC
    "*.hpp"
    "*.cpp"
)

set (CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS_INT_LIB})
add_library (cl3-mm_static STATIC ${CL3_MM_SRC})
add_library (cl3-mm SHARED ${CL3_MM_SRC})
add_dependencies (cl3-mm_static cl3-core_static)
add_dependencies (cl3-mm cl3-core)

add_subdirectory (test)
