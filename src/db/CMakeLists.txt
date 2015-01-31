cmake_minimum_required(VERSION 2.8)

file (GLOB CL3_DB_SRC
    "*.hpp"
    "*.cpp"
)

set (CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS_INT_LIB})
add_library (cl3-db_static STATIC ${CL3_DB_SRC})
add_library (cl3-db SHARED ${CL3_DB_SRC})
add_dependencies (cl3-db_static cl3-core_static)
add_dependencies (cl3-db cl3-core)

#add_subdirectory (test)
