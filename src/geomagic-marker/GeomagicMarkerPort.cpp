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

    visualization_msgs_Marker marker;
    marker.header.frame_id = "/torso_chest";

    yarp::os::Stamp stamp;
    getEnvelope(stamp);
    if(!stamp.isValid()) {
        stamp.update();
    }
    marker.header.stamp.sec = (int)stamp.getTime();
    marker.header.stamp.nsec = 0; //FIXME

    marker.lifetime.sec = 14; // FIXME
    marker.lifetime.nsec = 0; //FIXME

    marker.ns = "geomagic";

    marker.type = visualization_msgs_Marker::SPHERE;
    marker.action = visualization_msgs_Marker::ADD;
    marker.id = 0;

    marker.pose.position.x = x;
    marker.pose.position.y = y;
    marker.pose.position.z = z;

    marker.pose.orientation.x = qx;
    marker.pose.orientation.y = qy;
    marker.pose.orientation.z = qz;
    marker.pose.orientation.w = qw;

    marker.scale.x = 0.15;
    marker.scale.y = 0.10;
    marker.scale.z = 0.05;

    marker.color.r = 0.5f;
    marker.color.g = 0.5f;
    marker.color.b = 0.5f;
    marker.color.a = 0.7f;

    portOut->write(marker);

    tf2_msgs_TFMessage tf;
    geometry_msgs_TransformStamped ts;
    ts.header.frame_id = "/torso_chest";
    ts.header.stamp.sec = (int)stamp.getTime();
    ts.header.stamp.nsec = 0; //FIXME

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
}
