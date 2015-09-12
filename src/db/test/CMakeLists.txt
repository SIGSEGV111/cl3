cmake_minimum_required(VERSION 2.8)

file (GLOB CL3_CORE_SRC
    "*.hpp"
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
		#add_custom_target (cl3-db-coverage-prepare ALL lcov --no-external --base-directory "${ROOT_DIR}/src" --directory "${ROOT_DIR}/tmp/dbg/src/db/CMakeFiles/cl3-db.dir" --quiet --zerocounters && lcov --no-external --base-directory "${ROOT_DIR}/src" --directory "${ROOT_DIR}/tmp/dbg/src/db/CMakeFiles/cl3-db.dir" --quiet --initial --capture --output-file base.lcov)

		add_custom_target (cl3-db-test-run ALL LD_LIBRARY_PATH=${GEN_DIR}/lib valgrind --quiet --leak-check=full --show-reachable=no --track-origins=yes --num-callers=30 --trace-children=no --track-fds=yes --error-exitcode=1 --log-file=valgrind.log "${GEN_DIR}/bin/cl3-db-test" || (cat valgrind.log && false))

		#add_custom_target (cl3-db-coverage ALL lcov --no-external --base-directory "${ROOT_DIR}/src" --directory "${ROOT_DIR}/tmp/dbg/src/db/CMakeFiles/cl3-db.dir" --quiet --capture --output-file test.lcov && lcov --quiet --add-tracefile base.lcov --add-tracefile test.lcov --output-file cl3-db.lcov && genhtml --prefix "${ROOT_DIR}/src" --quiet --output-directory coverage cl3-db.lcov)

		#add_dependencies (cl3-db-coverage-prepare cl3-db)
		#add_dependencies (cl3-db-test-run cl3-db-coverage-prepare)
		#add_dependencies (cl3-db-coverage cl3-db-test-run)
	endif ()
endif ()

add_dependencies (cl3-db-test-run cl3-db-test)
