

#include "ros/ros.h"
#include "gazebo_msgs/GetModelState.h"
#include "assignment1_setup/Sonars.h"
#include <cmath>
#include <iostream>
#include <cstdlib>

//#include "assignment1_setup/sonar_wrapper.h"
#include "geometry_msgs/Twist.h"
#include "geometry_msgs/Pose.h"

#include "assignment1_setup/pid.h"
#include "assignment1_setup/sonar_values.h"
#include "assignment1_setup/kalman_filter_values.h"


using namespace std;

//roslaunch turtlebot3_gazebo turtlebot3_empty_world.launch
uint16_t variance = 1000;

float previousrobotx = 0;
float previousroboty = 0;


uint16_t previousd0v = 0;
uint16_t previousd1v = 0;
uint16_t previousd2v = 0;



uint16_t distance0 = 65535;
uint16_t distance1 = 65535;
uint16_t distance2 = 65535;

//time
uint16_t delay = 10;
//et1 is distance before current
uint16_t et1 = 0;
float ft1 = 0;


float eudidian_distance(float x, float y,float x2, float y2){
	
	return sqrt(((x-x2)*(x-x2))+((y-y2)*(y-y2)));
}

void chatterCallback(const assignment1_setup::Sonars::ConstPtr& msg)
{  //int i = 0;
    distance0 = msg->distance0;
    distance1 = msg->distance1;
    distance2 = msg->distance2;
    
    //ROS_INFO("I heard: [%s]", msg->data.c_str());
    
    
}

int main(int argc, char **argv) {
	//Initiated Node 
	ros::init(argc, argv, "controller");
	ros::NodeHandle n;
	ros::Rate rate(10);
	
	//services
	
	//client for sonar wrapper
	ros::ServiceClient client = n.serviceClient<assignment1_setup::sonar_values>("sonar_wrapper");
	assignment1_setup::sonar_values srv;
	
	//client for pid algorithmn
	ros::ServiceClient client2 = n.serviceClient<assignment1_setup::pid>("pid_algo");
	assignment1_setup::pid srv2;
	
	//client for kalman filter	
	ros::ServiceClient client3 = n.serviceClient<assignment1_setup::kalman_filter_values>("kalman_filter");
	assignment1_setup::kalman_filter_values srv3;
	
	//client for Gazebo model state
	ros::ServiceClient client4 = n.serviceClient<gazebo_msgs::GetModelState>("/gazebo/get_model_state");
	gazebo_msgs::GetModelState srv4;
	
	
	 	
	//rosservice call /gazebo/get_model_state "model_name: 'turtlebot3_burger'"
	

	//publishers
	
	//publishing movement to gazebo robot via cmd_vel topic
	ros::Publisher control = n.advertise<geometry_msgs::Twist>("/cmd_vel",1000);
	geometry_msgs::Twist vel_msg{};		
	
	//init stuff
	srv4.request.model_name = "turtlebot3_burger";
	client4.call(srv4);
	previousrobotx = (float)srv4.response.pose.position.x;
	previousroboty = (float)srv4.response.pose.position.y;
	
	uint16_t measuredvairanace = 0;
	uint16_t allvalues[100];
	ROS_INFO("Controller Initiated. Moving towards robot");
	while (ros::ok() && measuredvairanace <= 100) {
		client.call(srv);
		distance1 = srv.response.distance1;
		//ROS_INFO("distance %u",distance1);	
		if(distance1 < 65535){
			ROS_INFO("recording %u  ->val %u",measuredvairanace,distance1);
			vel_msg.angular.z = 0;						
			allvalues[measuredvairanace] = distance1;
			measuredvairanace += 1;
			
		}else{
			
			vel_msg.angular.z = 0.5;
		}
		control.publish(vel_msg);
		vel_msg = geometry_msgs::Twist{};
		ros::spinOnce();
		rate.sleep();
	}
	
	//find vairance
	
	//average	
	double all = 0;
	for(int64_t i =0; i < 100; i++ ){
		all += allvalues[i];
	} 
	all = all/100;
	
	//sums of squares
	double sums = 0;
	for(int64_t i =0; i < 100; i++ ){
		sums += (allvalues[i] - all) * (allvalues[i] - all);
	}
	
	sums = sums/(100-1);
	variance = (uint16_t)sums;
	
	ROS_INFO("Variance %u",variance);
	
	while (ros::ok()) {
	
		
		//service call to modelstate getting position
		
		
		//we have to make three different calls of each of the distance sensors (Because they are all noisy)
		
		//calling sensor wrapper
		client.call(srv);
		//defining the bot and getting its coords
		srv4.request.model_name = "turtlebot3_burger";
		client4.call(srv4);
				
	
		
		//cleaning distance1 and sending it to kalman filter	
		srv3.request.ri = variance;
		srv3.request.zi = srv.response.distance1;
		srv3.request.yid = abs(srv.response.distance1 - eudidian_distance(srv4.response.pose.position.x, srv4.response.pose.position.y,previousrobotx,previousroboty));	
		srv3.request.pid = previousd1v;
		client3.call(srv3);
		//getting fixed kalman filter values and previous varaiance
		previousd1v = (float)srv3.response.pi;
		distance1 = srv3.response.yi;
		

		
		
		//after run
		previousrobotx = (float)srv4.response.pose.position.x;
		previousroboty = (float)srv4.response.pose.position.y;
		
		
		
		
		distance0 = srv.response.distance0;
		distance2 = srv.response.distance2;
		//get sonar values task1 (old)
		/*
		
		distance1 = srv.response.distance1;
		
		client.call(srv);
		*/
		ROS_INFO("Sonar values 0[%d] 1[%d] 2[%d]",distance0,distance1,distance2);
		//logic of rotation 
		
		
		if(distance1 <= (65535 -variance)){
			ROS_INFO("Going Forwards");
			//bowl is infront on robot go forward and stop rotating
			vel_msg.angular.z = 0;
			//specify values in request
			srv2.request.delay = delay;
			srv2.request.before_distance1 = et1;
			srv2.request.distance1 = distance1;
			srv2.request.ft1 = ft1;

			//make request to service node
			client2.call(srv2);
			//upon receieving response make changes to publisher
			vel_msg.linear.x = srv2.response.acceleration; 
			ROS_INFO("I accel [%f]",  srv2.response.acceleration);
			//ensure that previous values are recorded
			ft1 = srv2.response.ft;
			et1 = distance1;
		  
			 
		  
		}else if(distance0 <= (65535 -variance)){
			vel_msg.linear.x = 0;
			vel_msg.angular.z = 0.5;
		}else{
			vel_msg.linear.x = 0;
			vel_msg.angular.z = -0.5;
		}

		control.publish(vel_msg);
		vel_msg = geometry_msgs::Twist{};
		ros::spinOnce();
		rate.sleep();
	}
}

