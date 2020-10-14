#ifndef _hagure_lib_h
#define _hagure_lib_h
#include <Arduino.h>
#include "RoboMotherMINI.h"

#define RED 1
#define GREEN 2
#define BLUE 3
#define WHITE 4

#define set_10_ms 0
#define set_25_ms 1
#define set_50_ms 2
#define set_100_ms 3

#define RIGHT 0
#define LEFT 1
#define CENTER 2

/*--------------------------------------------------------------------------------------------------
  wait(); //データの送信を停止

  read(); //カラーデータを取得

  get(センサーの位置)
    センサーの位置 : RIGHT , CENTER , LEFT
    返り値 : 0[white] , 1[red] , 2[green] , 3[blue] , 4[other]
--------------------------------------------------------------------------------------------------*/

inline const char* color(int colornum)
{
  switch(colornum)
  {
    case RED:return "red";
    case GREEN:return "green";
    case BLUE:return "blue";
    case WHITE:return "white";
    default:return "other";
  }
}

class hagure_lib
{
  public:
    hagure_lib(long unsigned int raedress);
    void read(long unsigned int canId,unsigned char *buf);
    byte get(byte pos_sel);
  private:
    long unsigned int read_address;
    unsigned char buf_data[3]={0};
    byte count;
};

#endif