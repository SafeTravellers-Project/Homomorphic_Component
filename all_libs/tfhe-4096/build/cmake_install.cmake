# Install script for directory: /home/olivechakraborty/Documents/CEA/Projects/SAFETravellers/Codes/Project25Update/tfhe-4096/src

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/home/olivechakraborty/Documents/CEA/Projects/SAFETravellers/Codes/Project25Update/tfhe-4096/build")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "optim")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/tfhe" TYPE FILE PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ FILES
    "/home/olivechakraborty/Documents/CEA/Projects/SAFETravellers/Codes/Project25Update/tfhe-4096/src/include/lagrangehalfc_arithmetic.h"
    "/home/olivechakraborty/Documents/CEA/Projects/SAFETravellers/Codes/Project25Update/tfhe-4096/src/include/lwe-functions.h"
    "/home/olivechakraborty/Documents/CEA/Projects/SAFETravellers/Codes/Project25Update/tfhe-4096/src/include/lwebootstrappingkey.h"
    "/home/olivechakraborty/Documents/CEA/Projects/SAFETravellers/Codes/Project25Update/tfhe-4096/src/include/lwekey.h"
    "/home/olivechakraborty/Documents/CEA/Projects/SAFETravellers/Codes/Project25Update/tfhe-4096/src/include/lwekeyswitch.h"
    "/home/olivechakraborty/Documents/CEA/Projects/SAFETravellers/Codes/Project25Update/tfhe-4096/src/include/lweparams.h"
    "/home/olivechakraborty/Documents/CEA/Projects/SAFETravellers/Codes/Project25Update/tfhe-4096/src/include/lwesamples.h"
    "/home/olivechakraborty/Documents/CEA/Projects/SAFETravellers/Codes/Project25Update/tfhe-4096/src/include/numeric_functions.h"
    "/home/olivechakraborty/Documents/CEA/Projects/SAFETravellers/Codes/Project25Update/tfhe-4096/src/include/polynomials.h"
    "/home/olivechakraborty/Documents/CEA/Projects/SAFETravellers/Codes/Project25Update/tfhe-4096/src/include/polynomials_arithmetic.h"
    "/home/olivechakraborty/Documents/CEA/Projects/SAFETravellers/Codes/Project25Update/tfhe-4096/src/include/tfhe.h"
    "/home/olivechakraborty/Documents/CEA/Projects/SAFETravellers/Codes/Project25Update/tfhe-4096/src/include/tfhe_core.h"
    "/home/olivechakraborty/Documents/CEA/Projects/SAFETravellers/Codes/Project25Update/tfhe-4096/src/include/tfhe_garbage_collector.h"
    "/home/olivechakraborty/Documents/CEA/Projects/SAFETravellers/Codes/Project25Update/tfhe-4096/src/include/tfhe_gate_bootstrapping_functions.h"
    "/home/olivechakraborty/Documents/CEA/Projects/SAFETravellers/Codes/Project25Update/tfhe-4096/src/include/tfhe_gate_bootstrapping_structures.h"
    "/home/olivechakraborty/Documents/CEA/Projects/SAFETravellers/Codes/Project25Update/tfhe-4096/src/include/tfhe_generic_streams.h"
    "/home/olivechakraborty/Documents/CEA/Projects/SAFETravellers/Codes/Project25Update/tfhe-4096/src/include/tfhe_generic_templates.h"
    "/home/olivechakraborty/Documents/CEA/Projects/SAFETravellers/Codes/Project25Update/tfhe-4096/src/include/tfhe_io.h"
    "/home/olivechakraborty/Documents/CEA/Projects/SAFETravellers/Codes/Project25Update/tfhe-4096/src/include/tgsw.h"
    "/home/olivechakraborty/Documents/CEA/Projects/SAFETravellers/Codes/Project25Update/tfhe-4096/src/include/tgsw_functions.h"
    "/home/olivechakraborty/Documents/CEA/Projects/SAFETravellers/Codes/Project25Update/tfhe-4096/src/include/tlwe.h"
    "/home/olivechakraborty/Documents/CEA/Projects/SAFETravellers/Codes/Project25Update/tfhe-4096/src/include/tlwe_functions.h"
    )
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/olivechakraborty/Documents/CEA/Projects/SAFETravellers/Codes/Project25Update/tfhe-4096/build/libtfhe/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/home/olivechakraborty/Documents/CEA/Projects/SAFETravellers/Codes/Project25Update/tfhe-4096/build/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
