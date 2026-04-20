cmake_minimum_required(VERSION 3.10)

# Set the project name and version
project(SAFETravellers VERSION 3.0)
set(PROJECT_DIR ${CMAKE_SOURCE_DIR})
set(LIBS_DIR ${PROJECT_DIR}/all_libs)
# Specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# set(TFHE_INCLUDE_DIR "/usr/local/include/tfhe")
# set(TFHE_LIBRARY_DIR "/usr/local/lib")


set(TFHE_INCLUDE_DIR ${LIBS_DIR}/tfhe/src/include)
set(TFHE_LIBRARY_DIR ${LIBS_DIR}/tfhe/build/libtfhe)

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
add_executable(HESysInit src/HESysInit.cpp src/params.cpp src/keysHEComp.cpp src/keysEGate.cpp) 
add_executable(Register src/Register.cpp src/keysUser.cpp src/utils.cpp src/dataIO.cpp src/token.cpp )
add_executable(EncBio src/Enc_Test_Bio.cpp src/params.cpp src/dataIO.cpp )
add_executable(Verify src/Verify.cpp src/keysEGate.cpp src/token.cpp src/params.cpp src/dataIO.cpp src/utils.cpp)

# Include directories for SEAL and TFHE
#include_directories(${SEAL_INCLUDE_DIR})
include_directories(${TFHE_INCLUDE_DIR})
include_directories(${TFHE_OPS_INCLUDE_DIR})
include_directories(${TFHE_NEW_INCLUDE_DIR})
include_directories(${OPENSSL_INCLUDE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/include)

target_include_directories(HESysInit PRIVATE ../include)


target_link_libraries(HESysInit SEAL::seal ${TFHE_LIBRARY_DIR}/libtfhe-spqlios-avx.so  ${TFHE_OPS_LIBRARY_DIR}/libmoretfheoperations.so ${TFHE_NEW_LIBRARY_DIR}/libnewtfhe.so ${OPENSSL_LIBRARIES}/libcrypto.so)

target_link_libraries(Register SEAL::seal ${TFHE_LIBRARY_DIR}/libtfhe-spqlios-avx.so ${TFHE_OPS_LIBRARY_DIR}/libmoretfheoperations.so ${TFHE_NEW_LIBRARY_DIR}/libnewtfhe.so ${OPENSSL_LIBRARIES}/libcrypto.so)

target_link_libraries(EncBio SEAL::seal ${TFHE_LIBRARY_DIR}/libtfhe-spqlios-avx.so ${TFHE_OPS_LIBRARY_DIR}/libmoretfheoperations.so ${TFHE_NEW_LIBRARY_DIR}/libnewtfhe.so ${OPENSSL_LIBRARIES}/libcrypto.so)

target_link_libraries(Verify SEAL::seal ${TFHE_LIBRARY_DIR}/libtfhe-spqlios-avx.so ${TFHE_OPS_LIBRARY_DIR}/libmoretfheoperations.so ${TFHE_NEW_LIBRARY_DIR}/libnewtfhe.so ${OPENSSL_LIBRARIES}/libcrypto.so)

 
# Add the executable

set_target_properties(HESysInit
PROPERTIES
RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)

set_target_properties(Register
PROPERTIES
RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)

set_target_properties(EncBio
PROPERTIES
RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)

set_target_properties(Verify
PROPERTIES
RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)
