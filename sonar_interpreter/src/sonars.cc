/*
 * sonars.cc
 * Copyright (C) 2021 Morgan McColl <morgan.mccoll@alumni.griffithuni.edu.au>
 *
 * Distributed under terms of the MIT license.
 */

#include "ros/ros.h"
#include "gazebo_msgs/GetModelState.h"
#include "assignment1_setup/Sonars.h"
#include "geometry_msgs/Twist.h"
#include "geometry_msgs/Pose.h"
#include <math.h>
#include <random>
#include <stdint.h>
#include <iostream>
#include <algorithm>

bool isSeen(double angle) {
    return angle <= 45.0 && angle >= -45.0;
}

bool isZero(double angle) {
    return angle <= 45.0 && angle > 15.0;
}

bool isFirst(double angle) {
    return angle <= 15.0 && angle > -15.0;
}

bool isSecond(double angle) {
    return angle <= -15.0 && angle >= -45.0;
}

#ifdef NOISY_SONAR
double addNoise(double value) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<double> d(value, 100.0);
    return d(gen);
}
#endif

assignment1_setup::Sonars setMsg(uint16_t distance0, uint16_t distance1, uint16_t distance2) {
    assignment1_setup::Sonars msg;
    msg.distance0 = distance0;
    msg.distance1 = distance1;
    msg.distance2 = distance2;
    return msg;
}

int main(int argc, char **argv) {
    ros::init(argc, argv, "sonars");
    ros::NodeHandle n;
    ros::Publisher pub = n.advertise<assignment1_setup::Sonars>("sonars", 1000);
    ros::Rate rate(100);
    gazebo_msgs::GetModelState srv;
    srv.request.relative_entity_name = "turtlebot3_burger";
    srv.request.model_name = "bowl";
    ros::ServiceClient locations = n.serviceClient<gazebo_msgs::GetModelState>("/gazebo/get_model_state");
    while (ros::ok()) {
       ros::spinOnce();
       if (!locations.call(srv)) {
           ROS_ERROR("Failed to query model state. Is gazebo running?");
           rate.sleep();
           continue;
       }
       double x = srv.response.pose.position.x;
       double y = srv.response.pose.position.y;
       double angle = atan2(y, x) / 3.1415926536 * 180.0;
       assignment1_setup::Sonars msg;
       if (!isSeen(angle)) {
           msg = setMsg(UINT16_MAX, UINT16_MAX, UINT16_MAX);
           pub.publish(msg);
           rate.sleep();
           continue;
       }
#ifdef NOISY_SONAR
       double centimetres = addNoise(sqrt(x * x + y * y) * 100.0);
#else
       double centimetres = sqrt(x * x + y * y) * 100.0;
#endif
       uint16_t distance = static_cast<uint16_t>(std::min(std::max(centimetres, 0.0), 65535.0));
       if (isZero(angle)) {
           msg = setMsg(distance, UINT16_MAX, UINT16_MAX);
       }
       if (isFirst(angle)) {
           msg = setMsg(UINT16_MAX, distance, UINT16_MAX);
       }
       if (isSecond(angle)) {
           msg = setMsg(UINT16_MAX, UINT16_MAX, distance);
       }
       pub.publish(msg);
       rate.sleep();
    }
    return 0;
}
