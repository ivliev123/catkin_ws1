// 2016-01-15 LLW Simple program to subscribe to a /joy topic and
//                publish a twist
#include <ros/ros.h>
#include <unistd.h>

// 2017-01-10 LLW Header files for ROS joystick and twist messages
#include <sensor_msgs/Joy.h>
#include <geometry_msgs/Twist.h>
#include <stdio.h>

#include "../include/face_tracker_pkg/centroid.h"

// global variables
geometry_msgs::Twist   command_velocity_twist;
ros::Publisher         cmd_vel_publisher;

//Tracker parameters

int servomaxx, servomin,screenmaxx, center_offset, center_left, center_right;
float servo_step_distancex, current_pos_x;

float joy_enable = true;

void track_face(int x,int y)
{

    //Find out if the X component of the face is to the left of the middle of the screen.
    if(x < (center_left)){

        ROS_INFO("Face is at Left");
	
	command_velocity_twist.linear.x = 0;
        command_velocity_twist.angular.z = 1;



    }

    
    //Find out if the X component of the face is to the right of the middle of the screen.
    else if(x > center_right){

        ROS_INFO("Face is at Right");

	command_velocity_twist.linear.x = 0;
  	command_velocity_twist.angular.z = -1;


    }

   else if(x > center_left and x < center_right){

        ROS_INFO("Face is at Center");

	command_velocity_twist.linear.x = 0;
        command_velocity_twist.angular.z = 0;


        }
	else{
		command_velocity_twist.linear.x = 0;
	        command_velocity_twist.angular.z = 0;

	}

}

//Callback of the topic /numbers
void face_callback(const face_tracker_pkg::centroid::ConstPtr& msg)
{
        //ROS_INFO("Recieved X = [%d], Y = [%d]",msg->x,msg->y);

        //Calling track face function
        track_face(msg->x,msg->y);

}




/*
// Callback function for joystick pessaged
void joyCallback(const sensor_msgs::Joy::ConstPtr& joy)
{
  // X vel driven by left joystick for and aft
float a5 = (1+joy->axes[5])/2;
float a2 = (1+joy->axes[2])/2;
//printf("test %ld\n",a5);
  command_velocity_twist.linear.x = a2-a5;
  // heading driven by left joysticj left and right
  command_velocity_twist.angular.z = joy->axes[0];
}
*/

// Callback function for joystick pessaged
void joyCallback(const sensor_msgs::Joy::ConstPtr& joy)
{
if (joy->buttons[0]) joy_enable = false;
if (joy->buttons[1]) joy_enable = true;
  // X vel driven by left joystick for and aft
float a5 = (1+joy->axes[5])/2;
float a2 = (1+joy->axes[2])/2;
//printf("test %ld\n",a5);
  command_velocity_twist.linear.x = (float)(a2-a5+joy->axes[4])*4;
  command_velocity_twist.linear.y = (float)joy->axes[3]*0.5;

  // heading driven by left joysticj left and right
  command_velocity_twist.angular.z = joy->axes[0]*8;
}



namespace my_package
{
 struct Foo
 {
   class Request
   {

   };

   class Response
   {

   };
 
   Request request;
   Response response;
 }; 
}

// this function gets called at 10Hz to publish the command_velocity array of 2 floats
void timerCallback(const ros::TimerEvent& event)
{
/*
ros::NodeHandle nh;
std::map<std::string, std::string> header;
header["val1"] = "val";
header["val2"] = "val";
ros::ServiceClient client = nh.serviceClient<my_package::Foo>("comm_to_stm", false, header);
*/

  if (joy_enable) {
    	cmd_vel_publisher.publish(command_velocity_twist);
//	command_velocity_twist.linear.x = 0;
//	command_velocity_twist.angular.z = 0;
  } 
}


int main(int argc, char** argv)
{

  // init ros
  ros::init(argc, argv, "edumip_ros_joy_node");
//Created a nodehandle object
        ros::NodeHandle node_obj;

  // create node handle
  ros::NodeHandle node;
printf("printf+++++++++++++");
ROS_INFO("++++++++++++++++++++++++++++++++++++++++++++");
  // advertise topic that this node will publish
  cmd_vel_publisher = node.advertise<geometry_msgs::Twist>("cmd_vel", 10);

//  ros::Subscriber number_subscriber = node_obj.subscribe("/face_centroid",10,face_callback);

  screenmaxx = 640;   //max screen horizontal (x)resolution
  center_offset = 100;

  center_left = (screenmaxx / 2) - center_offset;
  center_right = (screenmaxx / 2) + center_offset;;

  // subcscribe to /joy topic to get joystick messages
  ros::Subscriber sub = node.subscribe("joy", 10, &joyCallback);

  // publish to /edumip/cmd topic at 10 hz
  ros::Timer timer = node.createTimer(ros::Duration(0.1), timerCallback);

  // spin until process is killed or interrupted with <CTRL>-C
  ros::spin();

  return 0;

};
