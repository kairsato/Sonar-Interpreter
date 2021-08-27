#include "ros/ros.h"
#include "assignment1_setup/pid.h"
//you begin reading sonar value
//values of k
//Sensor value affect
float kp =  0.001;
//actual motor mainly reverses
float kd =  0.00001;

//Variance affect
float ki =  0.0005;
//for PID + default
int i = 0;

//t is the delay (specified by sonar updates in document)
float ti = 0;
float t = 10;
uint16_t et1 = 0;
//et is sonar values
uint16_t et = 0;
float ft1 = 0;
float ft = 0;
float pt = 0;
float dt = 0;
float it = 0;
//yt is acceleration (et * kp)
float yt = 1;
bool algo(assignment1_setup::pid::Request &req, assignment1_setup::pid::Response &res){
	t = req.delay;	
	if(i == 0){
		et = req.distance1;
		pt = et * kp;
		yt = pt + dt + it;
	}else{		
		ti = t + ti;
		et1 = req.before_distance1;
		et = req.distance1;
		ft1 = req.ft1;
		ft = et * t/1000;
		pt = et * kp;
		dt = (et - et1)/(t/1000)*kd;
		it = ft * ki;
		yt = pt + dt + it;
	}     
   res.acceleration = (float)yt;
   res.ft = (float)ft;  
   i = 1;
   ti = ti + t;
   return true;
}
int main(int argc, char **argv) {
    ros::init(argc, argv, "pid_algo");
    ros::NodeHandle n;
    ros::ServiceServer service = n.advertiseService("pid_algo", algo);
    ROS_INFO("PID algo service ready.");
    ros::spin();
}
