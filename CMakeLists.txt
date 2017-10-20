cmake_minimum_required(VERSION 2.8)

project (CL3)

option (BUILD_TYPE "Sets the Build-Type \"release\" or \"devel\"")

set (ROOT_DIR $ENV{ROOT_DIR})

if (${BUILD_TYPE} STREQUAL "release")
	set (GEN_DIR "${PROJECT_SOURCE_DIR}/gen/rel")
else ()
	set (GEN_DIR "${PROJECT_SOURCE_DIR}/gen/dbg")
endif ()

set (VALGRIND_CMD valgrind --quiet --leak-check=full --show-reachable=no --track-origins=yes --num-callers=30 --trace-children=no --track-fds=yes --error-exitcode=1 "--suppressions=${ROOT_DIR}/valgrind.supp" --soname-synonyms=somalloc=NONE --gen-suppressions=all --log-file=valgrind.log)

# -Wno-multichar workaround for GCC bug #53431 @ https://gcc.gnu.org/bugzilla/show_bug.cgi?id=53431

set (CXX_OPT_COMMON_LIB "-Wno-unused-command-line-argument -Wno-unused-parameter -Wno-deprecated-declarations -fvisibility-inlines-hidden -std=c++11 -fvisibility=hidden -Wno-unused-function -fPIC -I${GEN_DIR}/include -L${GEN_DIR}/lib -Wno-multichar")

set (CXX_OPT_COMMON_EXE "-Wno-unused-command-line-argument -Wno-unused-parameter -Wno-deprecated-declarations -fvisibility-inlines-hidden -std=c++11 -fvisibility=hidden -Wno-unused-function -fPIE -I${GEN_DIR}/include -L${GEN_DIR}/lib -Wno-multichar")

set (C_OPT_COMMON_LIB   "-fvisibility=hidden -fPIC -I${GEN_DIR}/include")
set (C_OPT_COMMON_EXE   "-fvisibility=hidden -fPIE -I${GEN_DIR}/include")

if (${BUILD_TYPE} STREQUAL "release")
	set (CMAKE_CXX_FLAGS_INT_LIB "-g -O3 -DCL3_RELEASE ${CXX_OPT_COMMON_LIB} -DINSIDE_CL3")
	set (CMAKE_CXX_FLAGS_EXT_LIB "-g -O3 -DCL3_RELEASE ${CXX_OPT_COMMON_LIB}")
	set (CMAKE_CXX_FLAGS_EXE     "-g -O3 -DCL3_RELEASE ${CXX_OPT_COMMON_EXE}")
	set (CMAKE_C_FLAGS_INT_LIB   "-g -O3 -DCL3_RELEASE ${C_OPT_COMMON_LIB} -DINSIDE_CL3")
	set (CMAKE_C_FLAGS_EXT_LIB   "-g -O3 -DCL3_RELEASE ${C_OPT_COMMON_LIB}")
	set (CMAKE_C_FLAGS_EXE       "-g -O3 -DCL3_RELEASE ${C_OPT_COMMON_EXE}")
else ()
	set (CMAKE_CXX_FLAGS_INT_LIB "-g -O0 -DCL3_DEBUG -Wall -Wextra -Werror --coverage ${CXX_OPT_COMMON_LIB} -DINSIDE_CL3")
	set (CMAKE_CXX_FLAGS_EXT_LIB "-g -O0 -DCL3_DEBUG -Wall -Wextra -Werror --coverage ${CXX_OPT_COMMON_LIB}")
	set (CMAKE_CXX_FLAGS_EXE     "-g -O0 -DCL3_DEBUG -Wall -Wextra -Werror --coverage ${CXX_OPT_COMMON_EXE}")
	set (CMAKE_C_FLAGS_INT_LIB   "-g -O0 -DCL3_DEBUG -Wall -Wextra -Werror --coverage ${C_OPT_COMMON_LIB} -DINSIDE_CL3")
	set (CMAKE_C_FLAGS_EXT_LIB   "-g -O0 -DCL3_DEBUG -Wall -Wextra -Werror --coverage ${C_OPT_COMMON_LIB}")
	set (CMAKE_C_FLAGS_EXE       "-g -O0 -DCL3_DEBUG -Wall -Wextra -Werror --coverage ${C_OPT_COMMON_EXE}")
endif ()

include_directories (${GEN_DIR}/include)
link_directories (${GEN_DIR}/lib)
set(OBJECT_DIR "${GEN_DIR}/${PROJECT_NAME}")

add_subdirectory (src)
