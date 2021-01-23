cmake_minimum_required(VERSION 3.11)
project(talker)

find_package(MOOS 10 REQUIRED)
find_package(Boost 1.71 REQUIRED)

add_executable(talker_classic
  src/talker_classic_main.cpp
  src/talker_classic.cpp
)
target_include_directories(talker_classic PRIVATE
  include/${PROJECT_NAME}
  ${MOOS_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS}
)
target_link_libraries(talker_classic
  ${MOOS_LIBRARIES}
  ${Boost_LIBRARIES}
)

add_executable(talker_protobuf
  src/talker_protobuf_main.cpp
  src/talker_protobuf.cpp
)
target_include_directories(talker_protobuf PRIVATE
  include/${PROJECT_NAME}
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/../test_messages/>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>
  ${MOOS_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS}
)
target_link_libraries(talker_protobuf
  ${MOOS_LIBRARIES}
  ${Boost_LIBRARIES}
  test_proto
)

install(
  TARGETS talker_classic talker_protobuf
  DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
)
