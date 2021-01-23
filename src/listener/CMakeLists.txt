cmake_minimum_required(VERSION 3.10.2)
project(listener)

find_package(MOOS 10 REQUIRED)
find_package(Boost 1.65 REQUIRED)

add_executable(listener_classic
  src/listener_classic_main.cpp
  src/listener_classic.cpp
)
target_include_directories(listener_classic PRIVATE
  include/${PROJECT_NAME}
  ${MOOS_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS}
)
target_link_libraries(listener_classic
  ${MOOS_LIBRARIES}
  ${Boost_LIBRARIES}
)

add_executable(listener_protobuf
  src/listener_protobuf_main.cpp
  src/listener_protobuf.cpp
)

target_include_directories(listener_protobuf PRIVATE
  include/${PROJECT_NAME}
  # From: https://stackoverflow.com/questions/27198947/cmake-how-to-create-include-path-to-generated-include-files
  # The location of the headers before installation
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/../test_messages/>
  # The location of the headers after installation
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>
  # ${CMAKE_INSTALL_PREFIX}/include  # This doesn't get created early enough.
  ${MOOS_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS}
)
target_link_libraries(listener_protobuf
  ${MOOS_LIBRARIES}
  ${Boost_LIBRARIES}
  test_proto
)

install(TARGETS listener_classic listener_protobuf
	DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
)
