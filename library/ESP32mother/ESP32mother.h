/*
geinsetup mode
kybode comand
'm':motor select
'p':Pgain set mode
'i':Igain set mode
'd':dgain set mode
'r':md1 gain set
'l':md2 gain set
' ':gain save
'q':mode reset
*/
#ifndef ESP32MOTHER_H_
#define ESP32MOTHER_H_
#include "Setup.h"
#include <Arduino.h>
#include "WiFi.h"
#include "math.h"
#include <FS.h>
#include <SPIFFS.h>
#include <FreeRTOS.h>
#include "Timer.h"
#include "taskrunner.h"
#include "stprint.h"

#include "ESP_peripheral.h"

#include "angle_corr.h"
#include "PID.h"

#include "Trapezoid.h"

#if (SD == ON)
#include "SD.h"
#endif

#if (ESP_OTAE == 1)
#include <ArduinoOTA.h>
#endif

#if (ESP_MODE == ESP_BTS)
#include "BTSerial.h"
#endif

#if (ROS == ON)
#include "ESP_ros.h"
#endif

#if (ESP_MODE == ESP_PS3)
#include "ESP_PS3.h"
#endif



#if defined __cplusplus
extern "C" {
#endif

extern const char*udpmode;

extern i2c_err_t I2C_Error;

extern float Vx;
extern float Vy;
extern float Angular;

class ESP32Mather
{
    private:
        taskrun run;
        Timer pdt;
        //task number
        int tasksel=0;
        //system flag
        bool EMS=false;
        bool LOWV=false;
        bool wificonnect=false;
    public:
        ESP32Mather();
        int setup();
        int setup_ROS();
        void setfunction(const char*name,void (*tfp)(Flag_t *));
        void update();
        bool EMARGENCYSTOP();
        void Error();
        void Start();
        void Start_();
        const char* Systemstatus();
        const char* Taskstatus();
        const char* OTAstatus();
};
extern ESP32Mather ESP32M;

//使用可能なクラスのオブジェクト

//I2CとSPIの関数ポインタ
extern fptr_vv_t ESPtask_i2c_init;
extern fptr_vv_t ESPtask_i2c_while;

void OTAsetup(); //ESP32M.setup()の内部で実行される
void ESPinit();  //setup()で１回実行

#if defined __cplusplus
}
#endif
#endif