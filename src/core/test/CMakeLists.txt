cmake_minimum_required(VERSION 2.8)

file (GLOB CL3_CORE_SRC
    "*.hpp"
    "*.cpp"
)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_EXE} -Wno-unused-function")

add_executable (cl3-core-test ${CL3_CORE_SRC})
add_dependencies (cl3-core-test cl3-core)
target_link_libraries (cl3-core-test gtest)
target_link_libraries (cl3-core-test gtest_main)
target_link_libraries (cl3-core-test cl3-core)

if (WIN32)
	add_custom_target (cl3-core-test-run-native ALL "${GEN_DIR}\bin\cl3-core-test.exe" dummy1 dummy2 dummy3)
else ()
	add_custom_target (cl3-core-test-run-native   ALL LD_LIBRARY_PATH="${GEN_DIR}/lib" "${GEN_DIR}/bin/cl3-core-test" --gtest_output=xml dummy1 dummy2 dummy3)
	add_custom_target (cl3-core-test-run-valgrind ALL LD_LIBRARY_PATH="${GEN_DIR}/lib" ${VALGRIND_CMD} "${GEN_DIR}/bin/cl3-core-test" --gtest_output=xml dummy1 dummy2 dummy3 || (cat valgrind.log && false))
	add_dependencies (cl3-core-test-run-valgrind cl3-core-test)
endif ()

add_dependencies (cl3-core-test-run-native cl3-core-test)

