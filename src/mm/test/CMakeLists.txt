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
	add_custom_target (cl3-mm-test-run-native ALL "${GEN_DIR}\bin\cl3-mm-test.exe")
else ()
	add_custom_target (cl3-mm-test-run-native   ALL LD_LIBRARY_PATH="${GEN_DIR}/lib" "${GEN_DIR}/bin/cl3-mm-test")
	add_custom_target (cl3-mm-test-run-valgrind ALL LD_LIBRARY_PATH="${GEN_DIR}/lib" ${VALGRIND_CMD} "${GEN_DIR}/bin/cl3-mm-test" || (cat valgrind.log && false))
	add_dependencies (cl3-mm-test-run-valgrind cl3-mm-test)
endif ()

add_dependencies (cl3-mm-test-run-native cl3-mm-test)
