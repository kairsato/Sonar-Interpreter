#include "ros/ros.h"
#include "assignment1_setup/Sonars.h"
#include "assignment1_setup/sonar_values.h"
//noisy sonars
uint16_t distance0 = 65535;
uint16_t distance1 = 65535;
uint16_t distance2 = 65535;
void chatterCallback(const assignment1_setup::Sonars::ConstPtr& msg){  
	distance0 = msg->distance0;
	distance1 = msg->distance1;
	distance2 = msg->distance2;    
}
bool algo(assignment1_setup::sonar_values::Request &req,assignment1_setup::sonar_values::Response &res){
	res.distance0 = distance0;
	res.distance1 = distance1;
	res.distance2 = distance2;
	ROS_INFO("Returning %d %d %d",distance0,distance1,distance2);
	return true;	
 }
int main(int argc, char **argv) {
	ros::init(argc, argv, "sonar_wrapper");
	ros::NodeHandle n;
	ros::ServiceServer service = n.advertiseService("sonar_wrapper", algo);
	ros::Subscriber sub = n.subscribe("/sonars", 10, chatterCallback);
	ROS_INFO("sonar_wrapper service Ready.");
	ros::spin();    
}
