#ifndef RGBD_2_POINTCLOUD_H
#define RGBD_2_POINTCLOUD_H

#include <yarp/os/RFModule.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/sig/Image.h>

#include <ros/ros.h>
#include <tf/tf.h>
#include <tf/transform_broadcaster.h>
#include <tf/transform_listener.h>
#include <std_msgs/Float32.h>
#include <std_msgs/String.h>
#include <sensor_msgs/LaserScan.h>
#include <actionlib/client/simple_action_client.h>
#include <nav_msgs/Odometry.h>
#include <geometry_msgs/Twist.h>
#include <geometry_msgs/PolygonStamped.h>
#include <visualization_msgs/Marker.h>

#include <sensor_msgs/PointCloud.h>
#include <sensor_msgs/PointCloud2.h>
#include <sensor_msgs/point_cloud2_iterator.h>


class RGBD2PointCloud : public yarp::os::RFModule
{
private:
    bool use_RGBD_client;
    yarp::dev::PolyDriver                       RGBD_Client;

    int  width, height;
//    yarp::sig::ImageOf<yarp::sig::PixelRgb>    *colorImage;
//    yarp::sig::ImageOf<yarp::sig::PixelFloat>  *depthImage;
//    yarp::os::BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelRgb>   >    imageFrame_inputPort;   // rgb input
//    yarp::os::BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelFloat> >    depthFrame_inputPort;   // depth input

    yarp::sig::ImageOf<yarp::sig::PixelRgb>    colorImage;
    yarp::sig::ImageOf<yarp::sig::PixelFloat>  depthImage;
    yarp::os::Port                             imageFrame_inputPort;   // rgb input
    yarp::os::Port                             depthFrame_inputPort;   // depth input

    yarp::os::BufferedPort<yarp::os::Bottle>   pointCloud_outputPort;

    int mirrorX;
    int mirrorY;
    int mirrorZ;

    double roll;
    double pitch;
    double yaw;
    double scaleFactor;

    // ROS stuff
    std::string frame_id;
    ros::NodeHandle             *rosNode;
    sensor_msgs::PointCloud2    point_cloud_msg;
    ros::Publisher              pcloud_outTopic;
    tf::TransformBroadcaster    *tf_broadcaster;

public:

    RGBD2PointCloud();
    ~RGBD2PointCloud();
    bool convertRGBD_2_XYZRGB();

    bool configure(yarp::os::ResourceFinder &rf);
    bool updateModule();
    double getPeriod();
    bool interruptModule();
    bool close();
};


#endif  //RGBD_2_POINTCLOUD_H
