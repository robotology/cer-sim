cmake_minimum_required(VERSION 2.8.9)
project(geomagic-marker)

find_package(YARP REQUIRED)
list(APPEND CMAKE_MODULE_PATH ${YARP_MODULE_PATH})

yarp_idl_to_dir(visualization_msgs/Marker.msg ${CMAKE_BINARY_DIR}/idl IDL_SRC IDL_HDR IDL_INCLUDE)
yarp_idl_to_dir(tf2_msgs/TFMessage.msg ${CMAKE_BINARY_DIR}/idl IDL_SRC IDL_HDR IDL_INCLUDE)
include_directories(${IDL_INCLUDE})

add_executable(geomagic-marker main.cpp
                               GeomagicMarkerModule.cpp
                               GeomagicMarkerPort.cpp)

target_link_libraries(geomagic-marker YARP::YARP_OS YARP::YARP_init)
