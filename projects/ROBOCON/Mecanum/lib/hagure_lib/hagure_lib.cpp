#include <Arduino.h>
#include "RoboMotherMINI.h"
#include "hagure_lib.h"

hagure_lib::hagure_lib(long unsigned int raedress)
{
  read_address = raedress;
}

void hagure_lib::read(long unsigned int canId,unsigned char *buf)
{
  if (canId == read_address)
  {
    for (count = 0; count < 3; count++)
    {
      if(buf[count]!=0)buf_data[count] = buf[count];
    }
  }
  else buf[0]=buf[1]=buf[2]=0;
}

byte hagure_lib::get(byte pos_sel)
{
  return buf_data[pos_sel];
}