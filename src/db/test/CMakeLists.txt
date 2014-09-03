cmake_minimum_required(VERSION 3.0)

file (GLOB CL3_CORE_SRC
    "*.h"
    "*.cpp"
)

set (CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS_EXE})
add_executable (cl3-db-test ${CL3_CORE_SRC})
add_dependencies (cl3-db-test cl3-db)
target_link_libraries (cl3-db-test gtest)
target_link_libraries (cl3-db-test gtest_main)
target_link_libraries (cl3-db-test cl3-core)
target_link_libraries (cl3-db-test cl3-db)

if (WIN32)
	add_custom_target (cl3-db-test-run ALL "${GEN_DIR}\bin\cl3-db-test.exe")
else ()
	if (${BUILD_TYPE} STREQUAL "release")
		add_custom_target (cl3-db-test-run ALL "${GEN_DIR}/bin/cl3-db-test")
	else ()
		add_custom_target (cl3-db-test-run ALL LD_LIBRARY_PATH=${GEN_DIR}/lib valgrind -q --leak-check=full --show-reachable=yes --track-origins=yes --num-callers=30 --trace-children=no --vgdb=yes --vgdb-error=1 --track-fds=yes --log-file=./valgrind.log --error-exitcode=1 "${GEN_DIR}/bin/cl3-db-test" || (tail -n60 ./valgrind.log && false))
	endif ()
endif ()

add_dependencies (cl3-db-test-run cl3-db-test)
