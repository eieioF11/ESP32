#ifndef SETUP_H_
#define SETUP_H_
#include <Arduino.h>
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver 
#include "Timer.h"
#include "stprint.h"
#include "Mag.h"

#if defined __cplusplus
extern "C" {
#endif

/*OTA Setup*/
#define ARDUINO_OTA_UPDATE      //! Enable this line OTA update
/*Pin Setup*/
#define TP_PIN_PIN          33
#define I2C_SDA_PIN         21
#define I2C_SCL_PIN         22
#define IMU_INT_PIN         38
#define RTC_INT_PIN         34
#define BATT_ADC_PIN        35
#define VBUS_PIN            36
#define TP_PWR_PIN          25
#define LED_PIN             4
#define CHARGE_PIN          32
/*Wifi Timeupdate setup*/
const char* ntpServer = "ntp.jst.mfeed.ad.jp";//日本のNTPサーバー選択
const long gmtOffset_sec = 9 * 3600;//9時間の時差を入れる
const int daylightOffset_sec = 0;//夏時間はないのでゼロ
/*Other*/
const IPAddress ip(192,168,0,109);
const IPAddress subnet(255,255,255,0);
#define funcnum 9
#define PSMLEVEL 30.0f //Power saving mode
#define Timernum 5
/*
Default color definitions
TFT_BLACK       0x0000     
TFT_NAVY        0x000F      
TFT_DARKGREEN   0x03E0      
TFT_DARKCYAN    0x03EF      
TFT_MAROON      0x7800      
TFT_PURPLE      0x780F      
TFT_OLIVE       0x7BE0      
TFT_LIGHTGREY   0xD69A      
TFT_DARKGREY    0x7BEF     
TFT_BLUE        0x001F     
TFT_GREEN       0x07E0      
TFT_CYAN        0x07FF     
TFT_RED         0xF800      
TFT_MAGENTA     0xF81F      
TFT_YELLOW      0xFFE0     
TFT_WHITE       0xFFFF      
TFT_ORANGE      0xFDA0      
TFT_GREENYELLOW 0xB7E0      
TFT_PINK        0xFE19       
TFT_BROWN       0x9A60      
TFT_GOLD        0xFEA0      
TFT_SILVER      0xC618      
TFT_SKYBLUE     0x867D      
TFT_VIOLET      0x915C      
 */
bool otaStart = false;
Timer t[Timernum];
float sleeptime=30;
/*Function*/
#define squar(x) (x*x)

inline float CPUTemp()
{
  return (float)temperatureRead();
}

inline float mapfloat(float x,float in_min,float in_max,float out_min,float out_max)
{
  const float r=500.0f;
  return (float)(map(x*r,in_min*r,in_max*r,out_min*r,out_max*r)/r);
}

inline String DATE(uint8_t month,uint8_t day,uint16_t year,bool monly=false)
{
  String Date="";
  switch(month)
  {
    default:
    case 1: Date="Jan.";break;
    case 2: Date="Feb.";break;
    case 3: Date="Mar.";break;
    case 4: Date="Apr.";break;
    case 5: Date="May ";break;
    case 6: Date="Jun.";break;
    case 7: Date="Jul.";break;
    case 8: Date="Aug.";break;
    case 9: Date="Sep.";break;
    case 10:Date="Oct.";break;
    case 11:Date="Nov.";break;
    case 12:Date="Dec.";break;
  }
  if(!monly)Date+=String(day)+" "+String(year);
  return Date;
}

inline int8_t Weeknum(uint8_t month,uint8_t day,uint16_t year)
{
  struct tm  t;
  t.tm_year  = year - 1900;    /* 年を調整 */
  t.tm_mon   = month - 1;        /* 月を調整 */
  t.tm_mday  = day;            /* 日 */
  t.tm_hour  = 0;                /* 時 */
  t.tm_min   = 0;                /* 分 */
  t.tm_sec   = 0;                /* 秒 */
  t.tm_isdst = -1;            /* 夏時間 */
  if (mktime(&t) == -1)        /* 変換に失敗 */
    return (-1);
  return (t.tm_wday);
}

inline bool IsLeapYear(int year)
{
    if (((year % 4 == 0) && (year % 100 != 0)) || year % 400 == 0) {
        return true;
    } else {
        return false;
    }
}
/* get the last day of month */
inline int GetLastDay(int year, int month)
{
    bool leap;
    int lmdays[] = {31,29,31,30,31,30,31,31,30,31,30,31};
    int mdays[]  = {31,28,31,30,31,30,31,31,30,31,30,31};
 
    leap = IsLeapYear(year);
    /* うるう年 */
    if (leap == true) {
        return lmdays[month - 1];
    /* うるう年以外 */
    } else {
        return mdays[month - 1];
    }
}

class Chart
{
  private:
    int w=0,oldh=0,oldw=0;
    bool start=false;
  public:
    void Chartprint(TFT_eSPI *tft,int val,int x,int y,uint32_t color);
};
void Chart::Chartprint(TFT_eSPI *tft,int val,int x,int y,uint32_t color)
{
  int h=tft->height()/2;
  h+=val-(x-h);
  if(!start)
  {
    oldh=h;
    start=true;
  }
  if(tft->height()<h)h=tft->height();
  else if(h<0)h=0;
  if(tft->width()<w)
  {
    w=y;
    oldw=y;
    tft->fillScreen(TFT_BLACK);
  }
  if(w<0)w=0;
  tft->drawLine(oldw,oldh,w,h,color);
  tft->drawLine(0,x,tft->width(),x,TFT_DARKGREY);
  oldw=w;
  oldh=h;
  w++;
}

inline String Week(TFT_eSPI *tft,uint8_t week,bool SHORT=false,bool colon=true)
{
  String Date="";
  switch(week)
  {
    default:
    case 0: Date=(SHORT)?"Sun":"Sunday";tft->setTextColor(TFT_RED, TFT_BLACK);break;
    case 1: Date=(SHORT)?"Mon":"Monday";tft->setTextColor(TFT_YELLOW, TFT_BLACK);break;
    case 2: Date=(SHORT)?"Tue":"Tuesday";tft->setTextColor(TFT_PINK, TFT_BLACK);break;
    case 3: Date=(SHORT)?"Wed":"Wednesday";tft->setTextColor(TFT_GREEN, TFT_BLACK);break;
    case 4: Date=(SHORT)?"Thu":"Thursday";tft->setTextColor(TFT_ORANGE, TFT_BLACK);break;
    case 5: Date=(SHORT)?"Fri":"Friday";tft->setTextColor(TFT_BLUE, TFT_BLACK);break;
    case 6: Date=(SHORT)?"Sat":"Saturday";tft->setTextColor(TFT_PURPLE, TFT_BLACK);break;
  }
  if(SHORT&&colon)Date+=".";
  return Date;
}
void wifitask(void *arg);


#if defined __cplusplus
}
#endif
#endif