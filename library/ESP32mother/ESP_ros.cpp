#include "ESP_ros.h"
/*ROS*/
#if (ROS == ON)

ros::NodeHandle nh; //ノードのハンドラ宣言
//std_msgs::Float32MultiArray sensor;
//ros::Publisher pub("arduino",&sensor);
sensor_msgs::Imu imu;
sensor_msgs::MagneticField mag;
ros::Publisher pubimu("imu/data_raw", &imu);
ros::Publisher pubmag("imu/mag", &mag);

float Tw_Vx = 0.0;
float Tw_Vy = 0.0;
float Tw_Angular = 0.0;
bool ROS_EMARGENCYSTOP = false;
void messageCb(const geometry_msgs::Twist &twist) //Twistを受け取ったら呼び出される関数
{
    const float linear_x = twist.linear.x;
    const float linear_y = twist.linear.y;
    const float linear_z = twist.linear.z;
    const float angle_x = twist.angular.x;
    const float angle_y = twist.angular.y;
    const float angle_z = twist.angular.z;
    /*
    sensor.data[0] = odm.roll();
    sensor.data[1] = odm.pitch();
    sensor.data[2] = odm.yaw();
    sensor.data[3] = l1.swread(sw1);
    sensor.data[4] = l1.swread(sw2);
    sensor.data[5] = ROS_EMARGENCYSTOP;
    pub.publish(&sensor);
    */
    Tw_Vx = linear_x;
    Tw_Vy = linear_y;
    Tw_Angular = angle_z;
}
ros::Subscriber<geometry_msgs::Twist> sub("cmd_vel", &messageCb); //cmd_velとしてTwistを受け取ったら、messageCbという関数が呼び出される
void ESPROStask(void *arg)
{
    nh.getHardware()->setBaud(115200); //通信速度を115200に設定
    nh.initNode();                     //ノードの初期化
    nh.subscribe(sub);
    nh.advertise(pubimu);
    nh.advertise(pubmag);
    /*
    nh.advertise(pub);
    sensor.data_length = ROSdatasize;
    sensor.data = (float *)malloc(sizeof(float) * ROSdatasize);
    sensor.data[0] = 0.0;
    sensor.data[1] = 0.0;
    sensor.data[2] = 0.0;
    sensor.data[3] = 0.0;
    sensor.data[4] = 0.0;
    sensor.data[5] = 0.0;
    */
    portTickType lt = xTaskGetTickCount();
    while (1)
    {
        imu.header.frame_id = "imu_link";
        imu.header.stamp = nh.now();
        imu.angular_velocity.x = mpu.read(GyroX);
        imu.angular_velocity.y = mpu.read(GyroY);
        imu.angular_velocity.z = mpu.read(GyroZ); // [rad/sec]
        imu.linear_acceleration.x = mpu.read(AccX);
        imu.linear_acceleration.y = mpu.read(AccY);
        imu.linear_acceleration.z = mpu.read(AccZ);
        pubimu.publish(&imu);

        mag.header.frame_id = "imu_link";
        mag.header.stamp = nh.now();
        mag.magnetic_field.x = mpu.read(MagX);
        mag.magnetic_field.y = mpu.read(MagY);
        mag.magnetic_field.z = mpu.read(MagZ); // [μT]
        pubmag.publish(&mag);
        nh.spinOnce();
        vTaskDelayUntil(&lt, 1 / portTICK_RATE_MS);
    }
}
#endif