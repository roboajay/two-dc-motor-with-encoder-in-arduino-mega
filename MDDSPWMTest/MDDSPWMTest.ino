/* 
 * This example shows how to control MDDS30 in PWM mode with Arduino.
 * Set MDDS30 input mode to 0b10110100
 * Open Serial Monitor, set baudrate to 9600bps and "No line ending".
 * Send 0: Left motor stops
 *      1: Left motor rotates CW with half speed
 *      2: Left motor rotates CW with full speed
 *      3: Left motor rotates CCW with half speed
 *      4: Left motor rotates CCW with full speed
 *      5: Right motor stops
 *      6: Right motor rotates CW with half speed
 *      7: Right motor rotates CW with full speed
 *      8: Right motor rotates CCW with half speed
 *      9: Right motor rotates CCW with full speed
 * 
 * Note: This example also compatible with MDDS10 and MDDS60
 *
 * Hardware Connection:
 *   Arduino Uno    MDDS30
 *   GND ---------- GND
 *   4 ------------ IN1
 *   5 ------------ AN1
 *   6 ------------ AN2
 *   7 ------------ IN2
 *
 * Related Products:
 * - SmartDriveDuo-30: http://www.cytron.com.my/P-MDDS60
 * - CT UNO: http://www.cytron.com.my/p-ct-uno
 * - DC Brush Motors: http://www.cytron.com.my/c-84-dc-motor
 * - LiPo Battery: http://www.cytron.com.my/c-87-power/c-97-lipo-rechargeable-battery-and-charger
 * - Male to Male Jumper: https://www.cytron.com.my/p-wr-jw-mm65
 *
 * URL: http://www.cytron.com.my
 */

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

//#include "robot_specs.h" 
/*
refer this when you want to inculde wheel spec for real robot with wheel
https://github.com/sungjik/my_personal_robotic_companion/blob/master/my_personal_robotic_companion/arduino/motor_controller/robot_specs.h
*/
 
//Motor Shield headers
#include <Cytron_SmartDriveDuo.h>

//Pin definition

#define IN1 4 // Arduino pin 4 is connected to MDDS30 pin IN1.
#define AN1 5 // Arduino pin 5 is connected to MDDS30 pin AN1.
#define AN2 6 // Arduino pin 6 is connected to MDDS30 pin AN2.
#define IN2 7 // Arduino pin 7 is connected to MDDS30 pin IN2.
Cytron_SmartDriveDuo smartDriveDuo30(PWM_INDEPENDENT, IN1, IN2, AN1, AN2);

char inChar;
signed int speedLeft, speedRight;

//these pins can not be changed 2/3 are special pins (they are interrupt  pins)
int encoderPin1 = 2;
int encoderPin2 = 3;

volatile int lastEncoded = 0;
volatile long encoderValue = 0;
 
long lastencoderValue = 0;
 
int lastMSB = 0;
int lastLSB = 0;

//ROS setup


ros::NodeHandle  nh;


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

ros::Subscriber<geometry_msgs::Twist> sub("cmd_vel", handle_cmd);

geometry_msgs::Vector3Stamped rpm_msg;
ros::Publisher rpm_pub("rpm", &rpm_msg);

ros::Time current_time;
ros::Time last_time;


void setup()
{
  pinMode(13, OUTPUT);
  Serial.begin(9600);
  
//DC motor setup  
  digitalWrite(13, HIGH);
  delay(2000); // Delay for 5 seconds.
  digitalWrite(13, LOW);

//enoder setup
  pinMode(encoderPin1, INPUT); 
  pinMode(encoderPin2, INPUT);
 
  digitalWrite(encoderPin1, HIGH); //turn pullup resistor on
  digitalWrite(encoderPin2, HIGH); //turn pullup resistor on
 
  //call updateEncoder() when any high/low changed seen
  //on interrupt 0 (pin 2), or interrupt 1 (pin 3) 
  attachInterrupt(0, updateEncoder, CHANGE); 
  attachInterrupt(1, updateEncoder, CHANGE);
 
}

void loop()
{
  //DC motor
  if (Serial.available()) {
    delay(100);
    inChar = (char)Serial.read();

    if (inChar == '0') {
      digitalWrite(13, LOW);
      speedLeft = 0;
    }
    else if (inChar == '1') {
      digitalWrite(13, HIGH);
      speedLeft = 50;
    }
    else if (inChar == '2') {
      digitalWrite(13, HIGH);
      speedLeft = 100;
    }
    else if (inChar == '3') {
      digitalWrite(13, HIGH);
      speedLeft = -50;
    }
    else if (inChar == '4') {
      digitalWrite(13, HIGH);
      speedLeft = -100;
    }
    else if (inChar == '5') {
      digitalWrite(13, LOW);
      speedRight = 0;
    }
    else if (inChar == '6') {
      digitalWrite(13, HIGH);
      speedRight = 50;
    }
    else if (inChar == '7') {
      digitalWrite(13, HIGH);
      speedRight = 100;
    }
    else if (inChar == '8') {
      digitalWrite(13, HIGH);
      speedRight = -50;
    }
    else if (inChar == '9') {
      digitalWrite(13, HIGH);
      speedRight = -100;
    }

    smartDriveDuo30.control(speedLeft, speedRight);
  }

  //Encoder
  Serial.println(encoderValue);
  delay(1000); //just here to slow down the output, and show it will work  even during a delay
}

void updateEncoder(){
  int MSB = digitalRead(encoderPin1); //MSB = most significant bit
  int LSB = digitalRead(encoderPin2); //LSB = least significant bit
 
  int encoded = (MSB << 1) |LSB; //converting the 2 pin value to single number
  int sum  = (lastEncoded << 2) | encoded; //adding it to the previous encoded value
 
  if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) encoderValue ++;
  if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) encoderValue --;
 
  lastEncoded = encoded; //store this value for next time
}

