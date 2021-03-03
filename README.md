R1 model generator
=================
This repository contains the resources and scripts to generate the models and the workspaces for Rviz, MoveIt, Gazebo and iKin.
The models (URDF, SDF and DH parameters) are generated using a custom python script and copied inside of the proper folders.

Table of Contents
-----------------
  * [Dependencies](#dependencies)
  * [Using the models](#using-the-models)
  * [Updating the models](#updating-the-models)
  * [Adding the checkerboard](#adding-the-checkerboard)

### Dependencies
The following dependencies are required to use the models:
- [YARP](http://www.yarp.it/)
- [gazebo-yarp-plugins](https://github.com/robotology/gazebo-yarp-plugins)
- [Gazebo](http://gazebosim.org/)
- [RViz](http://wiki.ros.org/rviz)
- [MoveIt](http://moveit.ros.org)
- [iKin](https://github.com/robotology/icub-main)

The following dependencies are required to generate and update the models:
- [Python](https://www.python.org/)
- [Gazebo](http://gazebosim.org/)
- [simmechanics-to-urdf](https://github.com/robotology/simmechanics-to-urdf)
- [iDynTree](https://github.com/robotology/idyntree)

### Using the models
##### Gazebo
Install YARP, gazebo, and gazebo_yarp_plugins

Add CER models to the GAZEBO_MODEL_PATH:
```
export GAZEBO_MODEL_PATH=<cer-sim>/gazebo
```
Start YARP server
```
yarp server
```
Start gazebo
```
gazebo
```
Drag the CER model into the scene.

##### RViz
Init and build the catkin workspace:
```
cd <cer-sim>/catkin_ws/src
catkin_init_workspace
cd <cer-sim>/catkin_ws
catkin_make
```
Add to .bashrc
```bash
if [ -f <cer-sim>/catkin_ws/devel/setup.bash ]; then
  . <cer-sim>/catkin_ws/devel/setup.bash
fi
```
Start roscore:
```
roscore
```
Start yarp server:
```
yarp server --ros
```
Start rviz and robot-state-publisher:
```
roslaunch cer_rviz cer.launch
```

##### geomagic-marker
Build:
```
cd <cer-sim>/src/geomagic-marker
mkdir build
cd build
ccmake ..
make
```
Start the module
```
<cer-sim>/src/geomagic-marker/build/geomagic-marker
```

### Updating the models
The proper procedure to update the models is to modify the files inside of the [resources](resources/) folder. 
Subsequently, run the python script `generate_models.py`. 

**Any local change to the models files will be lost**.

### Adding the checkerboard
The checkerboard is a board with a pattern of white and black squares ("chequered pattern"). The geometry file (*.dae) in the [mesh](resources/mesh/dae/) folder represents a rectangular checkerboard with a 7 by 9 pattern.
To add the checkerboard to the R1 model simply add the following code to the *.sdf file in the gazebo folder.
Since in normal conditions the checkerboard is not necessary, the modified sdf model shouldn't be committed and changes are lost every time the model is  [regenerated](#Updating-the-models)

```
    <link name='checkerboard_link'>
      <pose frame=''>VALUE_1 0.0 VALUE_2 1.570795 0.0 3.14159</pose>
      <visual name='checkerboard_link_visual'>
        <pose frame=''>0 0 0 0 0 0</pose>
        <geometry>
          <mesh>
            <scale>0.001 0.001 0.001</scale>
            <uri>model://cer/meshes/dae/checkerboard.dae</uri>
          </mesh>
        </geometry>
      </visual>
    </link>
    <joint name='checkerboard_joint' type='fixed'>
      <child>checkerboard_link</child>
      <parent>mobile_base_body_link</parent>
    </joint>
```

Where we have:

VALUE_1 = B + 0.1779

VALUE_2 = A + 0.2585

As shown in Figure 1, A is the distance between the terrain and the lower border of the white and black squares. While B represents the distance between the black and white squares front surfaces and the border of the covers of R1. The border of the covers can be easily accessed by extending the torso joint of the robot.

![Figure 1](/resources/distances.jpg?raw=true "Figure 1")
