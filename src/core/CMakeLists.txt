cmake_minimum_required(VERSION 2.8)

file (GLOB CL3_CORE_SRC
    "*.hpp"
    "*.cpp"
)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_INT_LIB} $ENV{CL3_CORE_LLVM_LIBS}")
add_library (cl3-core_static STATIC ${CL3_CORE_SRC})
add_library (cl3-core SHARED ${CL3_CORE_SRC})

target_link_libraries (cl3-core pthread)
target_link_libraries (cl3-core cl3-extlib_static)
target_link_libraries (cl3-core dl)

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
message(STATUS "LLVM Libs: " $ENV{CL3_CORE_LLVM_LIBS})

# llvm-config --libs codegen native core engine support

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})


add_subdirectory (test)
