#include "ESP_ros.h"
/*ROS*/
#if (ROS == ON)

ros::NodeHandle nh; //ノードのハンドラ宣言
geometry_msgs::TransformStamped t;
tf::TransformBroadcaster broadcaster;

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
    Tw_Vx = linear_x;
    Tw_Vy = linear_y;
    Tw_Angular = angle_z;
}
ros::Subscriber<geometry_msgs::Twist> sub("cmd_vel", &messageCb); //cmd_velとしてTwistを受け取ったら、messageCbという関数が呼び出される

void EulerAnglesToQuaternion(double roll, double pitch, double yaw,
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
}

void ESPROStask(void *arg)
{
    nh.getHardware()->setBaud(115200); //通信速度を115200に設定
    nh.initNode();                     //ノードの初期化
    nh.subscribe(sub);
    broadcaster.init(nh);
    portTickType lt = xTaskGetTickCount();
    while (1)
    {
        double q0,q1,q2,q3;
        EulerAnglesToQuaternion(odm.roll(ODOM_RAD),odm.pitch(ODOM_RAD),odm.wyaw(ODOM_RAD),q0,q1,q2,q3);
        t.header.frame_id = "/odom";
        t.child_frame_id = "/base_link";
        t.transform.translation.x = odm.x(ODOM_m);
        t.transform.translation.y = odm.y(ODOM_m);
        t.transform.translation.z = 0.0;

        t.transform.rotation.x = q1;
        t.transform.rotation.y = q2;
        t.transform.rotation.z = q3;
        t.transform.rotation.w = q0;
        t.header.stamp = nh.now();
        broadcaster.sendTransform(t);
        nh.spinOnce();
        vTaskDelayUntil(&lt, 10 / portTICK_RATE_MS);
    }
}
#endif