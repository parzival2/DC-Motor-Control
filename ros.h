#ifndef ROS_H
#define ROS_H

#include "ChibiOSHardware.h"
#include "ros_lib/ros/node_handle.h"

#ifdef __cplusplus
extern "C" {
#endif

namespace ros
{
	typedef NodeHandle_<ChibiOSHardware, 25, 25, 512, 512> NodeHandle;
}

#ifdef __cplusplus
}
#endif

#endif /* ROS_H */
