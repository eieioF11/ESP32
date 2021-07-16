#include "ESP_ros.h"
/*ROS*/
#if (ROS == ON)

ros::NodeHandle nh; //ノードのハンドラ宣言
geometry_msgs::TransformStamped t;
tf::TransformBroadcaster broadcaster;
nav_msgs::Odometry odom;
ros::Publisher odom_pub("odom",&odom);

Speed Tw_Vx(Speed_V,ODOM_R*ODOM_m);
Speed Tw_Vy(Speed_V,ODOM_R*ODOM_m);
Speed Tw_Angular(Speed_W,ODOM_R*ODOM_m);
/*float Tw_Vx = 0.0;
float Tw_Vy = 0.0;
float Tw_Angular = 0.0;*/
bool ROS_EMARGENCYSTOP = false;

void messageCb(const geometry_msgs::Twist &twist) //Twistを受け取ったら呼び出される関数
{
    const float linear_x = twist.linear.x;
    const float linear_y = twist.linear.y;
    const float linear_z = twist.linear.z;
    const float angle_x = twist.angular.x;
    const float angle_y = twist.angular.y;
    const float angle_z = twist.angular.z;
    Tw_Vx = linear_x;
    Tw_Vy = linear_y;
    Tw_Angular = angle_z;
}
ros::Subscriber<geometry_msgs::Twist> sub("cmd_vel", &messageCb); //cmd_velとしてTwistを受け取ったら、messageCbという関数が呼び出される

/*void EulerAnglesToQuaternion(double roll, double pitch, double yaw,
                            double& q0, double& q1, double& q2, double& q3)
{
    double cosRoll = cos(roll / 2.0);
    double sinRoll = sin(roll / 2.0);
    double cosPitch = cos(pitch / 2.0);
    double sinPitch = sin(pitch / 2.0);
    double cosYaw = cos(yaw / 2.0);
    double sinYaw = sin(yaw / 2.0);

    q0 = cosRoll * cosPitch * cosYaw + sinRoll * sinPitch * sinYaw;
    q1 = sinRoll * cosPitch * cosYaw - cosRoll * sinPitch * sinYaw;
    q2 = cosRoll * sinPitch * cosYaw + sinRoll * cosPitch * sinYaw;
    q3 = cosRoll * cosPitch * sinYaw - sinRoll * sinPitch * cosYaw;
}*/

void ESPROStask(void *arg)
{
    nh.getHardware()->setBaud(115200); //通信速度を115200に設定
    nh.initNode();                     //ノードの初期化
    nh.subscribe(sub);
    //nh.advertise(pubimu);
    //nh.advertise(pubmag);
    nh.advertise(odom_pub);
    broadcaster.init(nh);
    portTickType lt = xTaskGetTickCount();
    while (1)
    {
        //double q0,q1,q2,q3;
        //EulerAnglesToQuaternion(-1*odm.roll(ODOM_RAD),-1*odm.pitch(ODOM_RAD),-1*odm.yaw(ODOM_RAD),q0,q1,q2,q3);
        //EulerAnglesToQuaternion(-1*odm.roll(ODOM_RAD),-1*odm.pitch(ODOM_RAD),odm.wyaw(ODOM_RAD),q0,q1,q2,q3);
        float q0 = imu.calcQuat(imu.qw);
        float q1 = -1*imu.calcQuat(imu.qx);
        float q2 = -1*imu.calcQuat(imu.qy);
        float q3 = imu.calcQuat(imu.qz);
        geometry_msgs::Quaternion odom_quat;
        odom_quat.w=q0;
        odom_quat.x=q1;
        odom_quat.y=q2;
        odom_quat.z=q3;
        t.header.frame_id = "odom";
        t.child_frame_id = "base_link";
        t.transform.translation.x = odm.x(ODOM_m);
        t.transform.translation.y = odm.y(ODOM_m);
        t.transform.translation.z = 0.0;
        //t.transform.rotation = tf::createQuaternionFromYaw(odm.wyaw(ODOM_RAD));
        t.transform.rotation = odom_quat;
        t.header.stamp = nh.now();
        broadcaster.sendTransform(t);

        //set the position
        odom.header.stamp = nh.now();
        odom.header.frame_id = "odom";
        odom.pose.pose.position.x = odm.x(ODOM_m);
        odom.pose.pose.position.y = odm.y(ODOM_m);
        odom.pose.pose.position.z = 0.0;
        //odom.pose.pose.orientation = tf::createQuaternionFromYaw(odm.wyaw(ODOM_RAD));
        odom.pose.pose.orientation = odom_quat;
        //set the velocity
        odom.child_frame_id = "base_link";
        odom.twist.twist.linear.x = odm.Rx(ODOM_m);
        odom.twist.twist.linear.y = 0;
        odom.twist.twist.angular.z = odm.Angular(ODOM_RAD);
        odom_pub.publish(&odom);

        nh.spinOnce();
        vTaskDelayUntil(&lt, 200 / portTICK_RATE_MS);
    }
}
#endif