#include <Arduino.h>
#include "Mbed.h"
#include "ururudef.h"
#include "MD.h"
#include "denjiben.h"
extern "C"
{
    #include "ps3.h"
}

//電磁弁７

enum mstatus
{
    Turn,
    anglecon,
    smd1,
    smd2,
    smd3,
    smd4,
    Stop
};
mstatus sel = anglecon;
/*PIDvalue*/
#define KP  0.1
#define KI  0.01
#define KD  0.001
float kp = 0.9, ki = 0.09, kd = 0.01;
/*variable initialization*/
bool movesel=false;
bool mdebug=false;
bool autreset=false;
bool amove=false;
bool fieldch=true;
float mp=1;
float mpuangle,mpuold;//mpu value
short autostatus;
short linecount=0;
int duration = 300; 
int c,i=0;

MSPI spi(&SPI);
/*Motor initialization*/
MD md1(&spi,UM_SS1);
MD md2(&spi,UM_SS2);
MD md3(&spi,UM_SS3);
MD md4(&spi,UM_SS4);
/*Other initialization*/
denjiben sol1(&spi,UM_SS7);

float radcon(float deg)
{
    float Pi=3.1415926535897932384626433832795028841971;
    return deg*Pi/180.00;
}

void mecanum(int speed,float angle)
{
    float m1=0,m2=0,m3=0,m4=0;
    m1=speed*sin(radcon(angle+45));
    m2=speed*cos(radcon(angle+45));
    m3=speed*cos(radcon(angle+45));
    m4=speed*sin(radcon(angle+45));
    printf("%f,%f,%f,%f\n",m1,m2,m3,m4);
    md1=m1;
    md2=m2;
    md3=m3*-1;
    md4=m4*-1bfes;
}

void controller_event_cb( ps3_t ps3, ps3_event_t event )
{
    // Event handling here...
    if ( ps3.analog.stick.lx < 0 )
        //printf("Currently pulling analog stick to the left\n");
    if (ps3.button.right)mecanum(1,0);
    if (ps3.button.left)mecanum(1,180);
    if (ps3.button.up)mecanum(1,90);
    if (ps3.button.down)mecanum(1,270);
    if(!ps3.button.right&&!ps3.button.left&&!ps3.button.up&&!ps3.button.down) mecanum(0,0);
}
/*setup fanction*/
void setup()//setup fanction
{
    SPI.begin(UM_SCLK,UM_MISO,UM_MOSI,-1);
    Serial.begin(115200);
    uint8_t baseMac[6];
    char baseMacChr[18] = {0};
    esp_read_mac(baseMac, ESP_MAC_BT);
    sprintf(baseMacChr, "%02X:%02X:%02X:%02X:%02X:%02X", baseMac[0], baseMac[1], baseMac[2], baseMac[3], baseMac[4], baseMac[5]);
    Serial.print("MAC: ");
    Serial.println(baseMacChr);

    ps3SetEventCallback(controller_event_cb);
    ps3Init();
    while (!ps3IsConnected());

}
/*loop fanction*/
void loop()
{
    wait(0.02);
}