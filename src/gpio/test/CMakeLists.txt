cmake_minimum_required(VERSION 2.8)

file (GLOB CL3_CORE_SRC
    "*.hpp"
    "*.cpp"
)

set (CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS_EXE})
add_executable (cl3-gpio-test ${CL3_CORE_SRC})
add_dependencies (cl3-gpio-test cl3-gpio)
target_link_libraries (cl3-gpio-test gtest)
target_link_libraries (cl3-gpio-test gtest_main)
target_link_libraries (cl3-gpio-test cl3-core)
target_link_libraries (cl3-gpio-test cl3-gpio)

if (WIN32)
	add_custom_target (cl3-gpio-test-run ALL "${GEN_DIR}\bin\cl3-gpio-test.exe")
else ()
	if (${BUILD_TYPE} STREQUAL "release")
		add_custom_target (cl3-gpio-test-run ALL "${GEN_DIR}/bin/cl3-gpio-test")
	else ()
		add_custom_target (cl3-gpio-coverage-prepare ALL lcov --no-external --base-directory "${ROOT_DIR}/src" --directory "${ROOT_DIR}/tmp/dbg/src/gpio/CMakeFiles/cl3-gpio.dir" --quiet --zerocounters && lcov --no-external --base-directory "${ROOT_DIR}/src" --directory "${ROOT_DIR}/tmp/dbg/src/gpio/CMakeFiles/cl3-gpio.dir" --quiet --initial --capture --output-file base.lcov)

		add_custom_target (cl3-gpio-test-run ALL LD_LIBRARY_PATH=${GEN_DIR}/lib valgrind --quiet --leak-check=full --show-reachable=yes --track-origins=yes --num-callers=30 --trace-children=no --track-fds=yes --error-exitcode=1 --log-file=valgrind.log "${GEN_DIR}/bin/cl3-gpio-test" || (cat valgrind.log; false))

		add_custom_target (cl3-gpio-coverage ALL lcov --no-external --base-directory "${ROOT_DIR}/src" --directory "${ROOT_DIR}/tmp/dbg/src/gpio/CMakeFiles/cl3-gpio.dir" --quiet --capture --output-file test.lcov && lcov --quiet --add-tracefile base.lcov --add-tracefile test.lcov --output-file cl3-gpio.lcov && genhtml --prefix "${ROOT_DIR}/src" --quiet --output-directory coverage cl3-gpio.lcov)

		add_dependencies (cl3-gpio-coverage-prepare cl3-gpio)
		add_dependencies (cl3-gpio-test-run cl3-gpio-coverage-prepare)
		add_dependencies (cl3-gpio-coverage cl3-gpio-test-run)
	endif ()
endif ()

add_dependencies (cl3-gpio-test-run cl3-gpio-test)
