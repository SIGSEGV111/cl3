cmake_minimum_required(VERSION 2.8)

project (CL3)

option (BUILD_TYPE "Sets the Build-Type \"release\" or \"devel\"")

set (ROOT_DIR $ENV{ROOT_DIR})

if (${BUILD_TYPE} STREQUAL "release")
	set (GEN_DIR "${PROJECT_SOURCE_DIR}/gen/rel")
else ()
	set (GEN_DIR "${PROJECT_SOURCE_DIR}/gen/dbg")
endif ()

set (CXX_OPT_COMMON_LIB "-fvisibility-inlines-hidden -std=c++11 -fvisibility=hidden -fPIC -I${GEN_DIR}/include -L${GEN_DIR}/lib")
set (CXX_OPT_COMMON_EXE "-fvisibility-inlines-hidden -std=c++11 -fvisibility=hidden -fPIE -I${GEN_DIR}/include -L${GEN_DIR}/lib")

if (${BUILD_TYPE} STREQUAL "release")
	set (CMAKE_CXX_FLAGS_INT_LIB "${CXX_OPT_COMMON_LIB} -march=native -O2 -DINSIDE_CL3")
	set (CMAKE_CXX_FLAGS_EXT_LIB "${CXX_OPT_COMMON_LIB} -march=native -O2")
	set (CMAKE_CXX_FLAGS_EXE     "${CXX_OPT_COMMON_EXE} -march=native -O2")
else ()
	set (CMAKE_CXX_FLAGS_INT_LIB "${CXX_OPT_COMMON_LIB} -g -O0 -Wall -Wextra -Werror -Wno-unused-parameter --coverage -DINSIDE_CL3")
	set (CMAKE_CXX_FLAGS_EXT_LIB "${CXX_OPT_COMMON_LIB} -g -O0 -Wall -Wextra -Werror -Wno-unused-parameter --coverage")
	set (CMAKE_CXX_FLAGS_EXE     "${CXX_OPT_COMMON_EXE} -g -O0 -Wall -Wextra -Werror -Wno-unused-parameter --coverage")
endif ()

include_directories (${GEN_DIR}/include)
link_directories (${GEN_DIR}/lib)
set(OBJECT_DIR "${GEN_DIR}/${PROJECT_NAME}")

add_subdirectory (src)
