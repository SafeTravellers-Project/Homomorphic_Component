# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

if(EXISTS "/home/olivechakraborty/Documents/CEA/Projects/SAFETravellers/Codes/Project25Update/all_libs/SEAL/build/thirdparty/zstd-subbuild/zstd-populate-prefix/src/zstd-populate-stamp/zstd-populate-gitclone-lastrun.txt" AND EXISTS "/home/olivechakraborty/Documents/CEA/Projects/SAFETravellers/Codes/Project25Update/all_libs/SEAL/build/thirdparty/zstd-subbuild/zstd-populate-prefix/src/zstd-populate-stamp/zstd-populate-gitinfo.txt" AND
  "/home/olivechakraborty/Documents/CEA/Projects/SAFETravellers/Codes/Project25Update/all_libs/SEAL/build/thirdparty/zstd-subbuild/zstd-populate-prefix/src/zstd-populate-stamp/zstd-populate-gitclone-lastrun.txt" IS_NEWER_THAN "/home/olivechakraborty/Documents/CEA/Projects/SAFETravellers/Codes/Project25Update/all_libs/SEAL/build/thirdparty/zstd-subbuild/zstd-populate-prefix/src/zstd-populate-stamp/zstd-populate-gitinfo.txt")
  message(STATUS
    "Avoiding repeated git clone, stamp file is up to date: "
    "'/home/olivechakraborty/Documents/CEA/Projects/SAFETravellers/Codes/Project25Update/all_libs/SEAL/build/thirdparty/zstd-subbuild/zstd-populate-prefix/src/zstd-populate-stamp/zstd-populate-gitclone-lastrun.txt'"
  )
  return()
endif()

execute_process(
  COMMAND ${CMAKE_COMMAND} -E rm -rf "/home/olivechakraborty/Documents/CEA/Projects/SAFETravellers/Codes/Project25Update/all_libs/SEAL/build/thirdparty/zstd-src"
  RESULT_VARIABLE error_code
)
if(error_code)
  message(FATAL_ERROR "Failed to remove directory: '/home/olivechakraborty/Documents/CEA/Projects/SAFETravellers/Codes/Project25Update/all_libs/SEAL/build/thirdparty/zstd-src'")
endif()

# try the clone 3 times in case there is an odd git clone issue
set(error_code 1)
set(number_of_tries 0)
while(error_code AND number_of_tries LESS 3)
  execute_process(
    COMMAND "/usr/bin/git"
            clone --no-checkout --config "advice.detachedHead=false" "https://github.com/facebook/zstd.git" "zstd-src"
    WORKING_DIRECTORY "/home/olivechakraborty/Documents/CEA/Projects/SAFETravellers/Codes/Project25Update/all_libs/SEAL/build/thirdparty"
    RESULT_VARIABLE error_code
  )
  math(EXPR number_of_tries "${number_of_tries} + 1")
endwhile()
if(number_of_tries GREATER 1)
  message(STATUS "Had to git clone more than once: ${number_of_tries} times.")
endif()
if(error_code)
  message(FATAL_ERROR "Failed to clone repository: 'https://github.com/facebook/zstd.git'")
endif()

execute_process(
  COMMAND "/usr/bin/git"
          checkout "f8745da6ff1ad1e7bab384bd1f9d742439278e99" --
  WORKING_DIRECTORY "/home/olivechakraborty/Documents/CEA/Projects/SAFETravellers/Codes/Project25Update/all_libs/SEAL/build/thirdparty/zstd-src"
  RESULT_VARIABLE error_code
)
if(error_code)
  message(FATAL_ERROR "Failed to checkout tag: 'f8745da6ff1ad1e7bab384bd1f9d742439278e99'")
endif()

set(init_submodules TRUE)
if(init_submodules)
  execute_process(
    COMMAND "/usr/bin/git" 
            submodule update --recursive --init 
    WORKING_DIRECTORY "/home/olivechakraborty/Documents/CEA/Projects/SAFETravellers/Codes/Project25Update/all_libs/SEAL/build/thirdparty/zstd-src"
    RESULT_VARIABLE error_code
  )
endif()
if(error_code)
  message(FATAL_ERROR "Failed to update submodules in: '/home/olivechakraborty/Documents/CEA/Projects/SAFETravellers/Codes/Project25Update/all_libs/SEAL/build/thirdparty/zstd-src'")
endif()

# Complete success, update the script-last-run stamp file:
#
execute_process(
  COMMAND ${CMAKE_COMMAND} -E copy "/home/olivechakraborty/Documents/CEA/Projects/SAFETravellers/Codes/Project25Update/all_libs/SEAL/build/thirdparty/zstd-subbuild/zstd-populate-prefix/src/zstd-populate-stamp/zstd-populate-gitinfo.txt" "/home/olivechakraborty/Documents/CEA/Projects/SAFETravellers/Codes/Project25Update/all_libs/SEAL/build/thirdparty/zstd-subbuild/zstd-populate-prefix/src/zstd-populate-stamp/zstd-populate-gitclone-lastrun.txt"
  RESULT_VARIABLE error_code
)
if(error_code)
  message(FATAL_ERROR "Failed to copy script-last-run stamp file: '/home/olivechakraborty/Documents/CEA/Projects/SAFETravellers/Codes/Project25Update/all_libs/SEAL/build/thirdparty/zstd-subbuild/zstd-populate-prefix/src/zstd-populate-stamp/zstd-populate-gitclone-lastrun.txt'")
endif()
