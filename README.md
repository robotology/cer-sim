cer-sim
=======


gazebo
------

Install YARP, gazebo, and gazebo_yarp_plugins

Add CER models to the GAZEBO_MODEL_PATH.

```
export GAZEBO_MODEL_PATH=<cer-sim>/cer-gazebo
```


RViz
----

Init and build the catkin workspace


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



Start roscore

```
roscore
```

Start yarp server

```
yarp server --ros
```

start rviz and robot-state-publisher with

```
roslaunch cer-rviz cer.launch
```

geomagic-marker
---------------
