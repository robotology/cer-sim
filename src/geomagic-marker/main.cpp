/*
 * Copyright (C) 2015  iCub Facility, Istituto Italiano di Tecnologia
 * Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <yarp/os/Network.h>
#include <yarp/os/ResourceFinder.h>
#include <yarp/os/LogStream.h>

#include "GeomagicMarkerModule.h"

int main(int argc, char *argv[])
{
    yarp::os::Network yarp;
    if(!yarp.checkNetwork())
    {
        yFatal() << "YARP network not available";
    }

    yarp::os::ResourceFinder &rf = yarp::os::ResourceFinder::getResourceFinderSingleton();
    rf.setVerbose();
    rf.setDefaultConfigFile("geomagic-marker.ini");
    rf.configure(argc, argv);

    GeomagicMarkerModule module;
    return module.runModule(rf);
}