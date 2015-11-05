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




Launch with
```
roslaunch cer-rviz cer.launch
```

(NOTE: Probably something is still missing)

