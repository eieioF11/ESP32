#ifndef I_ENCODER_H_
#define I_ENCODER_H_
#include "mcp_can.h"
#include <stdio.h>
#include "Arduino.h"

#define IErIDX 0x111
#define IErIDY 0x110
#define resetID 0x000

#if defined __cplusplus
extern "C" {
#endif

class Installation_encoder
{
  private:
    MCP_CAN *can0;
    union CanData
    {
      float data;
      uint8_t IE[4];
    } c;
    unsigned char candata[8];
    bool re;
    float xinit,yinit;
  public:
    Installation_encoder(MCP_CAN *can0_);
    void communication(long unsigned int ID,unsigned char len,unsigned char *buf);
    void setlocation(float X,float Y);
    void readlocation(double *location);
    void reset();
};
#if defined __cplusplus
}
#endif
#endif