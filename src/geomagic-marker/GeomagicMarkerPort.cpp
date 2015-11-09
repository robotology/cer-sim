/*
 * Copyright (C) 2015  iCub Facility, Istituto Italiano di Tecnologia
 * Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include "GeomagicMarkerPort.h"
#include "visualization_msgs/Marker.h"
#include "tf2_msgs/TFMessage.h"

#include <yarp/os/Stamp.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Publisher.h>

#include <cmath>


void GeomagicMarkerPort::onRead(yarp::os::Bottle &b)
{
    if (b.size() != 6) {
        yDebug() << "Wrong bottle size";
        return;
    }

    yarp::os::Stamp stamp;
    getEnvelope(stamp);
    if(!stamp.isValid()) {
        stamp.update();
    }
    int sec = (int)stamp.getTime();
    int nsec = (int)((stamp.getTime() - ts.header.stamp.sec) * 1e9);

    double x     = b.get(0).asDouble();
    double y     = b.get(1).asDouble();
    double z     = b.get(2).asDouble();
    double roll  = b.get(3).asDouble()*(M_PI/180);
    double pitch = b.get(4).asDouble()*(M_PI/180);
    double yaw   = b.get(5).asDouble()*(M_PI/180);
    double qx = (sin(roll/2) * cos(pitch/2) * cos(yaw/2) - cos(roll/2) * sin(pitch/2) * sin(yaw/2));
    double qy = (cos(roll/2) * sin(pitch/2) * cos(yaw/2) + sin(roll/2) * cos(pitch/2) * sin(yaw/2));
    double qz = (cos(roll/2) * cos(pitch/2) * sin(yaw/2) - sin(roll/2) * sin(pitch/2) * cos(yaw/2));
    double qw = (cos(roll/2) * cos(pitch/2) * cos(yaw/2) + sin(roll/2) * sin(pitch/2) * sin(yaw/2));

    tf2_msgs_TFMessage tf;
    geometry_msgs_TransformStamped ts;
    ts.header.frame_id = "/torso_chest";
    ts.header.stamp.sec = sec;
    ts.header.stamp.nsec = nsec;

    ts.child_frame_id = "/geomagic";
    ts.transform.translation.x = x;
    ts.transform.translation.y = y;
    ts.transform.translation.z = z;
    ts.transform.rotation.x = qx;
    ts.transform.rotation.y = qy;
    ts.transform.rotation.z = qz;
    ts.transform.rotation.w = qw;

    tf.transforms.push_back(ts);
    portOutTF->write(tf);

    visualization_msgs_Marker marker;
    marker.header.frame_id = "/geomagic";

    marker.header.stamp.sec = sec;
    marker.header.stamp.nsec = nsec;

    marker.lifetime.sec = 60;
    marker.lifetime.nsec = 0;

    marker.ns = "geomagic";

    marker.type = visualization_msgs_Marker::SPHERE;
    marker.action = visualization_msgs_Marker::ADD;
    marker.id = 0;

    marker.pose.position.x = 0;
    marker.pose.position.y = 0;
    marker.pose.position.z = 0;

    marker.pose.orientation.x = 0.0;
    marker.pose.orientation.y = 0.0;
    marker.pose.orientation.z = 0.0;
    marker.pose.orientation.w = 1.0;

    marker.scale.x = 0.1;
    marker.scale.y = 0.1;
    marker.scale.z = 0.1;

    marker.color.r = 1.0f;
    marker.color.g = 0.5f;
    marker.color.b = 0.5f;
    marker.color.a = 0.7f;

    portOut->write(marker);
}
