# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/Users/kiran/esp/esp-idf/components/bootloader/subproject"
  "/Users/kiran/Documents/Projects/ICLR/Ricardo-OS-Template/build/bootloader"
  "/Users/kiran/Documents/Projects/ICLR/Ricardo-OS-Template/build/bootloader-prefix"
  "/Users/kiran/Documents/Projects/ICLR/Ricardo-OS-Template/build/bootloader-prefix/tmp"
  "/Users/kiran/Documents/Projects/ICLR/Ricardo-OS-Template/build/bootloader-prefix/src/bootloader-stamp"
  "/Users/kiran/Documents/Projects/ICLR/Ricardo-OS-Template/build/bootloader-prefix/src"
  "/Users/kiran/Documents/Projects/ICLR/Ricardo-OS-Template/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/Users/kiran/Documents/Projects/ICLR/Ricardo-OS-Template/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/Users/kiran/Documents/Projects/ICLR/Ricardo-OS-Template/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
