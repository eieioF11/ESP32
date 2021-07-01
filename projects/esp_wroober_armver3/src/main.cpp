#include <Arduino.h>
#include "WiFi.h"
#include <ArduinoOTA.h>
#include "SD.h"
#include "Setup.h"

class servo
{
  private:
    uint8_t ch;
    uint8_t pin;
  public:
    servo(uint8_t ch_,uint8_t pin_);
    void write(int deg);
};
servo::servo(uint8_t ch_,uint8_t pin_)
{
  ch=ch_;
  pin=pin_;
  ledcSetup(ch, 50, 10);  // 0ch 50 Hz 10bit resolution
  ledcAttachPin(pin, ch); // 15pin, 0ch
}
void servo::write(int deg)
{
  const int s_min = 26;  // (26/1024)*20ms ≒ 0.5 ms  (-90°)
  const int s_max = 123; // (123/1024)*20ms ≒ 2.4 ms (+90°)
  int n = deg+48;
  //printf("%d/\n",n);
  if (n > s_max) n = s_max;
  if (n < s_min) n = s_min;
  ledcWrite(ch, n);
}
servo s1(0,18);
servo s2(1,19);
servo s3(2,21);
servo s4(3,22);
int s1deg=75,s2deg=35,s3deg=32,s4deg=-40;
const int mnum=30;
int angle1[mnum]={0};
int angle2[mnum]={0};
int angle3[mnum]={0};
int angle4[mnum]={0};
int num=0;
int count=-1;
bool start=false;

void allreset()
{
  s1deg=75;
  s2deg=35;
  s3deg=32;
  s4deg=-40;
  num=0;
  count=-1;
  start=false;
  printf("allreset\n");
}
void reset()
{
  s1deg=75;
  s2deg=35;
  s3deg=32;
  s4deg=-40;
  printf("reset\n");
}

void save()
{
  angle1[num]=s1deg;
  angle2[num]=s2deg;
  angle3[num]=s3deg;
  angle4[num]=s4deg;
  printf("save %d(%d,%d,%d,%d)\n",num,angle1[num],angle2[num],angle3[num],angle4[num]);
  if(num>mnum)
    num=100;
  else
    num++;
}

void move(int sdeg1,int sdeg2,int sdeg3,int sdeg4)
{
    if(sdeg1<=-15)sdeg1=-15;
    if(sdeg1>=90)sdeg1=90;
    if(sdeg2<=-20)sdeg2=-20;
    if(sdeg2>=90)sdeg2=90;
    if(sdeg3<=-4)sdeg3=-4;
    if(sdeg3>=75)sdeg3=75;
    if(sdeg4<=-40)sdeg4=-40;
    if(sdeg4>=25)sdeg4=25;
    //printf("%d,%d,%d,%d\n",sdeg1,sdeg2,sdeg3,sdeg4);
    //BTSerial.printf("%d,%d,%d,%d\n",sdeg1,sdeg2,sdeg3,sdeg4);
    s1.write(sdeg1);
    s2.write(sdeg2);
    s3.write(sdeg3);
    s4.write(sdeg4);
}

void move()
{
  printf("%d run %d,",num,count);
  if(count==-1)
    reset();
  else
    move(angle1[count],angle2[count],angle3[count],angle4[count]);
  if(count==num-1)
  {
    count=-1;
    start=false;
  }
  else
    count++;
  delay(500);
}

//const IPAddress ip(192,168,30,3);
//const IPAddress subnet(255,255,255,0);

void setup()
{
  Serial.begin(115200);
    s1.write(s1deg);
    s2.write(s2deg);
    s3.write(s3deg);
    s4.write(s4deg);
}

void loop()
{
  int sdata=0;
  Serial.println("A");
  if(Serial.available())
  {
    sdata = Serial.read();
    switch(sdata)
    {
      case 180:
        s2deg=(int)Serial.read();
        s3deg=(int)Serial.read();
        s4deg=(int)Serial.read();
        s1deg=(int)Serial.read();
        if(s1deg>128)
          s1deg-=256;
        if(s2deg>128)
          s2deg-=256;
        if(s3deg>128)
          s3deg-=256;
        if(s4deg>128)
          s4deg-=256;
        Serial.printf("%d,%d,%d\n",s2deg,s3deg,s4deg);
        break;
      default:sdata=0;break;
    }
  }
  if(sdata!=0)
  {
    if(s1deg<=-15)s1deg=-15;
    if(s1deg>=90)s1deg=90;
    if(s2deg<=-20)s2deg=-20;
    if(s2deg>=90)s2deg=90;
    if(s3deg<=-4)s3deg=-4;
    if(s3deg>=75)s3deg=75;
    if(s4deg<=-40)s4deg=-40;
    if(s4deg>=25)s4deg=25;
    move(s1deg,s2deg,s3deg,s4deg);
  }
  else if(start)
  {
    move();
  }
  delay(20);
}