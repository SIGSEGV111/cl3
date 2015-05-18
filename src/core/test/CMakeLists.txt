cmake_minimum_required(VERSION 2.8)

file (GLOB CL3_CORE_SRC
    "*.hpp"
    "*.cpp"
)

# set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_EXE} -lLLVMAsmParser -lLLVMAsmPrinter -lLLVMBitReader -lLLVMBitWriter -lLLVMCodeGen -lLLVMCore -lLLVMDebugInfo -lLLVMExecutionEngine -lLLVMInstCombine -lLLVMInstrumentation -lLLVMInterpreter -lLLVMipa -lLLVMipo -lLLVMIRReader -lLLVMJIT -lLLVMLinker -lLLVMAnalysis -lLLVMMCDisassembler -lLLVMMCJIT -lLLVMMCParser -lLLVMMC -lLLVMObjCARCOpts -lLLVMObject -lLLVMOption -lLLVMR600AsmPrinter -lLLVMR600CodeGen -lLLVMR600Desc -lLLVMR600Info -lLLVMRuntimeDyld -lLLVMScalarOpts -lLLVMSelectionDAG -lLLVMSupport -lLLVMTableGen -lLLVMTarget -lLLVMTransformUtils -lLLVMVectorize -lLLVMX86AsmParser -lLLVMX86AsmPrinter -lLLVMX86CodeGen -lLLVMX86Desc -lLLVMX86Disassembler -lLLVMX86Info -lLLVMX86Utils")

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_EXE} -Wno-unused-function")

add_executable (cl3-core-test ${CL3_CORE_SRC})
add_dependencies (cl3-core-test cl3-core)
add_dependencies (cl3-core-test cl3-mm)
add_dependencies (cl3-core-test cl3-db)
target_link_libraries (cl3-core-test gtest)
target_link_libraries (cl3-core-test gtest_main)
target_link_libraries (cl3-core-test cl3-core)

if (WIN32)
	add_custom_target (cl3-core-test-run ALL "${GEN_DIR}\bin\cl3-core-test.exe" dummy1 dummy2 dummy3)
else ()
	if (${BUILD_TYPE} STREQUAL "release")
		add_custom_target (cl3-core-test-run ALL "${GEN_DIR}/bin/cl3-core-test" dummy1 dummy2 dummy3)
	else ()
		add_custom_target (cl3-core-coverage-prepare ALL lcov --no-external --base-directory "${ROOT_DIR}/src" --directory "${ROOT_DIR}/tmp/dbg/src/core/CMakeFiles/cl3-core.dir" --quiet --zerocounters && lcov --no-external --base-directory "${ROOT_DIR}/src" --directory "${ROOT_DIR}/tmp/dbg/src/core/CMakeFiles/cl3-core.dir" --quiet --initial --capture --output-file base.lcov)

		add_custom_target (cl3-core-test-run ALL LD_LIBRARY_PATH="${GEN_DIR}/lib" valgrind --quiet --leak-check=full --show-reachable=no --track-origins=yes --num-callers=30 --trace-children=no --track-fds=yes --error-exitcode=1 --log-file=valgrind.log "${GEN_DIR}/bin/cl3-core-test" dummy1 dummy2 dummy3 || (cat valgrind.log && false))

		add_custom_target (cl3-core-coverage ALL lcov --no-external --base-directory "${ROOT_DIR}/src" --directory "${ROOT_DIR}/tmp/dbg/src/core/CMakeFiles/cl3-core.dir" --quiet --capture --output-file test.lcov && lcov --quiet --add-tracefile base.lcov --add-tracefile test.lcov --output-file cl3-core.lcov && genhtml --prefix "${ROOT_DIR}/src" --quiet --output-directory coverage cl3-core.lcov)

		add_dependencies (cl3-core-coverage-prepare cl3-core)
		add_dependencies (cl3-core-test-run cl3-core-coverage-prepare)
		add_dependencies (cl3-core-coverage cl3-core-test-run)
	endif ()
endif ()

add_dependencies (cl3-core-test-run cl3-core-test)
