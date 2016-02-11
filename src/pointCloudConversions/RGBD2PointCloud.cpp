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
    flip_h = 1;
    frame_id = "/camera_link";
    scaleFactor = 1;
}

RGBD2PointCloud::~RGBD2PointCloud()
{

}

bool RGBD2PointCloud::configure(ResourceFinder& rf)
{
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

    roll  = rf.check("roll",  Value(0)).asDouble();
    pitch = rf.check("pitch", Value(0)).asDouble();
    yaw   = rf.check("yaw",   Value(0)).asDouble();
    frame_id = rf.check("frame_id",   Value("/camera_link")).asString();

    if(rf.check("flip_h"))
        flip_h= -1;

    scaleFactor = rf.check("scale",   Value(1)).asDouble();

    return true;
}

double RGBD2PointCloud::getPeriod()
{
    return 0.1;
}

bool RGBD2PointCloud::updateModule()
{
    yInfo() << "RGBD2PointCloud is running fine";

    bool ret = convertRGBD_2_XYZRGB();
    ros::spinOnce (); //@@@@@@@@@@
    return ret;
}


bool RGBD2PointCloud::convertRGBD_2_XYZRGB()
{

//    colorImage = imageFrame_inputPort.read();
//    depthImage = depthFrame_inputPort.read();
//    int d_width  = depthImage->width();
//    int d_height = depthImage->height();
//    int c_width  = colorImage->width();
//    int c_height = colorImage->height();


    bool ret = imageFrame_inputPort.read(colorImage);
    ret &= depthFrame_inputPort.read(depthImage);

    if(!ret)
    {
        yError() << "Cannot read from ports";
    }

    int d_width  = depthImage.width();
    int d_height = depthImage.height();
    int c_width  = colorImage.width();
    int c_height = colorImage.height();


    if( (d_width != c_width) && (d_height != c_height) )
    {
        yError() << "Size does not match";
        return true;
    }

    width  = d_width;
    height = d_height;

    sensor_msgs::PointCloud2Modifier pcd_modifier(point_cloud_msg);
    pcd_modifier.setPointCloud2FieldsByString(2, "xyz", "rgb");
    // convert to flat array shape, we need to reconvert later
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
//    float* colorDataRaw = (float*)colorImage->getRawImage();
//    float* depthDataRaw = (float*)depthImage->getRawImage();

    unsigned char* colorDataRaw = (unsigned char*)colorImage.getRawImage();
    float* depthDataRaw = (float*)depthImage.getRawImage();

    double hfov = 58 * 3.14 / 360;

    double fl = ((double)this->width) / (2.0 *tan(hfov/2.0));

    double fovHorizontalDeg = 58.62;
    double fovVerticalDeg = 45.666;
    double halfFovHorizontalRad = fovHorizontalDeg*M_PI/360.0;
    double halfFovVerticalRad = fovVerticalDeg*M_PI/360.0;
    double stepHorizontal = 2.0*tan(halfFovHorizontalRad)/((double) width);
    double stepVertical = 2.0*tan(halfFovVerticalRad)/((double) height);
    double startHorizontal = -tan(halfFovHorizontalRad);
    double startVertical = tan(halfFovVerticalRad);

    // convert depth to point cloud
    for (uint32_t j=0; j<height; j++)
    {
        for (uint32_t i=0; i<width; i++, ++iter_x, ++iter_y, ++iter_z, ++iter_rgb)
        {
            double depth = depthDataRaw[index++] * scaleFactor;

            *iter_x = (float) depth *(startHorizontal + ((float) j)*stepHorizontal);
            *iter_y = (float) depth *(startVertical - ((float) i)*stepVertical);
            *iter_z = depth;

            // color source for asus xtion is bgr (can be set/get somehow?)
            iter_rgb[2] = (uint8_t) colorDataRaw[i*3+j*width*3+0];
            iter_rgb[1] = (uint8_t) colorDataRaw[i*3+j*width*3+1];
            iter_rgb[0] = (uint8_t) colorDataRaw[i*3+j*width*3+2];
        }
    }

    // reconvert to original height and width after the flat reshape
    point_cloud_msg.height = height;
    point_cloud_msg.width = width;

    pcloud_outTopic.publish (point_cloud_msg);

    if (frame_id.compare("/base_link") != 0)
    {
        tf::StampedTransform camera_base_tf(
                    tf::Transform(tf::createQuaternionFromRPY(roll, pitch, yaw),
                    tf::Vector3(0.0,0.0,1.0)),ros::Time::now(),
                    "/base_link", frame_id);
        tf_broadcaster->sendTransform(camera_base_tf);
    }
    else
    {
        tf::StampedTransform camera_base_tf(
                    tf::Transform(tf::createQuaternionFromRPY(0.0, 0.0, 0.0),
                    tf::Vector3(0.0,0.0,0.0)),ros::Time::now(),
                    "/base_link", frame_id);
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

