#include "I_encoder.h"

Installation_encoder::Installation_encoder(MCP_CAN *can0_)
{
  can0=can0_;
  re=false;
  xinit=yinit=0;
}
void Installation_encoder::communication(long unsigned int ID,unsigned char len,unsigned char *buf)
{
  if(re)
  {
    printf("reset\n");
    can0->sendMsgBuf(resetID,1,candata);
    candata[0]=0;
    re=false;
  }
  //printf("ID %x len %d\n",ID,len);
  if(ID==IErIDX&&len==4)
  {
    for(int i=0;i<len;i++)
    {
      //printf("%d,",buf[i]);
      candata[i]=buf[i];
    }
  }
  if(ID==IErIDY&&len==4)
  {
    for(int i=0;i<len;i++)
    {
      //printf("%d,",buf[i]);
      candata[i+4]=buf[i];
    }
    //printf("  ID %x len %d\n",ID,len);
  }
}
void Installation_encoder::setlocation(float X,float Y)
{
  xinit+=X;
  yinit+=Y;
}
void Installation_encoder::readlocation(double *location)
{
  c.IE[0]=candata[0];
  c.IE[1]=candata[1];
  c.IE[2]=candata[2];
  c.IE[3]=candata[3];
  location[0]=(double)c.data+xinit;
  c.IE[0]=candata[4];
  c.IE[1]=candata[5];
  c.IE[2]=candata[6];
  c.IE[3]=candata[7];
  location[1]=(double)c.data+yinit;
}
void Installation_encoder::reset()
{
  candata[0]=193;
  re=true;
  xinit=yinit=0;
}