cmake_minimum_required(VERSION 3.10.2)
project(test_messages)

find_package(Protobuf REQUIRED)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS
 test_messages.proto
)
protobuf_generate_python(PROTO_PYS test_messages.proto)


add_library(test_proto ${PROTO_SRCS} ${PROTO_HDRS})

target_link_libraries(test_proto ${Protobuf_LIBRARIES})
set_target_properties(test_proto PROPERTIES PUBLIC_HEADER ${PROTO_HDRS})

install(TARGETS test_proto
  LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_PREFIX}/include
)

# QUESTION: Is this at all right for adding python targets?
message(STATUS "project_name = ${PROJECT_NAME}")
message(STATUS "Proto_pys = ${PROTO_PYS}")
add_custom_target(proto_python ALL DEPENDS ${PROTO_PYS})
install(FILES ${PROTO_PYS} __init__.py
  DESTINATION ${CMAKE_INSTALL_PREFIX}/py/${PROJECT_NAME}_pb2
)
