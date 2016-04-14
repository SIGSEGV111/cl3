cmake_minimum_required(VERSION 2.8)

file (GLOB CL3_CORE_SRC
    "*.hpp"
    "*.cpp"
)

set (CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS_INT_LIB})
add_library (cl3-core_static STATIC ${CL3_CORE_SRC})
add_library (cl3-core SHARED ${CL3_CORE_SRC})

target_link_libraries (cl3-core pthread)
target_link_libraries (cl3-core cl3-extlib)
target_link_libraries (cl3-core dl)
target_link_libraries (cl3-core LLVMX86Disassembler)
target_link_libraries (cl3-core LLVMX86AsmParser)
target_link_libraries (cl3-core LLVMX86CodeGen)
target_link_libraries (cl3-core LLVMSelectionDAG)
target_link_libraries (cl3-core LLVMAsmPrinter)
target_link_libraries (cl3-core LLVMCodeGen)
target_link_libraries (cl3-core LLVMScalarOpts)
target_link_libraries (cl3-core LLVMInstCombine)
target_link_libraries (cl3-core LLVMTransformUtils)
target_link_libraries (cl3-core LLVMipa)
target_link_libraries (cl3-core LLVMAnalysis)
target_link_libraries (cl3-core LLVMMCJIT)
target_link_libraries (cl3-core LLVMTarget)
target_link_libraries (cl3-core LLVMRuntimeDyld)
target_link_libraries (cl3-core LLVMObject)
target_link_libraries (cl3-core LLVMMCParser)
target_link_libraries (cl3-core LLVMBitReader)
target_link_libraries (cl3-core LLVMExecutionEngine)
target_link_libraries (cl3-core LLVMMC)
target_link_libraries (cl3-core LLVMCore)
target_link_libraries (cl3-core LLVMSupport)

target_link_libraries (cl3-core LLVMX86Desc)
target_link_libraries (cl3-core LLVMX86Info)
target_link_libraries (cl3-core LLVMX86AsmPrinter)
target_link_libraries (cl3-core LLVMX86Utils)

add_subdirectory (test)
