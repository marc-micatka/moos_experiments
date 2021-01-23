cmake_minimum_required(VERSION 3.11)
project(timing_tests)

find_package(MOOS 10 REQUIRED)

add_executable(source_node src/source_node.cpp)
add_executable(relay_node src/relay_node.cpp)
add_executable(relay_queue_node src/relay_queue_node.cpp)
add_executable(sink_node src/sink_node.cpp)

target_include_directories(source_node PRIVATE
  ${MOOSE_INCLUDE_DIRS}
)
target_link_libraries(source_node
  ${MOOS_LIBRARIES}
)

target_include_directories(relay_node PRIVATE
  ${MOOSE_INCLUDE_DIRS}
)
target_link_libraries(relay_node
  ${MOOS_LIBRARIES}
)
target_include_directories(relay_queue_node PRIVATE
  ${MOOSE_INCLUDE_DIRS}
)
target_link_libraries(relay_queue_node
  ${MOOS_LIBRARIES}
)

target_include_directories(sink_node PRIVATE
  ${MOOSE_INCLUDE_DIRS}
)
target_link_libraries(sink_node
  ${MOOS_LIBRARIES}
)

install(
  TARGETS source_node relay_node relay_queue_node sink_node
  DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
)
