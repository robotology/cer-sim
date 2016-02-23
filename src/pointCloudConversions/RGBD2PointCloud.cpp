#include <iostream>
#include <yarp/os/all.h>
#include <yarp/dev/all.h>
#include <yarp/sig/all.h>
#include <yarp/math/Math.h>
#include <yarp/conf/system.h>
#include <yarp/sig/Image.h>
#include <yarp/sig/Vector.h>

#include <ros/init.h>
#include "RGBD2PointCloud.hpp"

using namespace std;
using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;
using namespace yarp::math;

RGBD2PointCloud::RGBD2PointCloud()
{
    use_RGBD_client = false;
    width           = 0;
    height          = 0;
    point_cloud_msg.header.seq = 0;
    frame_id = "/camera_link";
    scaleFactor = 1;
    mirrorX = 1;
    mirrorY = 1;
    mirrorZ = 1;
    publishTF = false;
}

RGBD2PointCloud::~RGBD2PointCloud()
{

}

bool RGBD2PointCloud::configure(ResourceFinder& rf)
{
    if(rf.check("help"))
    {
        yInfo() << " Required parameters:";
        yInfo() << "\tremoteImagePort: remote port streaming rgb images";
        yInfo() << "\tremoteDepthPort: remote port streaming depth images";
        yInfo() << " Optional parameters:";
        yInfo() << "\tscale: scale factor to apply to depth data. For Gazebo has to be 1, for xtion sensor 0.001 in order to scale data to [m]";
        yInfo() << "\tmirrorX, mirrorY, mirrorZ: add it to mirror the resulting point cloud on the corresponding axes (no param, just '--mirrorx')";
        yInfo() << "\ttf: if present a tf will be published. This requires 7 parameters";
        yInfo() << "\t  reference frame name,\n\t  x,y,z translations [m], \n\t  roll, pitch, yaw rotations [rad]\n";
        yInfo() << "\t  For example --tf base_link 0.1 0.0 0.0   1.56 0.0 0.0   -- no parenthesis";
        return false;
    }


    if(!rf.check("remoteImagePort") || !rf.check("remoteDepthPort"))
    {
        yError() << "Missing required parameters: remoteImagePort or remoteDepthPort";
        return false;
    }

    int ciao = 0;
    ros::init(ciao, NULL, "my_node_name");
    ros::Time::init();
    rosNode         = new ros::NodeHandle();
    pcloud_outTopic = rosNode->advertise<sensor_msgs::PointCloud2 >("/RGBD2PointCloud/pcloud_out", 1);
    tf_broadcaster  = new tf::TransformBroadcaster;

    yarp::os::ConstString remoteImagePort_name = rf.find("remoteImagePort").asString();
    yarp::os::ConstString remoteDepthPort_name = rf.find("remoteDepthPort").asString();

    // cannot use because OpenNI2DeviceServer does not uses RGBDSensorWrapper yet
    if(use_RGBD_client)
    {
        yarp::os::Property params;
        params.put("device", "RGBDSensorClient");
        params.put("localImagePort","/RGBD2PointCloud/rgb/in:i");
        params.put("localDepthPort","/RGBD2PointCloud/depth/in:i");
        params.put("remoteImagePort",remoteImagePort_name);
        params.put("remoteDepthPort",remoteDepthPort_name);
        params.put("watchdog","100");
        params.put("synchPolicy","latest");

        if(!RGBD_Client.open(params))
        {
            yError() << "Cannot open device";
            return false;
        }
    }
    else
    {
        bool ret = true;
        ret &= imageFrame_inputPort.open("/RGBD2PointCloud/rgb/in:i");
        ret &= depthFrame_inputPort.open("/RGBD2PointCloud/depth/in:i");

        ret &= pointCloud_outputPort.open("/RGBD2PointCloud/pointCloud/out:o");

        if(!ret)
        {
            yError() << "Cannot open required ports";
            return false;
        }

        ret &= yarp::os::Network::connect(remoteImagePort_name, "/RGBD2PointCloud/rgb/in:i");
        ret &= yarp::os::Network::connect(remoteDepthPort_name, "/RGBD2PointCloud/depth/in:i");

        if(!ret)
        {
            yError() << "Cannot connect to remote ports";
            return false;
        }
        else
            yInfo() << "Connection is done!";
    }

    Bottle tf = rf.findGroup("tf");
    if(!tf.isNull() )
    {
        if(tf.size() != 1+7)
        {
            yError() << "TF parameter must have 7 elements: reference frame name, x,y,z [m] translations, roll, pitch, yaw rotations [rad]\n" \
            "For example --tf base_link 0.1 0.0 0.0     1.56 0.0 0.0   -- no parenthesis";
            return false;
        }

        translation.resize(3);
        rotation.resize(3);
        rotation_frame_id   = tf.get(1).asString();
        translation[0]      = tf.get(2).asDouble();
        translation[1]      = tf.get(3).asDouble();
        translation[2]      = tf.get(4).asDouble();

        rotation[0]         = tf.get(5).asDouble();
        rotation[1]         = tf.get(6).asDouble();
        rotation[2]         = tf.get(7).asDouble();

    }

    frame_id = rf.check("frame_id",   Value("/camera_link")).asString();
    rf.check("mirrorX") ? mirrorX = -1 : mirrorX = 1;
    rf.check("mirrorY") ? mirrorY = -1 : mirrorY = 1;
    rf.check("mirrorZ") ? mirrorZ = -1 : mirrorZ = 1;

    yInfo() << "Mirrors: x=" << mirrorX << " y=" << mirrorY << " z= " << mirrorZ;
    yInfo() << "Frame id: " << frame_id;
    scaleFactor = rf.check("scale",   Value(1)).asDouble();

    return true;
}

