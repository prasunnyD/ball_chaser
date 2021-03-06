#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z){
  // TODO: Request a service and pass the velocities to it to drive the robot
  ball_chaser::DriveToTarget srv;
  srv.request.linear_x = lin_x;
  srv.request.angular_z = ang_z;
  if(!client.call(srv)){
     ROS_ERROR("Failed to call DriveToTarget service");
   }
}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img){


    // TODO: Loop through each pixel in the image and check if there's a bright white one
    // Then, identify if this pixel falls in the left, mid, or right side of the image
    // Depending on the white ball position, call the drive_bot function and pass velocities to it
    // Request a stop when there's no white ball seen by the camera

    float x, z;
  	int ball_location;
    int white_pixel = 255;
    for (int i = 0; i < img.height * img.step; i+=3) {
      	int red = img.data[i];
      	int green = img.data[i+1];
      	int blue = img.data[i+2];
    	if(red == white_pixel && green == white_pixel && blue == white_pixel ){
          //Drives toward the ball and turns depending where it is in the image
        	x = 0.5;
          //Determines where is ball located in image along the x-axis
         	ball_location = i % img.step;
          //Function determines how much robot should turn depending on where ball is located in the image
        	z = -0.00042 * ball_location + 0.5;
          	break;
        }
     	else{
          //Searches for the ball by turning if ball in not in the image
          	x = 0.0;
          	z = 0.5;
        }
    }
   
    drive_robot(x,z);

}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}