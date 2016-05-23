# Copyright: (C) Alberto Cardellino
# Authors: Alberto Cardellino (alberto.cardellino@iit.it)
# CopyPolicy: Released under the terms of the GNU GPL v3.0.

#minimum cmake required version
cmake_minimum_required(VERSION 2.8)

set(project_name pointCloud_conversion)
project(${project_name})

# find the required packages
find_package(YARP REQUIRED)


###########
## Build ##
###########

# headers
set( msg_include_dirs ${CMAKE_CURRENT_SOURCE_DIR}/../msg )


# include directories list
include_directories( ${YARP_INCLUDE_DIRS} ${msg_include_dirs} )

add_executable(RGBD2pointCloud PointCloudConverter.cpp RGBD2PointCloud.cpp)

##########
## Link ##
##########

target_link_libraries( RGBD2pointCloud ${YARP_LIBRARIES})


#############
## Install ##
#############


###########
## Tests ##
###########

