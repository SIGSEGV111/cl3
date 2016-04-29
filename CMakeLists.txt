cmake_minimum_required(VERSION 2.8)

project (CL3)

option (BUILD_TYPE "Sets the Build-Type \"release\" or \"devel\"")

set (ROOT_DIR $ENV{ROOT_DIR})

if (${BUILD_TYPE} STREQUAL "release")
	set (GEN_DIR "${PROJECT_SOURCE_DIR}/gen/rel")
else ()
	set (GEN_DIR "${PROJECT_SOURCE_DIR}/gen/dbg")
endif ()

# -Wno-multichar workaround for GCC bug #53431 @ https://gcc.gnu.org/bugzilla/show_bug.cgi?id=53431
set (CXX_OPT_COMMON_LIB "-fvisibility-inlines-hidden -std=c++11 -fvisibility=hidden -fPIC -I${GEN_DIR}/include -L${GEN_DIR}/lib -Wno-multichar")
set (CXX_OPT_COMMON_EXE "-fvisibility-inlines-hidden -std=c++11 -fvisibility=hidden -fPIE -I${GEN_DIR}/include -L${GEN_DIR}/lib -Wno-multichar")
set (C_OPT_COMMON_LIB   "-fvisibility=hidden -fPIC -I${GEN_DIR}/include -L${GEN_DIR}/lib")
set (C_OPT_COMMON_EXE   "-fvisibility=hidden -fPIE -I${GEN_DIR}/include -L${GEN_DIR}/lib")

if (${BUILD_TYPE} STREQUAL "release")
	set (CMAKE_CXX_FLAGS_INT_LIB "${CXX_OPT_COMMON_LIB} -g -march=native -O3 -DCL3_RELEASE -DINSIDE_CL3")
	set (CMAKE_CXX_FLAGS_EXT_LIB "${CXX_OPT_COMMON_LIB} -g -march=native -O3 -DCL3_RELEASE")
	set (CMAKE_CXX_FLAGS_EXE     "${CXX_OPT_COMMON_EXE} -g -march=native -O3 -DCL3_RELEASE")
	set (CMAKE_C_FLAGS_INT_LIB   "${C_OPT_COMMON_LIB}   -g -march=native -O3 -DCL3_RELEASE -DINSIDE_CL3")
	set (CMAKE_C_FLAGS_EXT_LIB   "${C_OPT_COMMON_LIB}   -g -march=native -O3 -DCL3_RELEASE")
	set (CMAKE_C_FLAGS_EXE       "${C_OPT_COMMON_EXE}   -g -march=native -O3 -DCL3_RELEASE")
else ()
	set (CMAKE_CXX_FLAGS_INT_LIB "${CXX_OPT_COMMON_LIB} -g -march=native -O0 -DCL3_DEBUG -Wall -Wextra -Werror -Wno-unused-command-line-argument -Wno-unused-parameter -Wno-unused-function --coverage -DINSIDE_CL3")
	set (CMAKE_CXX_FLAGS_EXT_LIB "${CXX_OPT_COMMON_LIB} -g -march=native -O0 -DCL3_DEBUG -Wall -Wextra -Werror -Wno-unused-command-line-argument -Wno-unused-parameter -Wno-unused-function --coverage")
	set (CMAKE_CXX_FLAGS_EXE     "${CXX_OPT_COMMON_EXE} -g -march=native -O0 -DCL3_DEBUG -Wall -Wextra -Werror -Wno-unused-command-line-argument -Wno-unused-parameter -Wno-unused-function --coverage")
	set (CMAKE_C_FLAGS_INT_LIB   "${C_OPT_COMMON_LIB}   -g -march=native -O0 -DCL3_DEBUG -Wall -Wextra -Werror -Wno-unused-command-line-argument -Wno-unused-parameter -Wno-unused-function --coverage -DINSIDE_CL3")
	set (CMAKE_C_FLAGS_EXT_LIB   "${C_OPT_COMMON_LIB}   -g -march=native -O0 -DCL3_DEBUG -Wall -Wextra -Werror -Wno-unused-command-line-argument -Wno-unused-parameter -Wno-unused-function --coverage")
	set (CMAKE_C_FLAGS_EXE       "${C_OPT_COMMON_EXE}   -g -march=native -O0 -DCL3_DEBUG -Wall -Wextra -Werror -Wno-unused-command-line-argument -Wno-unused-parameter -Wno-unused-function --coverage")
endif ()

include_directories (${GEN_DIR}/include)
link_directories (${GEN_DIR}/lib)
set(OBJECT_DIR "${GEN_DIR}/${PROJECT_NAME}")

add_subdirectory (src)
