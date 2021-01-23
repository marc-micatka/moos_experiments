cmake_minimum_required(VERSION 3.11)
project(moos_echoer)

# If we were following ROS's conventions, the executable would be
# a very thin wrapper around the library, with the library installed
# separately.
#
# ROS python libraries get installed to
# /opt/ros/noetic/lib/python3/dist-packages/[package_name]/__init__.py
# ALL of the code gets stuffed into that __init__.py (at least for rostopic)
install(PROGRAMS py/moos_echoer.py
  DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
)
