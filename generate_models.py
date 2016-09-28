# -*- coding: utf-8 -*-
"""
Created on Sun Sep 25 16:29:44 2016

@author: Luca Fiorio

Optimized for Python 3
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
GAZEBO_CONFIGURATION_FILENAME = 'model.config'
GAZEBO_CONFIGURATION_FILE_PATH = os.path.join(RESOURCES_DIRECTORY, GAZEBO_CONFIGURATION_FILENAME)

# RViz parameters
RVIZ_DIRECTORY = os.path.join(workingDirectory, 'catkin_ws', 'src', 'cer_rviz', 'urdf') 

# SimMechanicsToURDF parameters
SIMMECHANICS_XML = "SIM_CER_ROBOT.xml"
YAML_FILENAME = "cer_options.yaml"
CSV_JOINT_FILENAME = "cer_joint_parameters.csv"
OUTPUT_TYPE = "xml"
URDF_FILENAME = "cer.urdf"
MESH_PATH_IN_YAML_FILE = 'meshes'
CONFIGURATION_PATH_IN_YAML_FILE = 'conf'
URDF_MESH_PATH_TOCKEN = 'package://'

# Other parameters
XML_DECLARATION = "<?xml version=""1.0"" ?>"
WARNING_README_NAME = 'README.md'
BUILD_DIRECTORY = os.path.join(RESOURCES_DIRECTORY, 'build')

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
    
def generateURDF(directory, build_directory, urdf_file_name_and_extension, simMechanich_xml, yaml_filename, csv_joint_filename, output_type):
    urdf = subprocess.check_output(["simmechanics_to_urdf", simMechanich_xml, 
                                    "--yaml", yaml_filename, "--csv-joint", 
                                    csv_joint_filename, "--output", output_type], cwd=directory, universal_newlines=True) 
    urdf = XML_DECLARATION + os.linesep + urdf
    urdf = urdf.replace('model://', URDF_MESH_PATH_TOCKEN)
    urdf_filepath = os.path.join(build_directory, urdf_file_name_and_extension)
    urdf_file = open(urdf_filepath,"w")
    urdf_file.write(urdf)
    urdf_file.close()
    return True;
    
def generateSDF(directory, urdf_file_name_and_extension, sdf_file_name_and_extension):
    urdf = os.path.join(directory, urdf_file_name_and_extension)
    sdf = subprocess.check_output(["gz", "sdf", "-p", urdf], universal_newlines=True) 
    sdf = XML_DECLARATION + os.linesep + sdf
    sdf_filepath = os.path.join(directory, sdf_file_name_and_extension)
    sdf_file = open(sdf_filepath,"w")
    sdf_file.write(sdf)
    sdf_file.close()
    return True;
    
def generateDH(urdf_file_name, parameters, directory):
    subprocess.call(["urdf2dh", urdf_file_name, parameters[0], parameters[1], parameters[2]], cwd=directory) 
    return True;
    
def addWarningReadme(folder):
    readme_filepath = os.path.join(folder, WARNING_README_NAME)
    readme_file = open(readme_filepath,"w")
    readme_file.write('AUTOMATICALLY GENERATED FILES: DO NOT EDIT!')
    readme_file.close()
    
def copyMultipleFilesWithExtension(source_folder, destination_folder, extension):
    source_files = os.listdir(source_folder)
    for file_name in source_files:
        if file_name.endswith(extension):
            full_file_name = os.path.join(source_folder, file_name)
            if (os.path.isfile(full_file_name)):
                shutil.copy2(full_file_name, destination_folder) 

def copyFolderFiles(source_folder, destination_folder):
    source_files = os.listdir(source_folder)
    for file_name in source_files:
        full_file_name = os.path.join(source_folder, file_name)
        if (os.path.isfile(full_file_name)):
            shutil.copy2(full_file_name, destination_folder)
                

# Checking dependencies
for dependency in DEPENDENCIES:
    checkDependency(dependency)

# Clean previous builds if still here (in case an error occurred)
if os.path.exists(BUILD_DIRECTORY):
    shutil.rmtree(BUILD_DIRECTORY)
os.mkdir(BUILD_DIRECTORY)

# GENERATE NECESSARY FILES:
# URDF
generateURDF(RESOURCES_DIRECTORY, BUILD_DIRECTORY, URDF_FILENAME, SIMMECHANICS_XML, YAML_FILENAME, CSV_JOINT_FILENAME, OUTPUT_TYPE)
# SDF - TODO: start Gazebo directly from urdf and get rid of the sdf: http://gazebosim.org/tutorials/?tut=ros_urdf
generateSDF(BUILD_DIRECTORY, URDF_FILENAME, SDF_FILENAME)
# DH
for parameters in DH_CONVERSION_PARAMETERS:
    generateDH(URDF_FILENAME, parameters, BUILD_DIRECTORY)

# COPY FILES IN PROPER FOLDERS
# RViz
if os.path.exists(RVIZ_DIRECTORY):
    shutil.rmtree(RVIZ_DIRECTORY)
os.mkdir(RVIZ_DIRECTORY)
shutil.copy2(os.path.join(BUILD_DIRECTORY, URDF_FILENAME), RVIZ_DIRECTORY)
rviz_mesh_directory = os.path.join(RVIZ_DIRECTORY, MESH_PATH_IN_YAML_FILE)
shutil.copytree(RESOURCES_MESH_DIRECTORY, rviz_mesh_directory)
addWarningReadme(RVIZ_DIRECTORY)
# Gazebo
if os.path.exists(GAZEBO_DIRECTORY):
    shutil.rmtree(GAZEBO_DIRECTORY)
os.mkdir(GAZEBO_DIRECTORY)
gazebo_mesh_directory = os.path.join(GAZEBO_DIRECTORY, MESH_PATH_IN_YAML_FILE)
gazebo_configuration_directory = os.path.join(GAZEBO_DIRECTORY, CONFIGURATION_PATH_IN_YAML_FILE)
shutil.copytree(RESOURCES_MESH_DIRECTORY, gazebo_mesh_directory)
os.mkdir(gazebo_configuration_directory)
copyFolderFiles(RESOURCES_CONFIGURATION_DIRECTORY, gazebo_configuration_directory)
shutil.copy2(GAZEBO_CONFIGURATION_FILE_PATH, GAZEBO_DIRECTORY)
shutil.copy2(os.path.join(BUILD_DIRECTORY, SDF_FILENAME), GAZEBO_DIRECTORY)
addWarningReadme(GAZEBO_DIRECTORY)
# DH
if os.path.exists(DH_DIRECTORY):
    shutil.rmtree(DH_DIRECTORY)
os.mkdir(DH_DIRECTORY)
copyMultipleFilesWithExtension(BUILD_DIRECTORY, DH_DIRECTORY, '.ini')
addWarningReadme(DH_DIRECTORY)

# CLEAN BUILD DIRECTORY
shutil.rmtree(BUILD_DIRECTORY)