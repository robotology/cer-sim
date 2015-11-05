/*
 * Copyright (C) 2015  iCub Facility, Istituto Italiano di Tecnologia
 * Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef _GeomagicMarkerModule_h_
#define _GeomagicMarkerModule_h_

#include <yarp/os/RFModule.h>
#include <yarp/os/Node.h>
#include <yarp/os/Publisher.h>

#include "GeomagicMarkerPort.h"
#include "visualization_msgs/Marker.h"
#include "tf2_msgs/TFMessage.h"


class GeomagicMarkerModule : public yarp::os::RFModule
{
public:
    explicit GeomagicMarkerModule();
    virtual double getPeriod();
    virtual bool configure(yarp::os::ResourceFinder &rf);
    virtual bool updateModule();

private:
    yarp::os::Node node;
    GeomagicMarkerPort portIn;
    yarp::os::Publisher<visualization_msgs_Marker> portOut;
    yarp::os::Publisher<tf2_msgs_TFMessage> portOutTF;
};

#endif // _GeomagicMarkerModule_h_
