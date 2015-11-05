/*
 * Copyright (C) 2015  iCub Facility, Istituto Italiano di Tecnologia
 * Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include "GeomagicMarkerModule.h"

#include <yarp/os/LogStream.h>

GeomagicMarkerModule::GeomagicMarkerModule() :
        yarp::os::RFModule(),
        node("/geomagic/marker")
{
}

double GeomagicMarkerModule::getPeriod()
{
    return 60;
}

bool GeomagicMarkerModule::configure(yarp::os::ResourceFinder &rf)
{
    if (!portOut.topic("/geomagic/marker:o")) {
        std::cerr << "Error opening output port, check your ros/yarp network" << std::endl;
        return false;
    }

    if (!portOutTF.topic("/tf")) {
        std::cerr << "Error opening output port, check your ros/yarp network" << std::endl;
        return false;
    }

    if (!portIn.open("/geomagic/marker:i")) {
        std::cerr << "Error opening input port, check your yarp network" << std::endl;
        return false;
    }
    portIn.portOut = &portOut;
    portIn.portOutTF = &portOutTF;
    portIn.useCallback();
    return true;
}

bool GeomagicMarkerModule::updateModule()
{
    yDebug() << "geomagic-marker running happily";
    return true;
}
