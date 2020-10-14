#include "Mag.h"
Compass::Compass()
{
  inistart=false;
  up=false;
  reset();
}
void Compass::begin()
{
  EEPROM.begin(1028);
  b[0]=EEPROM.readFloat(addr0);
  b[1]=EEPROM.readFloat(addr1);
  b[2]=EEPROM.readFloat(addr2);
  b[3]=EEPROM.readFloat(addr3);
}
void Compass::update(float x, float y, float z)
{
  dx = x - b[0];
  dy = y - b[1];
  dz = z - b[2];
  f = dx*dx + dy*dy + dz*dz - b[3]*b[3];
  b[0] = b[0] + 4 * lr * f * dx;
  b[1] = b[1] + 4 * lr * f * dy;
  b[2] = b[2] + 4 * lr * f * dz;
  b[3] = b[3] + 4 * lr * f * b[3];
  up=false;
}
void Compass::save()
{
  if(!up)
  {
    EEPROM.writeFloat(addr0,b[0]);
    EEPROM.writeFloat(addr1,b[1]);
    EEPROM.writeFloat(addr2,b[2]);
    EEPROM.writeFloat(addr3,b[3]);
    EEPROM.commit();
    up=true;
  }
}
void Compass::reset()
{
  dx=dy=dz=f=0.0f;
  b[0]=b[1]=b[2]=0.0f;
  b[3]=1.0f;
}

Compass CO;