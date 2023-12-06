from launch import LaunchDescription
from launch.conditions import IfCondition
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
from launch.substitutions import Command, FindExecutable, PathJoinSubstitution

from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare


def generate_launch_description():

    # Arguments declaration
    robot_xacro_decl = DeclareLaunchArgument(
            'robot_xacro_name',
            description='The robot xacro file name')
    controllers_file_decl = DeclareLaunchArgument(
            'controllers_file',
            description='The ros2 controllers configuration file')
    sim_time_decl = DeclareLaunchArgument(
            'use_sim_time',
            default_value='false',
            description='Use simulation (Gazebo) clock if true')
    launch_rviz2_decl = DeclareLaunchArgument(
            'launch_rviz2',
            default_value='false',
            description='If true, run rviz2')
    cb_node_name_decl = DeclareLaunchArgument(
            'input_node',
            description='Node name for the CbHwTest object')
    cb_comp_name_decl = DeclareLaunchArgument(
            'input_component',
            description='Component name for the CbHwTest object')
    cb_msgs_name_decl = DeclareLaunchArgument(
            'input_msgs_name',
            default_value='ros2_cb_msgs',
            description='YARP ControlBoard control related topics and services prefix')
    cb_cont_pos_write_decl = DeclareLaunchArgument(
            'input_cont_pos_write',
            default_value='true',
            description='If true, the hardware component will continuosly publish the stored position command interfaces values even if they did not change')

    declared_args = [robot_xacro_decl,
                     controllers_file_decl,
                     sim_time_decl,
                     launch_rviz2_decl,
                     cb_node_name_decl,
                     cb_comp_name_decl,
                     cb_msgs_name_decl,
                     cb_cont_pos_write_decl]

    # Launch configurations
    robot_xacro = LaunchConfiguration('robot_xacro_name')
    controllers_file = LaunchConfiguration('controllers_file')
    use_sim_time = LaunchConfiguration('use_sim_time')
    launch_rviz2 = LaunchConfiguration('launch_rviz2')
    cb_node_name = LaunchConfiguration('input_node')
    cb_comp_name = LaunchConfiguration('input_component')
    cb_msgs_name = LaunchConfiguration('input_msgs_name')
    cb_cont_pos_write = LaunchConfiguration('input_cont_pos_write')

    # Get URDF via xacro
    robot_description_content = Command(
        [
            PathJoinSubstitution([FindExecutable(name="xacro")]),
            " ",
            PathJoinSubstitution(
                [
                    FindPackageShare("cer_ros2_control"),
                    "urdf",
                    robot_xacro,
                ]
            ),
            " ",
            "input_node:=",
            cb_node_name,
            " ",
            "input_component:=",
            cb_comp_name,
            " ",
            "input_msgs_name:=",
            cb_msgs_name,
            " ",
            "input_cont_pos_write:=",
            cb_cont_pos_write
        ]
    )
    robot_description = {"robot_description": robot_description_content}

    robot_controllers = PathJoinSubstitution(
        [
            FindPackageShare("cer_ros2_control"),
            "config",
            controllers_file,
        ]
    )
    rviz_config_file = PathJoinSubstitution(
        [FindPackageShare("cer_rviz2"), "rviz2", "cer_config.rviz"]
    )

    control_node = Node(
        package="controller_manager",
        executable="ros2_control_node",
        parameters=[robot_description, robot_controllers],
        output="both",
    )
    robot_state_pub_node = Node(
        package="robot_state_publisher",
        executable="robot_state_publisher",
        output="both",
        parameters=[{'use_sim_time': use_sim_time, 'robot_description': robot_description_content}],
    )
    rviz_node = Node(
        package="rviz2",
        executable="rviz2",
        name="rviz2",
        output="log",
        condition=IfCondition(launch_rviz2),
        arguments=["-d", rviz_config_file],
    )

    nodes = [
        control_node,
        robot_state_pub_node,
        rviz_node
    ]

    return LaunchDescription(declared_args+nodes)
