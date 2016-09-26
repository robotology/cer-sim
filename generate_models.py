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

# Directories
RESOURCES_DIRECTORY = os.path.join(workingDirectory, 'resources')
GAZEBO_DIRECTORY = os.path.join(workingDirectory, 'gazebo')
RVIZ_DIRECTORY = os.path.join(workingDirectory, 'catkin_ws', 'src', 'cer_rviz', 'urdf') 
DH_DIRECTORY = os.path.join(workingDirectory, 'dh_parameters') 
DAE_MESH_DIRECTORY = os.path.join(RESOURCES_DIRECTORY, 'mesh', 'dae')
STL_MESH_DIRECTORY = os.path.join(RESOURCES_DIRECTORY, 'mesh', 'stl')

# SimMechanicsToURDF parameters
SIMMECHANICS_XML = "SIM_CER_ROBOT.xml"
YAML_FILENAME = "cer_options.yaml"
CSV_JOINT_FILENAME = "cer_joint_parameters.csv"
OUTPUT_TYPE = "xml"
URDF_FILENAME = "cer.urdf"

# DH conversion parameters

# RvIZ parameters
RVIZ_MESH_PATH_TOCKEN = 'package://'

# Other parameters
XML_DECLARATION = "<?xml version=""1.0"" ?>"



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
    
# Checking dependencies
for dependency in DEPENDENCIES:
    checkDependency(dependency)

urdf = generateURDF(RESOURCES_DIRECTORY, SIMMECHANICS_XML, YAML_FILENAME, CSV_JOINT_FILENAME, OUTPUT_TYPE)
urdf = XML_DECLARATION + os.linesep + urdf

# Remove old files
if os.path.exists(RVIZ_DIRECTORY):
    shutil.rmtree(RVIZ_DIRECTORY)
shutil.copytree(DAE_MESH_DIRECTORY, RVIZ_DIRECTORY)
        
rviz_urdf = urdf.replace('model://', RVIZ_MESH_PATH_TOCKEN)
rviz_urdf_filepath = RVIZ_DIRECTORY + os.sep + URDF_FILENAME
urdf_file = open(rviz_urdf_filepath,"w")
urdf_file.write(rviz_urdf)
urdf_file.close()


ciao = 2