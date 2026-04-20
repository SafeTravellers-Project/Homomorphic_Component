# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/olivechakraborty/Documents/CEA/Projects/SAFETravellers/Codes/Project25Update/all_libs/SEAL/build/thirdparty/msgsl-src"
  "/home/olivechakraborty/Documents/CEA/Projects/SAFETravellers/Codes/Project25Update/all_libs/SEAL/build/thirdparty/msgsl-build"
  "/home/olivechakraborty/Documents/CEA/Projects/SAFETravellers/Codes/Project25Update/all_libs/SEAL/build/thirdparty/msgsl-subbuild/msgsl-populate-prefix"
  "/home/olivechakraborty/Documents/CEA/Projects/SAFETravellers/Codes/Project25Update/all_libs/SEAL/build/thirdparty/msgsl-subbuild/msgsl-populate-prefix/tmp"
  "/home/olivechakraborty/Documents/CEA/Projects/SAFETravellers/Codes/Project25Update/all_libs/SEAL/build/thirdparty/msgsl-subbuild/msgsl-populate-prefix/src/msgsl-populate-stamp"
  "/home/olivechakraborty/Documents/CEA/Projects/SAFETravellers/Codes/Project25Update/all_libs/SEAL/build/thirdparty/msgsl-subbuild/msgsl-populate-prefix/src"
  "/home/olivechakraborty/Documents/CEA/Projects/SAFETravellers/Codes/Project25Update/all_libs/SEAL/build/thirdparty/msgsl-subbuild/msgsl-populate-prefix/src/msgsl-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/olivechakraborty/Documents/CEA/Projects/SAFETravellers/Codes/Project25Update/all_libs/SEAL/build/thirdparty/msgsl-subbuild/msgsl-populate-prefix/src/msgsl-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/olivechakraborty/Documents/CEA/Projects/SAFETravellers/Codes/Project25Update/all_libs/SEAL/build/thirdparty/msgsl-subbuild/msgsl-populate-prefix/src/msgsl-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
