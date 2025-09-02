cmake_minimum_required(VERSION 3.10)

project(tfhe_more_operations VERSION 1.0)
set(PROJECT_DIR ~/Documents/CEA/Projects/SAFETravellers/Codes)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CLANG_FLAGS "-stdlib=libc++")
set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -std=c99")
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)


set(TFHE_INCLUDE_DIR ${PROJECT_DIR}/tfhe-4096/src/include)
set(TFHE_LIBRARY_DIR ${PROJECT_DIR}/tfhe-4096/build/libtfhe)

set(TFHE_SCALE_INCLUDE_DIR ${PROJECT_DIR}/tfhe-operators-master/new_tfhe/include)
set(TFHE_SCALE_LIBRARY_DIR ${PROJECT_DIR}/tfhe-operators-master/new_tfhe/build/lib)

include_directories(${TFHE_INCLUDE_DIR})
include_directories(${TFHE_SCALE_INCLUDE_DIR})

# Link directories
link_directories(${TFHE_LIBRARY_DIR})
link_directories(${TFHE_SCALE_LIBRARY_DIR})


# Add source files
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)

file(GLOB SOURCE_FILES ${SRC_DIR}/*.cpp)
# Include directories

# Create the library
add_library(moretfheoperations SHARED ${SOURCE_FILES})
# Specify the include directory
target_include_directories(moretfheoperations PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)
target_include_directories(moretfheoperations PUBLIC
    ${TFHE_SCALE_INCLUDE_DIR}
)

# Link the TFHE library
target_link_libraries(moretfheoperations ${TFHE_LIBRARY_DIR}/libtfhe-spqlios-avx.so ${TFHE_SCALE_LIBRARY_DIR}/libnewtfhe.so)


# Specify the include directory
target_include_directories(moretfheoperations PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

# Set the output location for the shared object file
set_target_properties(moretfheoperations PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)