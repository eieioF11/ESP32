#include "ESP_ros.h"
/*ROS*/
#if (ROS == ON)
void messageCb(const geometry_msgs::Twist& twist) //Twistを受け取ったら呼び出される関数
{
    const float linear_x = twist.linear.x;
    const float linear_y = twist.linear.y;
    const float linear_z = twist.linear.z;
    const float angle_x = twist.angular.x;
    const float angle_y = twist.angular.y;
    const float angle_z = twist.angular.z;
    sensor.data[0]=l1.read(0);
    sensor.data[1]=l1.read(1);
    sensor.data[2]=l1.read(2);
    sensor.data[3]=l1.swread(sw1);
    sensor.data[4]=l1.swread(sw2);
    pub.publish(&sensor);
    if (linear_x != 0.0 && angle_z == 0.0)
    {
        //tw.move(linear_x);
    }
    else if (angle_z > 0.0) 
    {
        if (linear_x == 0) 
        {
            //tw.turn(angle_x);
        }
        else if (linear_x > 0) 
        {
      // Go left forward
        } 
        else 
        {
      // Go right back
        }
    } 
    else if (angle_z < 0.0) 
    {
        if (linear_x == 0)
        { 
            //turn(angle_x);
        }
        else if (linear_x > 0) 
        {
          // Go right forward
        } 
        else 
        {
      // Go left back
        }
    } 
    else
    { 
        //stop();
    }
}
ros::Subscriber<geometry_msgs::Twist> sub("cmd_vel", &messageCb);//cmd_velとしてTwistを受け取ったら、messageCbという関数が呼び出される
void ESPROStask(void *arg)
{
    nh.getHardware()->setBaud(115200);//通信速度を115200に設定
    nh.initNode();//ノードの初期化
    nh.subscribe(sub);
    nh.advertise(pub);
    sensor.data_length=ROSdatasize;
    sensor.data = (float*)malloc(sizeof(float)*ROSdatasize);
    sensor.data[0]=0.0;
    sensor.data[1]=0.0;
    sensor.data[2]=0.0;
    sensor.data[3]=0.0;
    sensor.data[4]=0.0;
    portTickType lt = xTaskGetTickCount();
    while(1)
    {
        nh.spinOnce();
        vTaskDelayUntil(&lt, 1/portTICK_RATE_MS);
    }
}
#endif