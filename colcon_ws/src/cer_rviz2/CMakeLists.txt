cmake_minimum_required(VERSION 3.5)
project(cer_rviz2)
find_package(ament_cmake REQUIRED)
install(
  DIRECTORY
    launch
  DESTINATION
    share/${PROJECT_NAME}/
)
install(
  DIRECTORY
    urdf
  DESTINATION
    share/${PROJECT_NAME}/
)
install(
  DIRECTORY
    rviz2
  DESTINATION
    share/${PROJECT_NAME}/
)
ament_package()
