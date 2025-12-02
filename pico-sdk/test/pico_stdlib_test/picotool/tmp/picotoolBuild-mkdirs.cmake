# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/doug/nuttxspace-picow/pico-sdk/_deps/picotool-src"
  "/home/doug/nuttxspace-picow/pico-sdk/_deps/picotool-build"
  "/home/doug/nuttxspace-picow/pico-sdk/_deps"
  "/home/doug/nuttxspace-picow/pico-sdk/test/pico_stdlib_test/picotool/tmp"
  "/home/doug/nuttxspace-picow/pico-sdk/test/pico_stdlib_test/picotool/src/picotoolBuild-stamp"
  "/home/doug/nuttxspace-picow/pico-sdk/test/pico_stdlib_test/picotool/src"
  "/home/doug/nuttxspace-picow/pico-sdk/test/pico_stdlib_test/picotool/src/picotoolBuild-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/doug/nuttxspace-picow/pico-sdk/test/pico_stdlib_test/picotool/src/picotoolBuild-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/doug/nuttxspace-picow/pico-sdk/test/pico_stdlib_test/picotool/src/picotoolBuild-stamp${cfgdir}") # cfgdir has leading slash
endif()
