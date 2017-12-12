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
	add_custom_target (cl3-gpio-test-run-native ALL "${GEN_DIR}\bin\cl3-gpio-test.exe")
else ()
	add_custom_target (cl3-gpio-test-run-native   ALL LD_LIBRARY_PATH="${GEN_DIR}/lib" "${GEN_DIR}/bin/cl3-gpio-test" --gtest_output=xml:gtest.native.xml)
	add_custom_target (cl3-gpio-test-run-valgrind ALL LD_LIBRARY_PATH="${GEN_DIR}/lib" ${VALGRIND_CMD} "${GEN_DIR}/bin/cl3-gpio-test" --gtest_output=xml:gtest.valgrind.xml || (cat valgrind.log && false))
	add_dependencies (cl3-gpio-test-run-valgrind cl3-gpio-test)
endif ()

add_dependencies (cl3-gpio-test-run-native cl3-gpio-test)
