import os

import launch.actions
import launch_ros.actions
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    urdf = "/home/user1/cer-sim/colcon_ws/install/cer_rviz2/share/cer_rviz2/urdf/cer.urdf"
    with open(urdf, 'r') as infp:
        robot_desc = infp.read()

    ld = LaunchDescription([
        Node(
            package='robot_state_publisher',
            executable='robot_state_publisher',
            name='robot_state_publisher_ros2',
            output='screen',
            parameters=[{'use_sim_time': True},
                        {'robot_description': robot_desc}]
        )
    ])
    return ld


if __name__ == '__main__':
    generate_launch_description()
