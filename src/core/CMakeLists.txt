cmake_minimum_required(VERSION 2.8)

file (GLOB CL3_CORE_SRC
    "*.hpp"
    "*.cpp"
)

set (CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS_INT_LIB})
add_library (cl3-core_static STATIC ${CL3_CORE_SRC})
add_library (cl3-core SHARED ${CL3_CORE_SRC})

target_link_libraries (cl3-core pthread)
target_link_libraries (cl3-core cl3-extlib)
target_link_libraries (cl3-core dl)

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})
llvm_map_components_to_libnames(llvm_libs native LLVM${LLVM_NATIVE_ARCH}CodeGen)
message(STATUS "LLVM libs: ${llvm_libs}")
target_link_libraries(cl3-core ${llvm_libs} LLVMSupport LLVMCodeGen LLVMExecutionEngine LLVMMCJIT)
# target_link_libraries(cl3-core LLVM)

add_subdirectory (test)
