cmake_minimum_required(VERSION 3.10)

# Set the project name and version
project(SAFETravellers VERSION 2.0)
set(PROJECT_DIR /home/olivechakraborty/Documents/CEA/Projects/SAFETravellers/Codes/Project25Update)
set(LIBS_DIR ${PROJECT_DIR}/all_libs)
# Specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# set(TFHE_INCLUDE_DIR "/usr/local/include/tfhe")
# set(TFHE_LIBRARY_DIR "/usr/local/lib")


set(TFHE_INCLUDE_DIR ${LIBS_DIR}/tfhe-4096/src/include)
set(TFHE_LIBRARY_DIR ${LIBS_DIR}/tfhe-4096/build/libtfhe)

set(TFHE_OPS_INCLUDE_DIR ${LIBS_DIR}/tfhe-operators-master/more_operations/include)
set(TFHE_OPS_LIBRARY_DIR ${LIBS_DIR}/tfhe-operators-master/more_operations/build/lib)

set(TFHE_NEW_INCLUDE_DIR ${LIBS_DIR}/tfhe-operators-master/new_tfhe/include)
set(TFHE_NEW_LIBRARY_DIR ${LIBS_DIR}/tfhe-operators-master/new_tfhe/build/lib)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Find the SEAL library
#set(CMAKE_PREFIX_PATH "${LIBS_DIR}" ${CMAKE_PREFIX_PATH})
set(CMAKE_PREFIX_PATH "${LIBS_DIR}/SEAL/build/cmake" ${CMAKE_PREFIX_PATH})
find_package(SEAL REQUIRED)

#Find the OpenSSL library
set(OPENSSL_ROOT_DIR "${LIBS_DIR}/openssl-3.0.14/install")
set(OPENSSL_INCLUDE_DIR "${LIBS_DIR}/openssl-3.0.14/install/include")
set(OPENSSL_LIBRARIES "${LIBS_DIR}/openssl-3.0.14/install/lib64")
#find_package(OpenSSL REQUIRED)


#Addn executabkes
add_executable(HESysInit src/HESysInit.cpp src/params.cpp src/keysHEComp.cpp)
add_executable(Register src/Register.cpp src/keysUser.cpp src/utils.cpp src/dataIO.cpp src/token.cpp )
add_executable(Verify src/Verify.cpp src/keysEGate.cpp src/token.cpp src/params.cpp src/dataIO.cpp src/utils.cpp)

# Include directories for SEAL and TFHE
#include_directories(${SEAL_INCLUDE_DIR})
include_directories(${TFHE_INCLUDE_DIR})
include_directories(${TFHE_OPS_INCLUDE_DIR})
include_directories(${TFHE_NEW_INCLUDE_DIR})
include_directories(${OPENSSL_INCLUDE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/include)
#link_directories(${TFHE_OPS_LIBRARY_DIR})
#link_directories(${TFHE_SCALE_LIBRARY_DIR})


target_link_libraries(HESysInit SEAL::seal ${TFHE_LIBRARY_DIR}/libtfhe-spqlios-avx.so  ${TFHE_OPS_LIBRARY_DIR}/libmoretfheoperations.so ${TFHE_NEW_LIBRARY_DIR}/libnewtfhe.so ${OPENSSL_LIBRARIES}/libcrypto.a)
#target_include_directories(Register PUBLIC ${TFHE_INCLUDE_DIR})
target_link_libraries(Register SEAL::seal ${TFHE_LIBRARY_DIR}/libtfhe-spqlios-avx.so ${TFHE_OPS_LIBRARY_DIR}/libmoretfheoperations.so ${TFHE_NEW_LIBRARY_DIR}/libnewtfhe.so ${OPENSSL_LIBRARIES}/libcrypto.a)
#target_include_directories(Verify PUBLIC ${TFHE_INCLUDE_DIR})
target_link_libraries(Verify SEAL::seal ${TFHE_LIBRARY_DIR}/libtfhe-spqlios-avx.so ${TFHE_OPS_LIBRARY_DIR}/libmoretfheoperations.so ${TFHE_NEW_LIBRARY_DIR}/libnewtfhe.so ${OPENSSL_LIBRARIES}/libcrypto.a)

# Manually specify the TFHE include directories and library paths
# set(TFHE_INCLUDE_DIR "/usr/local/include/tfhe")
# set(TFHE_LIBRARY_DIR "/usr/local/lib")

#link_directories(${TFHE_LIBRARY_DIR})

# Print the values of the variables to the console
# message(STATUS "TFHE_INCLUDE_DIR: ${TFHE_INCLUDE_DIR}")
# message(STATUS "TFHE_LIBRARY_DIR: ${TFHE_LIBRARY_DIR}")



#set(CPLUS_INCLUDE_PATH "${CPLUS_INCLUDE_PATH}:${TFHE_INCLUDE_DIR}")
 
# Add the executable
#target_include_directories(HESysInit PUBLIC ${TFHE_INCLUDE_DIR})


set_target_properties(HESysInit
PROPERTIES
RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)

set_target_properties(Register
PROPERTIES
RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)

set_target_properties(Verify
PROPERTIES
RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)
