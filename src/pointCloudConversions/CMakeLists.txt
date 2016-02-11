# Copyright: (C) Alberto Cardellino
# Authors: Alberto Cardellino (alberto.cardellino@iit.it)
# CopyPolicy: Released under the terms of the GNU GPL v3.0.

#minimum cmake required version
cmake_minimum_required(VERSION 2.8)

set(project_name pointCloud_conversion)
project(${project_name})

# find the required packages
find_package(YARP REQUIRED)
find_package(rosconsole REQUIRED)
find_package(rosnode REQUIRED)
find_package(roscpp REQUIRED)
find_package(tf REQUIRED)

link_directories(${PCL_LIBRARY_DIRS})
add_definitions(${PCL_DEFINITIONS})


find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})


#######################
##     ROS STUFF     ##
#######################

include($ENV{ROS_ROOT}/core/rosbuild/rosbuild.cmake)

###########
## Build ##
###########

# headers
set( ${project_name}_headers ${msg_sources} ${msg_headers})

# include directories list
include_directories( ${YARP_INCLUDE_DIRS} ${msg_include_dirs} )

# ROS include dirs
include_directories( ${rosconsole_INCLUDE_DIRS} ${rosnode_INCLUDE_DIRS} ${roscpp_INCLUDE_DIRS} )

add_executable(RGBD2pointCloud PointCloudConverter.cpp RGBD2PointCloud.cpp)


##########
## Link ##
##########

target_link_libraries( RGBD2pointCloud ${YARP_LIBRARIES} ${BOOST_LIBRARIES})

# link ROS libraries
target_link_libraries( RGBD2pointCloud ${tf_LIBRARIES} ${rosconsole_LIBRARIES} ${rosnode_LIBRARIES} ${roscpp_LIBRARIES})


#############
## Install ##
#############


###########
## Tests ##
###########

