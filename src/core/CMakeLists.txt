cmake_minimum_required(VERSION 2.8)

file (GLOB CL3_CORE_SRC
    "*.h"
    "*.cpp"
)

set (CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS_INT_LIB})
add_library (cl3-core_static STATIC ${CL3_CORE_SRC})
add_library (cl3-core SHARED ${CL3_CORE_SRC})

target_link_libraries (cl3-core pthread)

add_subdirectory (test)
