#ifndef SETUP_H_
#define SETUP_H_
#include <FreeRTOS.h>
#include <Arduino.h>

//User Settings------------------------------------
#define ESP_MODE        ESP_NONE        //ESP_PS3:PS3 ESP_BTS: BluetoothSerial, ESP_NONE: NONE
#define ESP_OTA         OFF             //Enable OTA(ON/OFF)
#define ESP_SSID        "ESP32ROBO"     //set SSID for OTA
#define ESP_WIFIPASS    "roborobo"      //set password for wifi
#define LocalPort       10000           //local host
#define ESP_BTSNAME     "ESP32ROBO"     //set name for BTS
#define Delta_T         0.02            //Execution interval[s]
#define d_interval      0.1             //debug interval[s]
#define sensor_interval 0.01            //Sensor update interval[s]
#define GyroZrange      0.15            //Gyro sensor Z-axis error
const IPAddress ip(192,168,30,4);       //Robot ip
const IPAddress subnet(255,255,255,0);  //sub-net mask
//-------------------------------------------------

//PIN Assign -----------------------
//I2C
#define ESP_SDA    GPIO_NUM_13 //SDA
#define ESP_SCL    GPIO_NUM_15 //SCL
//Status 
#define StatusLED1  GPIO_NUM_19 //Status display LED
#define StatusLED2  GPIO_NUM_21 //Status display LED
#define STBY        GPIO_NUM_12
//Encoder
#define enc1a      GPIO_NUM_33 //Motor 1 rotary encoder A phase
#define enc1b      GPIO_NUM_25 //Motor 1 rotary encoder B phase
#define enc2a      GPIO_NUM_26 //Motor 2 rotary encoder A phase
#define enc2b      GPIO_NUM_27 //Motor 2 rotary encoder B phase
#define enc3a      GPIO_NUM_17 //Motor 3 rotary encoder A phase
#define enc3b      GPIO_NUM_16 //Motor 3 rotary encoder B phase
#define enc4a      GPIO_NUM_18 //Motor 4 rotary encoder A phase
#define enc4b      GPIO_NUM_5  //Motor 4 rotary encoder B phase
#define resolution 2048.0f     //Rotary encoder resolution
//----------------------------------

//Don't care -----------------------
#define ESP_NONE 0
#define ESP_BTS 1
#define ESP_PS3 2

#if (ESP_MODE == ESP_PS3)
#define ESP_OTAE　0
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
    inline float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
    {
        const float r = 500.0f;//1/500まで
        return (float)(map(x * r, in_min * r, in_max * r, out_min * r, out_max * r) / r);
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