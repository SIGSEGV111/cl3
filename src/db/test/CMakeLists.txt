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
	add_custom_target (cl3-db-test-run-native ALL "${GEN_DIR}\bin\cl3-db-test.exe")
else ()
	add_custom_target (cl3-db-test-run-native   ALL LD_LIBRARY_PATH="${GEN_DIR}/lib" "${GEN_DIR}/bin/cl3-db-test")
	add_custom_target (cl3-db-test-run-valgrind ALL LD_LIBRARY_PATH="${GEN_DIR}/lib" ${VALGRIND_CMD} "${GEN_DIR}/bin/cl3-db-test" || (cat valgrind.log && false))
	add_dependencies (cl3-db-test-run-valgrind cl3-db-test)
endif ()

add_dependencies (cl3-db-test-run-native cl3-db-test)
