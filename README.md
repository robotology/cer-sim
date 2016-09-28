R1 model generator
=================
This repository contains the resources and scripts to generate the models and the workspaces for Rviz, MoveIt, Gazebo and iKin.
The models (URDF, SDF and DH parameters) are generated using a custom python script and copied inside of the proper folders.

Table of Contents
-----------------
  * [Dependencies](#dependencies)
  * [Using the models](#using-the-models)
  * [Updating the models](#update)

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
export GAZEBO_MODEL_PATH=<cer-sim>/cer-gazebo
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
if [ -f <cer-sim>/ ]; then
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