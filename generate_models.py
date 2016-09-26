# -*- coding: utf-8 -*-
"""
Created on Sun Sep 25 16:29:44 2016

@author: Luca Fiorio
"""
import os
import subprocess
import shutil

DEPENDENCIES = ['urdf2dh', 'simmechanics_to_urdf', 'gz']
workingDirectory = os.getcwd()

# Resources
RESOURCES_DIRECTORY = os.path.join(workingDirectory, 'resources')
RESOURCES_MESH_DIRECTORY = os.path.join(RESOURCES_DIRECTORY, 'mesh')
RESOURCES_CONFIGURATION_DIRECTORY = os.path.join(RESOURCES_DIRECTORY, 'conf')

# Gazebo parameters
GAZEBO_DIRECTORY = os.path.join(workingDirectory, 'gazebo', 'cer')
SDF_FILENAME = 'cer.sdf'

# RViz parameters
RVIZ_DIRECTORY = os.path.join(workingDirectory, 'catkin_ws', 'src', 'cer_rviz', 'urdf') 
RVIZ_MESH_PATH_TOCKEN = 'package://'

# SimMechanicsToURDF parameters
SIMMECHANICS_XML = "SIM_CER_ROBOT.xml"
YAML_FILENAME = "cer_options.yaml"
CSV_JOINT_FILENAME = "cer_joint_parameters.csv"
OUTPUT_TYPE = "xml"
URDF_FILENAME = "cer.urdf"
MESH_PATH_IN_YAML_FILE = 'meshes'
CONFIGURATION_PATH_IN_YAML_FILE = 'conf'

# Other parameters
XML_DECLARATION = "<?xml version=""1.0"" ?>"
WARNING_README_NAME = 'README.md'

# DH conversion parameters {parameters order: root_of_DH_chain, end_of_DH_chain, DH_parameters_filename}
DH_CONVERSION_PARAMETERS = [['torso_tripod_root', 'head_leopard_right', 'r_camera_chain.ini'],
                            ['torso_tripod_root', 'head_leopard_left', 'l_camera_chain.ini'],
                            ['torso_tripod_root', 'head_leopard_cyclopic', 'cyclopic_camera_chain.ini'],
                            ['torso_tripod_root', 'depth', 'xtion_depth_chain.ini'],
                            ['torso_tripod_root', 'depth_rgb', 'xtion_rgb_chain.ini'],
                            ['torso_tripod_root', 'depth_center', 'xtion_center_chain.ini'],
                            ['torso_tripod_root', 'gaze','gaze_chain.ini'],
                            ['torso_tripod_root', 'r_wrist_tripod_root', 'r_wrist_tripod_base_chain_x_back.ini'],
                            ['torso_tripod_root', 'l_wrist_tripod_root', 'l_wrist_tripod_base_chain_x_back.ini']]
DH_DIRECTORY = os.path.join(workingDirectory, 'dh_parameters') 

def checkDependency(dependecyName):
    FNULL = open(os.devnull, 'w')
    try:
        subprocess.call([dependecyName], stdout=FNULL, stderr=subprocess.STDOUT)
    except:
        print("The Dependency " + dependecyName + " is not correctly installed")
    FNULL.close()
    
def generateURDF(directory, simMechanich_xml, yaml_filename, csv_joint_filename, output_type):
    urdf = subprocess.check_output(["simmechanics_to_urdf", simMechanich_xml, 
                                    "--yaml", yaml_filename, "--csv-joint", 
                                    csv_joint_filename, "--output", output_type], cwd=directory, universal_newlines=True) 
    return urdf;
    
def generateSDF(urdf):
    sdf = subprocess.check_output(["gz", "sdf", "-p", urdf], universal_newlines=True) 
    return sdf;
    
def generateDH(urdf_file_name, parameters, directory):
    subprocess.call(["urdf2dh", urdf_file_name, parameters[0], parameters[1], parameters[2]], cwd=directory) 
    return True;
    
def addWarningReadme(folder):
    readme_filepath = os.path.join(folder, WARNING_README_NAME)
    readme_file = open(readme_filepath,"w")
    readme_file.write('AUTOMATICALLY GENERATED FILES: DO NOT EDIT!')
    readme_file.close()
    
def copyMultipleFiles(source_folder, destination_folder):
    source_files = os.listdir(source_folder)
    for file_name in source_files:
        full_file_name = os.path.join(source_folder, file_name)
        if (os.path.isfile(full_file_name)):
            shutil.copy(full_file_name, destination_folder)
    
# Checking dependencies
for dependency in DEPENDENCIES:
    checkDependency(dependency)

# URDF
urdf = generateURDF(RESOURCES_DIRECTORY, SIMMECHANICS_XML, YAML_FILENAME, CSV_JOINT_FILENAME, OUTPUT_TYPE)
urdf = XML_DECLARATION + os.linesep + urdf

# RViz
rviz_mesh_directory = os.path.join(RVIZ_DIRECTORY, MESH_PATH_IN_YAML_FILE)
rviz_urdf_filepath = RVIZ_DIRECTORY + os.sep + URDF_FILENAME
if os.path.exists(RVIZ_DIRECTORY):
    shutil.rmtree(RVIZ_DIRECTORY)
shutil.copytree(RESOURCES_MESH_DIRECTORY, rviz_mesh_directory)
rviz_urdf = urdf.replace('model://', RVIZ_MESH_PATH_TOCKEN)
urdf_file = open(rviz_urdf_filepath,"w")
urdf_file.write(rviz_urdf)
urdf_file.close()
addWarningReadme(RVIZ_DIRECTORY)

# SDF
# TODO: start Gazebo directly from urdf and get rid of the sdf: http://gazebosim.org/tutorials/?tut=ros_urdf
sdf = generateSDF(rviz_urdf_filepath)
sdf = XML_DECLARATION + os.linesep + sdf

# Gazebo
gazebo_mesh_directory = os.path.join(GAZEBO_DIRECTORY, MESH_PATH_IN_YAML_FILE)
gazebo_configuration_directory = os.path.join(GAZEBO_DIRECTORY, CONFIGURATION_PATH_IN_YAML_FILE)
gazebo_sdf_filepath = GAZEBO_DIRECTORY + os.sep + SDF_FILENAME
if os.path.exists(GAZEBO_DIRECTORY):
    shutil.rmtree(GAZEBO_DIRECTORY)
shutil.copytree(RESOURCES_MESH_DIRECTORY, gazebo_mesh_directory)
os.mkdir(gazebo_configuration_directory)
copyMultipleFiles(RESOURCES_CONFIGURATION_DIRECTORY, gazebo_configuration_directory)
sdf_file = open(gazebo_sdf_filepath,"w")
sdf_file.write(sdf)
sdf_file.close()
addWarningReadme(GAZEBO_DIRECTORY)

# DH
# TODO: get rid of this copy and delete
if os.path.exists(DH_DIRECTORY):
    shutil.rmtree(DH_DIRECTORY)
os.mkdir(DH_DIRECTORY)
shutil.copy(rviz_urdf_filepath, DH_DIRECTORY)
for parameters in DH_CONVERSION_PARAMETERS:
    generateDH(URDF_FILENAME, parameters, DH_DIRECTORY)
os.remove(os.path.join(DH_DIRECTORY,URDF_FILENAME))
addWarningReadme(DH_DIRECTORY)