cer-sim
=======


gazebo
------

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



RViz
----

Init and build the catkin workspace:

```
cd <cer-sim>/catkin_ws/src
catkin_init_workspace
cd <cer-sim>/catkin_ws
catkin_make
```

Add to .bashrc

```bash
if [ -f <cer-sim>/catkin-ws/devel/setup.bash ]; then
  . ~/Workspace/catkin-ws/devel/setup.bash
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
roslaunch cer-rviz cer.launch
```

geomagic-marker
---------------

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
