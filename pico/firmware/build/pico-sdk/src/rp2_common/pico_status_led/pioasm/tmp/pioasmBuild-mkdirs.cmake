# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/svk_unix/pico-sdk/tools/pioasm"
  "/mnt/c/Users/akusu/Desktop/College/Year 2 - 2627/Fall Sem - 26/3 - CS378/FRITentacleProject/pico/firmware/build/pioasm"
  "/mnt/c/Users/akusu/Desktop/College/Year 2 - 2627/Fall Sem - 26/3 - CS378/FRITentacleProject/pico/firmware/build/pioasm-install"
  "/mnt/c/Users/akusu/Desktop/College/Year 2 - 2627/Fall Sem - 26/3 - CS378/FRITentacleProject/pico/firmware/build/pico-sdk/src/rp2_common/pico_status_led/pioasm/tmp"
  "/mnt/c/Users/akusu/Desktop/College/Year 2 - 2627/Fall Sem - 26/3 - CS378/FRITentacleProject/pico/firmware/build/pico-sdk/src/rp2_common/pico_status_led/pioasm/src/pioasmBuild-stamp"
  "/mnt/c/Users/akusu/Desktop/College/Year 2 - 2627/Fall Sem - 26/3 - CS378/FRITentacleProject/pico/firmware/build/pico-sdk/src/rp2_common/pico_status_led/pioasm/src"
  "/mnt/c/Users/akusu/Desktop/College/Year 2 - 2627/Fall Sem - 26/3 - CS378/FRITentacleProject/pico/firmware/build/pico-sdk/src/rp2_common/pico_status_led/pioasm/src/pioasmBuild-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/mnt/c/Users/akusu/Desktop/College/Year 2 - 2627/Fall Sem - 26/3 - CS378/FRITentacleProject/pico/firmware/build/pico-sdk/src/rp2_common/pico_status_led/pioasm/src/pioasmBuild-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/mnt/c/Users/akusu/Desktop/College/Year 2 - 2627/Fall Sem - 26/3 - CS378/FRITentacleProject/pico/firmware/build/pico-sdk/src/rp2_common/pico_status_led/pioasm/src/pioasmBuild-stamp${cfgdir}") # cfgdir has leading slash
endif()
