#ifndef ESP_ROS_H_
#define ESP_ROS_H_
#include <Arduino.h>
#include <stdio.h>
#include "math.h"
#include "Setup.h"

#if defined __cplusplus
extern "C" {
#endif

#if (ROS == ON)
#include <ros.h>
#include <geometry_msgs/Twist.h>
#include <std_msgs/Bool.h>
#include <std_msgs/String.h>
#include "std_msgs/Float32MultiArray.h"
#include "ESP_Sensor.h"

ros::NodeHandle nh;//ノードのハンドラ宣言
std_msgs::Float32MultiArray sensor;
ros::Publisher pub("arduino",&sensor);
void ESPROStask(void *arg);
#endif

#if defined __cplusplus
}
#endif
#endif