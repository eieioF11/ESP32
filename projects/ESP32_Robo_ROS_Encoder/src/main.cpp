#include <Arduino.h>
#include "ESP32mother.h"

#undef ESP32
#include <ros.h>
#include <ros/time.h>
#include <tf/tf.h>
#include <tf/transform_broadcaster.h>
#include <nav_msgs/Odometry.h>

ros::NodeHandle nh; //ノードのハンドラ宣言
nav_msgs::Odometry odom;
ros::Publisher odom_pub("Encoder_odom", &odom);

void setup()
{
	/*UART setup*/
	Serial.begin(115200);
	/*Digitalpin setup*/
	pinMode(STBY, OUTPUT);
	pinMode(SD_CHECK, INPUT);
	pinMode(SD_MOUNT, INPUT);
	pinMode(stsw, INPUT_PULLUP);
	pinMode(StatusLED, OUTPUT);
	digitalWrite(StatusLED, HIGH);
	digitalWrite(STBY, LOW);
	motorsetup();
	((ESPMotor *)md[0])->Reset();
	((ESPMotor *)md[1])->Reset();
	xTaskCreatePinnedToCore(enctask, "Encoder task", 1024, NULL, 2, NULL, 0);
	xTaskCreatePinnedToCore(Odmetryupdate, "Odmetry task", 8096, NULL, 0, NULL, 1);
	tone(beep, 246);
	delay(100);
	noTone(beep);
	digitalWrite(StatusLED, LOW);

	nh.getHardware()->setBaud(115200); //通信速度を115200に設定
	nh.initNode();					   //ノードの初期化
	nh.advertise(odom_pub);
}

void loop()
{
	// set the position
	odom.header.stamp = nh.now();
	odom.header.frame_id = "odom";
	odom.pose.pose.position.x = 0.0;
	odom.pose.pose.position.y = 0.0;
	odom.pose.pose.position.z = 0.0;
	// set the velocity
	// md[0]->now_val, md[1]->now_val
	odom.child_frame_id = "base_link";
	odom.twist.twist.linear.x = 2.0f * PI * md[0]->now_val;
	odom.twist.twist.linear.y = 2.0f * PI * md[1]->now_val;
	odom.twist.twist.angular.z = 0.0;
	odom_pub.publish(&odom);

	nh.spinOnce();
	delay(200);
}