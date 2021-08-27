Using ROS to drive a robot to a desired target using PID algorithmn and Kalman Filter.
Linux Distro: https://www.ros.org/

Structure
Using a feedback loop there are 5 main ROS Nodes
*Controller -> Identifies direction heading and timing for accerelation. Node that send movement instruction to robot.
*Kalman Filter -> Filter that takes in the noisy sonar values and reduces the noise emitted from them
*PID algorithmn -> Algorithmn responsible for calculation movement and adjusting velocity 
*Sonar Wrapper -> Grabs the sonar values


Input
![alt text](https://drive.google.com/file/d/101U5PQFTuDMLP9NDVxg_o4IVRVQEhoWq/view?usp=sharing)

Structure
![alt text](https://drive.google.com/file/d/19tdwWErA8LcisXoTl9Q2WsUjrgxPd9FO/view?usp=sharing)

Implemented Kalman Filter
![alt text](https://drive.google.com/file/d/173YXdjpgEnzbUqosGt0KxFvlYIHYzhLd/view?usp=sharing)

Implemented PID algorithmn
![alt text](https://drive.google.com/file/d/1DCws05VcmJNza3SM_5676-9IzQXM89re/view?usp=sharing)