double RGBD2PointCloud::getPeriod()
{
    return 0.3;
}

bool RGBD2PointCloud::updateModule()
{
    yInfo() << "RGBD2PointCloud is running fine";

    bool ret = convertRGBD_2_XYZRGB();
    ros::spinOnce ();
    return ret;
}


bool RGBD2PointCloud::convertRGBD_2_XYZRGB()
{
    bool ret = imageFrame_inputPort.read(colorImage);
    ret &= depthFrame_inputPort.read(depthImage);

    if(!ret)
    {
        yError() << "Cannot read from ports";
        return false;
    }

    int d_width  = depthImage.width();
    int d_height = depthImage.height();
    int c_width  = colorImage.width();
    int c_height = colorImage.height();


    if( (d_width != c_width) && (d_height != c_height) )
    {
        yError() << "Size does not match";
        return false;
    }

    width  = d_width;
    height = d_height;

    sensor_msgs::PointCloud2Modifier pcd_modifier(point_cloud_msg);
    pcd_modifier.setPointCloud2FieldsByString(2, "xyz", "rgb");
    pcd_modifier.resize(width * height);
    point_cloud_msg.is_dense = false;

    point_cloud_msg.header.seq++;
    point_cloud_msg.width  = width;
    point_cloud_msg.height = height;
    point_cloud_msg.header.frame_id = frame_id;
    point_cloud_msg.header.stamp = ros::Time::now();

    point_cloud_msg.row_step = point_cloud_msg.point_step * width;

    sensor_msgs::PointCloud2Iterator<float> iter_x(point_cloud_msg, "x");
    sensor_msgs::PointCloud2Iterator<float> iter_y(point_cloud_msg, "y");
    sensor_msgs::PointCloud2Iterator<float> iter_z(point_cloud_msg, "z");
    sensor_msgs::PointCloud2Iterator<uint8_t> iter_rgb(point_cloud_msg, "rgb");

    int index = 0;

    unsigned char* colorDataRaw = (unsigned char*)colorImage.getRawImage();
    float* depthDataRaw = (float*)depthImage.getRawImage();

    double hfov = 58 * 3.14 / 360;

    double fl = ((double)this->width) / (2.0 *tan(hfov/2.0));

    double fovHorizontalDeg = 58.62;
    double halfFovHorizontalRad = fovHorizontalDeg*M_PI/360.0;
//     double fovVerticalDeg = 45.666;
//     double halfFovVerticalRad = fovVerticalDeg*M_PI/360.0;

    double f=(width/2)/sin(halfFovHorizontalRad)*cos(halfFovHorizontalRad); // / sqrt(2);

    // convert depth to point cloud
    for (int32_t j=0; j<height; j++)
    {
        for (int32_t i=0; i<width; i++, ++iter_x, ++iter_y, ++iter_z, ++iter_rgb)
        {
            double depth = depthDataRaw[index++] * scaleFactor;

            double u = -(i - 0.5*(width-1));
            double v = (0.5*(height-1) -j);
            //gazebo
            *iter_x = (float) depth;
            *iter_y = (float) *iter_x * u/f;
            *iter_z = (float) *iter_x * v/f;
            // end gazebo

            int new_i;
            if( (*iter_x) > 0)
                new_i = (i + (int) ( (0.03 *f/(*iter_x)) +0.5) );
            else
                new_i = i;

            if( (new_i >= width) || (new_i < 0))
            {
                iter_rgb[2] = 0;
                iter_rgb[1] = 0;
                iter_rgb[0] = 0;
            }
            else
            {
                iter_rgb[2] = (uint8_t) colorDataRaw[(j*width*3) + new_i*3 +0];
                iter_rgb[1] = (uint8_t) colorDataRaw[(j*width*3) + new_i*3 +1];
                iter_rgb[0] = (uint8_t) colorDataRaw[(j*width*3) + new_i*3 +2];
            }
        }
    }

    // reconvert to original height and width after the flat reshape
    point_cloud_msg.height = height;
    point_cloud_msg.width = width;

    pcloud_outTopic.publish (point_cloud_msg);

    if(publishTF)
    {
        tf::StampedTransform camera_base_tf(
                    tf::Transform(tf::createQuaternionFromRPY(rotation[0], rotation[1], rotation[2]),
                    tf::Vector3(translation[0], translation[1], translation[2])), ros::Time::now(),
                    rotation_frame_id.c_str(), frame_id);
        tf_broadcaster->sendTransform(camera_base_tf);
    }
    return true;
}

bool RGBD2PointCloud::interruptModule()
{
    yDebug() << "Interrupting module";
    imageFrame_inputPort.interrupt();
    depthFrame_inputPort.interrupt();

    imageFrame_inputPort.close();
    depthFrame_inputPort.close();
}

bool RGBD2PointCloud::close()
{
    yTrace();
    interruptModule();
}

