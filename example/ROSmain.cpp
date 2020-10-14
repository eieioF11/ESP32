#include <Arduino.h>
#include <ros.h>
#include <geometry_msgs/Twist.h>
#include "PWM.h"

PwmOut p1(GPIO_NUM_12);
PwmOut p2(GPIO_NUM_26);

void move(int speed)
{
  if(speed>0)
  {
    digitalWrite(33, HIGH);
    digitalWrite(25, LOW);
    p1=speed;//0~255
    digitalWrite(27, HIGH);
    digitalWrite(14, LOW);
    p2=speed;//0~255
  }
  else
  {
    digitalWrite(33, LOW);
    digitalWrite(25, HIGH);
    p1=speed*-1;//0~255
    digitalWrite(27, LOW);
    digitalWrite(14, HIGH);
    p2=speed*-1;//0~255
  }
}
void turn(int speed)
{
  if(speed>0)
  {
    digitalWrite(33, HIGH);
    digitalWrite(25, LOW);
    p1=speed;//0~255
    digitalWrite(27, LOW);
    digitalWrite(14, HIGH);
    p2=speed;//0~255
  }
  else
  {
    digitalWrite(33, LOW);
    digitalWrite(25, HIGH);
    p1=speed*-1;//0~255
    digitalWrite(27, HIGH);
    digitalWrite(14, LOW);
    p2=speed*-1;//0~255
  }
}
void stop()
{
    digitalWrite(33, HIGH);
    digitalWrite(25, HIGH);
    p1=0;//0~255
    digitalWrite(27, HIGH);
    digitalWrite(14, HIGH);
    p2=0;//0~255 
}

ros::NodeHandle nh;//ノードのハンドラ宣言

void messageCb(const geometry_msgs::Twist& twist) //Twistを受け取ったら呼び出される関数
{
  const float linear_x = twist.linear.x;
  const float linear_y = twist.linear.y;
  const float linear_z = twist.linear.z;
  const float angle_x = twist.angular.x;
  const float angle_y = twist.angular.y;
  const float angle_z = twist.angular.z;

}

ros::Subscriber<geometry_msgs::Twist> sub("cmd_vel", &messageCb);//cmd_velとしてTwistを受け取ったら、messageCbという関数が呼び出される

void setup() 
{
  //nh.getHardware()->setBaud(115200);//通信速度を115200に設定
  nh.initNode();//ノードの初期化
  nh.subscribe(sub);
  pinMode(33, OUTPUT);
  pinMode(25, OUTPUT);
  pinMode(27, OUTPUT);
  pinMode(14, OUTPUT);
}

void loop() 
{
  nh.spinOnce();
  delay(1);
}