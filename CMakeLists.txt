cmake_minimum_required(VERSION 3.10.2)
project(moos_experiments)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# From: https://stackoverflow.com/questions/39481958/setting-cmake-install-prefix-from-cmakelists-txt-file/39485990#39485990
IF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
SET(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/devel CACHE PATH "setting install prefix" FORCE)
ENDIF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

add_subdirectory(src/talker)
add_subdirectory(src/listener)
add_subdirectory(src/test_messages)

add_subdirectory(src/timing_tests)

add_subdirectory(src/moos_echoer)

message(STATUS "top-level project_binary_dir = ${PROJECT_BINARY_DIR}")
