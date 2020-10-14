#include <Arduino.h>
#include "RoboMotherMINI.h"
#include "ururuMD.h"
#include "MoveBase.h"
#include "Mecanum4.h"
#include "odometry.h"

extern "C"
{
  #include "ps3.h"
}

SPISettings mySPISettings = SPISettings(2400000, MSBFIRST, SPI_MODE0);

MD *md[4];
MoveBase *wheel;
double Vx, Vy, Angular;
uint8_t PS3button=0b00000000;//0b/up/down/right/left/△/〇/×/□/

void controller_event_cb(ps3_t ps3, ps3_event_t event);

void SPI_task(void *arg)//SPI communication function
{
  SPI.begin(SCLK, MISO, MOSI, -1);
  SPI.beginTransaction(mySPISettings);
  for(int i=4; i<=7; i++){md[i-4]=new ururuMD(DEV[i],SPEED_MODE);}
  for(int i=0;i<4;i++){((ururuMD*)md[i])->Kp=910.0;((ururuMD*)md[i])->Ki=5600.0;((ururuMD*)md[i])->Kd=1;((ururuMD*)md[i])->SetGain();}//default gain
  
  wheel = new Mecanum4(md);
  portTickType lt = xTaskGetTickCount();
  while(1)
  {
    for(int i=0;i<8;i++)
    {
      switch((int)PS3button&1<<i)
      {
        case 0b10000000:Vy=0.5;printf("up\n");break;
        case 0b01000000:Vy=-0.5;printf("down\n");break;
        case 0b00100000:Vx=0.5;printf("right\n");break;
        case 0b00010000:Vx=-0.5;printf("left\n");break;
        case 0b00001000:printf("triangle\n");break;
        case 0b00000100:printf("circle\n");break;
        case 0b00000010:printf("cross\n");break;
        case 0b00000001:printf("square\n");break;
      }
    }
    PS3button=0b00000000;
    wheel->Move(Vx, Vy,Angular);
    printf("Vx=%3lf Vy=%3lf Angular=%3lf{%f,%f,%f,%f}\n", Vx, Vy,Angular,md[0]->now_val,md[1]->now_val,md[2]->now_val,md[3]->now_val);
    vTaskDelayUntil(&lt,(int)(sampleT*1.0E+3)/portTICK_RATE_MS);
  }
}

void setup() 
{
  Serial.begin(115200);
  uint8_t baseMac[6];
  char baseMacChr[18] = {0};
  esp_read_mac(baseMac, ESP_MAC_BT);
  sprintf(baseMacChr, "%02X:%02X:%02X:%02X:%02X:%02X", baseMac[0], baseMac[1], baseMac[2], baseMac[3], baseMac[4], baseMac[5]);
  Serial.print("MAC: ");
  Serial.println(baseMacChr);
  ps3SetEventCallback(controller_event_cb);
  ps3Init();
  while(!ps3IsConnected());
  Serial.println("Connected");
  xTaskCreatePinnedToCore(SPI_task,"spi_task",8096,NULL,5,NULL,1);
}

void loop() 
{
}

void controller_event_cb(ps3_t ps3, ps3_event_t event)
{
  if (ps3.button.up&&!(PS3button&1<<7))      PS3button+=0b10000000;
  if (ps3.button.down&&!(PS3button&1<<6))    PS3button+=0b01000000;
  if (ps3.button.right&&!(PS3button&1<<5))   PS3button+=0b00100000;
  if (ps3.button.left&&!(PS3button&1<<4))    PS3button+=0b00010000;
  if (ps3.button.triangle&&!(PS3button&1<<3))PS3button+=0b00001000;
  if (ps3.button.circle&&!(PS3button&1<<2))  PS3button+=0b00000100;
  if (ps3.button.cross&&!(PS3button&1<<1))   PS3button+=0b00000010;
  if (ps3.button.square&&!(PS3button&1<<0))  PS3button+=0b00000001;                      
  if(ps3.button.l1)
    Angular=0.5;
  else if(ps3.button.r1)
    Angular=-0.5;
  else
  {
    Vx = (double)ps3.analog.stick.lx/127.0;
    Vy = (double)(ps3.analog.stick.ly/127.0)*-1;
    Angular=(double)(ps3.analog.stick.rx/127.0)*-1;
  }
}