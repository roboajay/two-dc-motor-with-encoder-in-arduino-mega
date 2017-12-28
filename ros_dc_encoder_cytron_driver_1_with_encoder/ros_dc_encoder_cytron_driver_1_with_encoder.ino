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

ros::NodeHandle  nh;

double rpm_req1 = 0;
double rpm_req2 = 0; 

//encoder definition
//these pins can not be changed 2/3 are special pins
int encoderPin1 = 2;
int encoderPin2 = 3;
 
volatile int lastEncoded = 0;
volatile double encoderValue = 0;
volatile long no_rev_act=0;
 
long lastencoderValue = 0;
 int t=0;
 
int lastMSB = 0;
int lastLSB = 0;

void handle_cmd( const geometry_msgs::Twist& cmd_msg) {
  double x = cmd_msg.linear.x;
  double no_rev_cmd = cmd_msg.linear.y;
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
   
  while (no_rev_act < no_rev_cmd)
   {
     smartDriveDuo30.control(rpm_req1, rpm_req2);

     attachInterrupt(0, updateEncoder, CHANGE); 
     attachInterrupt(1, updateEncoder, CHANGE);
 
   }
   
 if (no_rev_act >= no_rev_cmd)
   {
   smartDriveDuo30.control(0, 0);
   }
}   

ros::Subscriber<geometry_msgs::Twist> sub("cmd_vel", handle_cmd);

void updateEncoder(){
  int MSB = digitalRead(encoderPin1); //MSB = most significant bit
  int LSB = digitalRead(encoderPin2); //LSB = least significant bit
 
  int encoded = (MSB << 1) |LSB; //converting the 2 pin value to single number
  int sum  = (lastEncoded << 2) | encoded; //adding it to the previous encoded value
  if(encoderValue<=14760 && encoderValue>=-14760)
  {
    if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) encoderValue ++;
    if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) encoderValue --;
    
    lastEncoded = encoded;
   //store this value for next time
  volatile double temp = abs(encoderValue);
  }
  else
  {
    encoderValue=0;
    no_rev_act++;
  }
  
 
  
}                          
geometry_msgs::Vector3Stamped rpm_msg;
ros::Publisher encoder_val_1("/encoder_value/1", &rpm_msg);

void setup()
{
  nh.getHardware()->setBaud(57600);
  nh.initNode();
  
  //encoder setup
  pinMode(encoderPin1, INPUT); 
  pinMode(encoderPin2, INPUT);
 
  digitalWrite(encoderPin1, HIGH); //turn pullup resistor on
  digitalWrite(encoderPin2, HIGH); //turn pullup resistor on
 
  //call updateEncoder() when any high/low changed seen
  //on interrupt 0 (pin 2), or interrupt 1 (pin 3) 

    attachInterrupt(0, updateEncoder, CHANGE); 
  attachInterrupt(1, updateEncoder, CHANGE);
  nh.advertise(encoder_val_1);
  nh.subscribe(sub);
}

void loop()
{



  
  rpm_msg.header.stamp = nh.now();
  rpm_msg.vector.x = encoderValue;
  rpm_msg.vector.y = no_rev_act;
  rpm_msg.vector.z = 0;
  encoder_val_1.publish(&rpm_msg);
  nh.spinOnce();
  delay(1);
}


