#ifndef ESP_ROS_H_
#define ESP_ROS_H_
#include <Arduino.h>
#include <stdio.h>
#include "math.h"
#include "Setup.h"

#if (ROS == ON)
#include <ros.h>
#include <geometry_msgs/Twist.h>
#include <std_msgs/Bool.h>
#include <std_msgs/String.h>
#include "std_msgs/Float32MultiArray.h"
#include <sensor_msgs/Imu.h>
#include <sensor_msgs/MagneticField.h>
#include "ESP_peripheral.h"
#endif

#if defined __cplusplus
extern "C" {
#endif

#if (ROS == ON)
extern float Tw_Vx;
extern float Tw_Vy;
extern float Tw_Angular;
extern bool ROS_EMARGENCYSTOP;
void ESPROStask(void *arg);
#endif

#if defined __cplusplus
}
#endif
#endif