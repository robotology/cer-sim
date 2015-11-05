/*
 * Copyright (C) 2015  iCub Facility, Istituto Italiano di Tecnologia
 * Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef _GeomagicMarkerPort_h_
#define _GeomagicMarkerPort_h_

#include <yarp/os/BufferedPort.h>
#include <yarp/os/Bottle.h>

namespace yarp { namespace os { template <class T> class Publisher; }}
class visualization_msgs_Marker;
class tf2_msgs_TFMessage;

class GeomagicMarkerPort : public yarp::os::BufferedPort<yarp::os::Bottle>
{
protected:
    virtual void onRead(yarp::os::Bottle &b);
public:
    yarp::os::Publisher<visualization_msgs_Marker> *portOut;
    yarp::os::Publisher<tf2_msgs_TFMessage> *portOutTF;
};

#endif // _GeomagicMarkerPort_h_
