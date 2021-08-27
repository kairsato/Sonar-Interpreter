#include "ros/ros.h"
#include "assignment1_setup/kalman_filter_values.h"
int i =0;
//variance const
int ri = 0;
int zi = 0;
int yid = 0;
int pid = 0;
int ki = 0;
//previous filtered value
int yi =  500;
//ri
float pi = 100;
bool algo(assignment1_setup::kalman_filter_values::Request &req, assignment1_setup::kalman_filter_values::Response &res){
	//ROS_INFO("Input ri[%d] zi[%d] yid[%d] pid[%d]",(int)req.ri,(int)req.zi,(int)req.yid,(int)req.pid);
	ri = (int)req.ri;
	zi = req.zi;
	yid = req.yid; 
	pid = req.pid;
	if(i == 0){
		yid = 0;
		yi = zi;
		pi = ri;
	}else{
		ki = pid/(pid+ri);
		yi = yid + (ki * (ri - yid));	
		pi = (1-ki) * pid;
	}	
	res.yi = yi;
	res.pi = pi;
	i = 1;
	ROS_INFO("Output %u %f",yi,pi);
	return true;	
}
int main(int argc, char **argv) {
	ros::init(argc, argv, "kalman_filter");
	ros::NodeHandle n;   
	ros::ServiceServer service = n.advertiseService("kalman_filter", algo);
	ros::spin();
	return 0;
}
