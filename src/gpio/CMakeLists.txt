cmake_minimum_required(VERSION 2.8)

file (GLOB CL3_GPIO_SRC
    "*.hpp"
    "*.cpp"
    "*.c"
    "*.h"
)

set (CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS_INT_LIB})
set (CMAKE_C_FLAGS   ${CMAKE_C_FLAGS_INT_LIB})
add_library (cl3-gpio_static STATIC ${CL3_GPIO_SRC})
add_library (cl3-gpio SHARED ${CL3_GPIO_SRC})
add_dependencies (cl3-gpio_static cl3-core_static)
add_dependencies (cl3-gpio cl3-core)

add_subdirectory (test)
