cmake_minimum_required(VERSION 2.8)

file (GLOB CL3_CORE_SRC
    "*.hpp"
    "*.cpp"
)

set (CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS_EXE})
add_executable (cl3-mm-test ${CL3_CORE_SRC})
add_dependencies (cl3-mm-test cl3-mm)
target_link_libraries (cl3-mm-test gtest)
target_link_libraries (cl3-mm-test gtest_main)
target_link_libraries (cl3-mm-test cl3-core)
target_link_libraries (cl3-mm-test cl3-mm)

if (WIN32)
	add_custom_target (cl3-mm-test-run ALL "${GEN_DIR}\bin\cl3-mm-test.exe")
else ()
	if (${BUILD_TYPE} STREQUAL "release")
		add_custom_target (cl3-mm-test-run ALL "${GEN_DIR}/bin/cl3-mm-test")
	else ()
		add_custom_target (cl3-mm-coverage-prepare ALL lcov --no-external --base-directory "${ROOT_DIR}/src" --directory "${ROOT_DIR}/tmp/dbg/src/mm/CMakeFiles/cl3-mm.dir" --quiet --zerocounters && lcov --no-external --base-directory "${ROOT_DIR}/src" --directory "${ROOT_DIR}/tmp/dbg/src/mm/CMakeFiles/cl3-mm.dir" --quiet --initial --capture --output-file base.lcov)

		add_custom_target (cl3-mm-test-run ALL LD_LIBRARY_PATH=${GEN_DIR}/lib valgrind --quiet --leak-check=full --show-reachable=yes --track-origins=yes --num-callers=30 --trace-children=no --track-fds=yes --error-exitcode=1 --log-file=valgrind.log "${GEN_DIR}/bin/cl3-mm-test" || (cat valgrind.log; false))

		add_custom_target (cl3-mm-coverage ALL lcov --no-external --base-directory "${ROOT_DIR}/src" --directory "${ROOT_DIR}/tmp/dbg/src/mm/CMakeFiles/cl3-mm.dir" --quiet --capture --output-file test.lcov && lcov --quiet --add-tracefile base.lcov --add-tracefile test.lcov --output-file cl3-mm.lcov && genhtml --prefix "${ROOT_DIR}/src" --quiet --output-directory coverage cl3-mm.lcov)

		add_dependencies (cl3-mm-coverage-prepare cl3-mm)
		add_dependencies (cl3-mm-test-run cl3-mm-coverage-prepare)
		add_dependencies (cl3-mm-coverage cl3-mm-test-run)
	endif ()
endif ()

add_dependencies (cl3-mm-test-run cl3-mm-test)
