from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, IncludeLaunchDescription
from launch.substitutions import LaunchConfiguration, ThisLaunchFileDir
from launch.launch_description_sources import PythonLaunchDescriptionSource

from launch_ros.actions import Node


def generate_launch_description():

    # Arguments declaration
    robot_xacro_decl = DeclareLaunchArgument(
            'robot_xacro_name',
            default_value='cer_R1SN003_neck_pos_only.urdf.xacro',
            description='The robot xacro file name')
    controllers_file_decl = DeclareLaunchArgument(
            'controllers_file',
            default_value='cer_R1SN003_ros2_control_neck_pos_only.yaml',
            description='The ros2 controllers configuration file')
    launch_rviz2_decl = DeclareLaunchArgument(
            'launch_rviz2',
            default_value='false',
            description='If true, run rviz2')
    cb_node_name_decl = DeclareLaunchArgument(
            'input_node',
            default_value='neck_cb_hw_pos',
            description='Node name for the CbHwFwPos object')
    cb_comp_name_decl = DeclareLaunchArgument(
            'input_component',
            default_value='NeckCbHwFwPos',
            description='Component name for the CbHwFwPos object')
    cb_msgs_name_decl = DeclareLaunchArgument(
            'input_msgs_name',
            default_value='ros2_cb_msgs',
            description='YARP ControlBoard control related topics and services prefix')
    cb_cont_pos_write_decl = DeclareLaunchArgument(
            'input_cont_pos_write',
            default_value='false',
            description='If true, the hardware component will continuosly publish the stored position command interfaces values even if they did not change')

    declared_args = [robot_xacro_decl,
                     controllers_file_decl,
                     launch_rviz2_decl,
                     cb_node_name_decl,
                     cb_comp_name_decl,
                     cb_msgs_name_decl,
                     cb_cont_pos_write_decl]

    # Launch configurations
    robot_xacro = LaunchConfiguration('robot_xacro_name')
    controllers_file = LaunchConfiguration('controllers_file')
    launch_rviz2 = LaunchConfiguration('launch_rviz2')
    cb_node_name = LaunchConfiguration('input_node')
    cb_comp_name = LaunchConfiguration('input_component')
    cb_msgs_name = LaunchConfiguration('input_msgs_name')
    cb_cont_pos_write = LaunchConfiguration('input_cont_pos_write')

    # TODO: in a future version, the spawners number should not be fixed but determined by the content of the selected yaml file
    neck_controller_spawner = Node(
        package="controller_manager",
        executable="spawner",
        arguments=["neck_position_controller", "-c", "/controller_manager"],
    )

    base_launch = IncludeLaunchDescription(
        PythonLaunchDescriptionSource([ThisLaunchFileDir(), "/cer_control_base.launch.py"]),
        launch_arguments={
            "robot_xacro_name": robot_xacro,
            "controllers_file": controllers_file,
            "launch_rviz2": launch_rviz2,
            "input_node": cb_node_name,
            "input_component": cb_comp_name,
            "input_msgs_name": cb_msgs_name,
            "input_cont_pos_write": cb_cont_pos_write
        }.items(),
    )

    return LaunchDescription(declared_args+[base_launch]+[neck_controller_spawner])
