#include <Cytron_SmartDriveDuo.h>
#define IN1 4 // Arduino pin 4 is connected to MDDS30 pin IN1.
#define AN1 5 // Arduino pin 5 is connected to MDDS30 pin AN1.
#define AN2 6 // Arduino pin 6 is connected to MDDS30 pin AN2.
#define IN2 7 // Arduino pin 7 is connected to MDDS30 pin IN2.
Cytron_SmartDriveDuo smartDriveDuo30(PWM_INDEPENDENT, IN1, IN2, AN1, AN2);

//ROS headers
#if (ARDUINO >= 100)
 #include <Arduino.h>
#else
 #include <WProgram.h>
#endif
#include <ros.h>
#include <geometry_msgs/Vector3Stamped.h>
#include <geometry_msgs/Twist.h>
#include <ros/time.h>

#include "robot_specs.h" 

//Motor Shield headers



double rpm_req1 = 0;
double rpm_req2 = 0;


void handle_cmd( const geometry_msgs::Twist& cmd_msg) {
  double x = cmd_msg.linear.x;
  double z = cmd_msg.angular.z;
  if (z == 0) {     // go straight                                                                                                                                                                                                                                                        
    // convert m/s to rpm
    rpm_req1 = x*60/(pi*wheel_diameter);
    rpm_req2 = rpm_req1;
  }
  else if (x == 0) {
    // convert rad/s to rpm
    rpm_req2 = z*track_width*60/(wheel_diameter*pi*2);
    rpm_req1 = -rpm_req2;                                          
  }
  else {
    rpm_req1 = x*60/(pi*wheel_diameter)-z*track_width*60/(wheel_diameter*pi*2);
    rpm_req2 = x*60/(pi*wheel_diameter)+z*track_width*60/(wheel_diameter*pi*2);
  }
}   
                                    
ros::NodeHandle  nh;
ros::Subscriber<geometry_msgs::Twist> sub("cmd_vel", handle_cmd);

geometry_msgs::Vector3Stamped rpm_msg;
ros::Publisher rpm_pub("rpm", &rpm_msg);

ros::Time current_time;
ros::Time last_time;


void setup()
{


 //ROS Setup
  nh.initNode();
  nh.subscribe(sub);
  smartDriveDuo30.control(rpm_req1, rpm_req2);
 
}


void loop()
{
  nh.spinOnce();
  delay(10);
}



