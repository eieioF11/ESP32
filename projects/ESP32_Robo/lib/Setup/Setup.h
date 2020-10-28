#ifndef SETUP_H_
#define SETUP_H_
#include <FreeRTOS.h>
#include <Arduino.h>

//User Settings------------------------------------
#define ESP_MODE        ESP_NONE         //ESP_PS3:PS3 ESP_BTS: BluetoothSerial, ESP_NONE: NONE
#define ESP_OTA         ON               //Enable OTA(ON/OFF)
#define ESP_SSID        "ESP32ROBO"      //Set SSID for OTA
#define ESP_WIFIPASS    "roborobo"       //Set password for wifi
#define LocalPort       10000            //local host
#define ESP_BTSNAME     "ESP32ROBO"      //Set name for BTS
#define MOTORMODE       PID_M            //Set motor mode(PID_M/DUTY_M)
#define DEFAULTTASK     ON               //Set up a task for testing(test,wificontroller or PS3controller)
#define STARTTASK       0                //0 setuptask,1 contorollertask
#define I2CPORT         ON               //using i2c port
#define SD              ON               //SD card activation
#define ROS             OFF              //Enable ROS
#define ROSdatasize     5                //ROS transmission data size [byte]
#define Delta_T         0.02             //Execution interval[s]
#define d_interval      0.1              //debug interval[s]
#define sensor_interval 0.01             //Sensor update interval[s]
#define LOG_interval    0.05             //State recording interval[s]
#define LOGNUM          50               //Number of recordings
const IPAddress ip(192,168,30,4);        //Robot ip
const IPAddress subnet(255,255,255,0);   //sub-net mask
const IPAddress ip2(192, 168, 30, 5);    //Controller ip
const IPAddress gateway(192,168, 30, 1); //Gateway settings
const IPAddress DNS(192, 168, 30, 90);   //DNS settings
#define MAC_ADD (char*)"24:0a:c4:0f:c3:aa"//PS3controller MAC Address
//-------------------------------------------------

//PIN Assign -----------------------
//I2C
#define ESP_SDA    GPIO_NUM_26 //SDA
#define ESP_SCL    GPIO_NUM_27 //SCL
//SDcard
#define SD_MOSI    GPIO_NUM_23 //SD card MOSI
#define SD_MISO    GPIO_NUM_19 //SD card MISO
#define SD_SCLK    GPIO_NUM_18 //SD card SCLK
#define SD_CS      GPIO_NUM_15 //SD card CS
#define SD_CHECK   GPIO_NUM_34 //Pin to check if SD card is inserted
#define SD_MOUNT   GPIO_NUM_0  //SD card unmount button
//Beep
#define beep       GPIO_NUM_2  //Pin used for piezoelectric element
//Status
#define StatusLED  GPIO_NUM_12 //Status display LED
#define stsw       GPIO_NUM_39 //Mode selection button
//Motor
#define m1o1       GPIO_NUM_33 //Motor 1 input 1
#define m1o2       GPIO_NUM_25 //Motor 1 input 2
#define m1pwm      GPIO_NUM_32 //Motor 1 PWM
#define m2o1       GPIO_NUM_16 //Motor 2 input 1
#define m2o2       GPIO_NUM_4  //Motor 2 input 2
#define m2pwm      GPIO_NUM_17 //Motor 2 PWM
#define STBY       GPIO_NUM_5  //Standby PIN
//Encoder
#define enc1a      GPIO_NUM_21 //Motor 1 rotary encoder A phase
#define enc1b      GPIO_NUM_22 //Motor 1 rotary encoder B phase
#define enc2a      GPIO_NUM_13 //Motor 2 rotary encoder A phase
#define enc2b      GPIO_NUM_14 //Motor 2 rotary encoder B phase
#define resolution 2048.0f     //Rotary encoder resolution
//Motor power supply voltage
#define MOTOR_V    GPIO_NUM_35 //Battery voltage measuring pin
//----------------------------------

//Don't care -----------------------
#define ESP_NONE 0
#define ESP_BTS 1
#define ESP_PS3 2

#if (ESP_MODE == ESP_PS3)
#define ESP_OTAE 0
#else
#define ESP_OTAE ESP_OTA
#endif

#define ON 1
#define OFF 0

#if (ESP_MODE == ESP_BTS)
#define ESP_SERIAL BTSerial
#else
#define ESP_SERIAL Serial
#endif

#if defined __cplusplus
extern "C"
{
#endif
    extern bool OTAFLAG;

    inline float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
    {
        return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    }
    inline int STSW()
    {
        return digitalRead(stsw);
    }
    inline int SDCH()
    {
        return digitalRead(SD_CHECK);
    }
    inline int mount()
    {
        return digitalRead(SD_MOUNT);
    }

    typedef void (*fptr_vv_t)();

    inline void wait(float time)
    {
        vTaskDelay(time * 1000 / portTICK_RATE_MS);
    }

#if defined __cplusplus
}
#endif

#endif