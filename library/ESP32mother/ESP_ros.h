#ifndef ESP_ROS_H_
#define ESP_ROS_H_
#include <Arduino.h>
#include <stdio.h>
#include "math.h"
#include "Setup.h"

#if (ROS == ON)
#include <ros.h>
#include <ros/time.h>
#include <geometry_msgs/Twist.h>
#include <std_msgs/Bool.h>
#include <std_msgs/String.h>
#include "std_msgs/Float32MultiArray.h"
#include <tf/tf.h>
#include <tf/transform_broadcaster.h>
#include <sensor_msgs/Imu.h>
#include <sensor_msgs/MagneticField.h>
#include <nav_msgs/Odometry.h>
#include "ESP_peripheral.h"
#include "Speed.h"
#endif

#if defined __cplusplus
extern "C" {
#endif

#if (ROS == ON)
/*
extern float Tw_Vx;
extern float Tw_Vy;
extern float Tw_Angular;
*/
extern Speed Tw_Vx;
extern Speed Tw_Vy;
extern Speed Tw_Angular;
extern bool ROS_EMARGENCYSTOP;
void ESPROStask(void *arg);
#endif

#if defined __cplusplus
}
#endif
#endif