cmake_minimum_required(VERSION 3.10.2)
project(cer_ros2_control)

find_package(ament_cmake REQUIRED)

ament_package()

install(DIRECTORY launch DESTINATION share/${PROJECT_NAME})
install(DIRECTORY config DESTINATION share/${PROJECT_NAME})
install(DIRECTORY urdf DESTINATION share/${PROJECT_NAME})
install(DIRECTORY ros2_control DESTINATION share/${PROJECT_NAME})
