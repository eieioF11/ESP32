#include "ESP_PS3.h"

#if (ESP_MODE == ESP_PS3)
bool conect=false;
uint16_t PS3button=0b0000000000000000;
bool psbt=false;
int8_t lx=0,ly=0,rx=0,ry=0;
String PS3Debug="";

void notify()
{
    //---------- Digital select/start/ps button events ---------
    if( Ps3.event.button_down.select&&!(PS3button&1ull<<15))
        PS3button+=0b1000000000000000;;
    if( Ps3.event.button_up.select&&(PS3button&1ull<<15))
        PS3button-=0b1000000000000000;

    if( Ps3.event.button_down.start&&!(PS3button&1ull<<14))
        PS3button+=0b0100000000000000;
    if( Ps3.event.button_up.start&&(PS3button&1ull<<14))
        PS3button-=0b0100000000000000;

    if( Ps3.event.button_down.ps)
        psbt=true;
    if( Ps3.event.button_up.ps)
        psbt=false;

   //--------------- Digital stick button events --------------
    if( Ps3.event.button_down.l3&&!(PS3button&1ull<<13))
        PS3button+=0b0010000000000000;
    if( Ps3.event.button_up.l3&&(PS3button&1ull<<13))
        PS3button-=0b0010000000000000;

    if( Ps3.event.button_down.r3&&!(PS3button&1ull<<12))
        PS3button+=0b0001000000000000;
    if( Ps3.event.button_up.r3&&(PS3button&1ull<<12))
        PS3button-=0b0001000000000000;

    //------------- Digital shoulder button events -------------
    if( Ps3.event.button_down.l1&&!(PS3button&1ull<<11))
        PS3button+=0b0000100000000000;
    if( Ps3.event.button_up.l1&&(PS3button&1ull<<11))
        PS3button-=0b0000100000000000;

    if( Ps3.event.button_down.r1&&!(PS3button&1ull<<10))
        PS3button+=0b0000010000000000;
    if( Ps3.event.button_up.r1&&(PS3button&1ull<<10))
        PS3button-=0b0000010000000000;

    //-------------- Digital trigger button events -------------
    if( Ps3.event.button_down.l2&&!(PS3button&1ull<<9))
        PS3button+=0b0000001000000000;
    if( Ps3.event.button_up.l2&&(PS3button&1ull<<9))
        PS3button-=0b0000001000000000;

    if( Ps3.event.button_down.r2&&!(PS3button&1ull<<8))
        PS3button+=0b0000000100000000;
    if( Ps3.event.button_up.r2&&(PS3button&1ull<<8))
        PS3button-=0b0000000100000000;

    //--------------- Digital D-pad button events --------------
    if( Ps3.event.button_down.up&&!(PS3button&1ull<<7))
        PS3button+=0b0000000010000000;
    if( Ps3.event.button_up.up&&(PS3button&1ull<<7))
        PS3button-=0b0000000010000000;

    if( Ps3.event.button_down.down&&!(PS3button&1ull<<6))
        PS3button+=0b0000000001000000;
    if( Ps3.event.button_up.down&&(PS3button&1ull<<6))
        PS3button-=0b0000000001000000;

    if( Ps3.event.button_down.right&&!(PS3button&1ull<<5))
        PS3button+=0b0000000000100000;
    if( Ps3.event.button_up.right&&(PS3button&1ull<<5))
        PS3button-=0b0000000000100000;

    if( Ps3.event.button_down.left&&!(PS3button&1ull<<4))
        PS3button+=0b0000000000010000;
    if( Ps3.event.button_up.left&&(PS3button&1ull<<4))
        PS3button-=0b0000000000010000;

    //--- Digital cross/square/triangle/circle button events ---
    if( Ps3.event.button_down.triangle&&!(PS3button&1ull<<3))
        PS3button+=0b0000000000001000;
    if( Ps3.event.button_up.triangle&&(PS3button&1ull<<3))
        PS3button-=0b0000000000001000;

    if( Ps3.event.button_down.circle&&!(PS3button&1ull<<2))
        PS3button+=0b0000000000000100;
    if( Ps3.event.button_up.circle&&(PS3button&1ull<<2))
        PS3button-=0b0000000000000100;

    if( Ps3.event.button_down.cross&&!(PS3button&1ull<<1))
        PS3button+=0b0000000000000010;
    if( Ps3.event.button_up.cross&&(PS3button&1ull<<1))
        PS3button-=0b0000000000000010;

    if( Ps3.event.button_down.square&&!(PS3button&1ull<<0))
        PS3button+=0b0000000000000001;
    if( Ps3.event.button_up.square&&(PS3button&1ull<<0))
        PS3button-=0b0000000000000001;

    //---------------- Analog stick value events ---------------
   if( abs(Ps3.event.analog_changed.stick.lx) + abs(Ps3.event.analog_changed.stick.ly) > 2 ){
        lx=Ps3.data.analog.stick.lx;
        ly=Ps3.data.analog.stick.ly;
    }

   if( abs(Ps3.event.analog_changed.stick.rx) + abs(Ps3.event.analog_changed.stick.ry) > 2 ){
        rx=Ps3.data.analog.stick.rx;
        ry=Ps3.data.analog.stick.ry;
   }
}

void onConnect()
{
    conect=true;
    Serial.println("Connected.");
}

void PS3init()
{
    Ps3.attach(notify);
    Ps3.attachOnConnect(onConnect);
    Ps3.begin(MAC_ADD);
}
int player = 0;
Timer ut;
void PS3Update()
{
    if(!Ps3.isConnected())
        return;
    if(ut.stand_by(0.5f))
    {
        //-------------------- Player LEDs -------------------
        Ps3.setPlayer(player);
        player += 1;
        if(player > 10) player = 0;
    }
}
bool PS3Controller(float *Vx,float *Vy,float *Angular,bool stick)
{
    bool b=false;
    PS3Debug="";
    for(int i=0;i<16;i++)
    {
        switch((int)PS3button&1ull<<i)//0b/select/start/l3/r3/l1/r1/l2/r2/up/down/right/left/△/〇/×/□/
        {
            case 0b1000000000000000:St.addprintf(&PS3Debug,"select\n");break;
            case 0b0100000000000000:St.addprintf(&PS3Debug,"start\n");break;
            case 0b0010000000000000:St.addprintf(&PS3Debug,"l3\n");b=true;break;
            case 0b0001000000000000:St.addprintf(&PS3Debug,"r3\n");break;
            case 0b0000100000000000:St.addprintf(&PS3Debug,"l1\n");*Angular=-0.5;break;
            case 0b0000010000000000:St.addprintf(&PS3Debug,"r1\n");*Angular=0.5;break;
            case 0b0000001000000000:St.addprintf(&PS3Debug,"l2\n");break;
            case 0b0000000100000000:St.addprintf(&PS3Debug,"r2\n");break;
            case 0b0000000010000000:St.addprintf(&PS3Debug,"up\n");*Vy=0.5;break;
            case 0b0000000001000000:St.addprintf(&PS3Debug,"down\n");*Vy=-0.5;break;
            case 0b0000000000100000:St.addprintf(&PS3Debug,"right\n");*Vx=0.5;break;
            case 0b0000000000010000:St.addprintf(&PS3Debug,"left\n");*Vx=-0.5;break;
            case 0b0000000000001000:St.addprintf(&PS3Debug,"triangle\n");break;
            case 0b0000000000000100:St.addprintf(&PS3Debug,"circle\n");break;
            case 0b0000000000000010:St.addprintf(&PS3Debug,"cross\n");break;
            case 0b0000000000000001:St.addprintf(&PS3Debug,"square\n");break;
        }
    }
    if(!PS3button&&stick)
    {
        *Vx = float(lx)/128.0f;
        *Vy = (float(ly)/128.0f)*-1.f;
        *Angular=float(rx)/128.0f;
    }
    else if(!PS3button)
        *Vx=*Vy=*Angular=0.0f;
    if(b)
    {
        lx=ly=rx=ry=0;
        *Vx=*Vy=*Angular=0.0f;
    }
    return b;
}
bool PS3ControllerTw(float *Vx,float *Vy,float *Angular,bool stick)
{
    bool b=false;
    PS3Debug="";
    for(int i=0;i<16;i++)
    {
        switch((int)PS3button&1ull<<i)//0b/select/start/l3/r3/l1/r1/l2/r2/up/down/right/left/△/〇/×/□/
        {
            case 0b1000000000000000:St.addprintf(&PS3Debug,"select\n");break;
            case 0b0100000000000000:St.addprintf(&PS3Debug,"start\n");break;
            case 0b0010000000000000:St.addprintf(&PS3Debug,"l3\n");b=true;break;
            case 0b0001000000000000:St.addprintf(&PS3Debug,"r3\n");break;
            case 0b0000100000000000:St.addprintf(&PS3Debug,"l1\n");*Angular=0.5;break;
            case 0b0000010000000000:St.addprintf(&PS3Debug,"r1\n");*Angular=-0.5;break;
            case 0b0000001000000000:St.addprintf(&PS3Debug,"l2\n");break;
            case 0b0000000100000000:St.addprintf(&PS3Debug,"r2\n");break;
            case 0b0000000010000000:St.addprintf(&PS3Debug,"up\n");*Vx=0.5;break;
            case 0b0000000001000000:St.addprintf(&PS3Debug,"down\n");*Vx=-0.5;break;
            case 0b0000000000100000:St.addprintf(&PS3Debug,"right\n");*Angular=-0.5;break;
            case 0b0000000000010000:St.addprintf(&PS3Debug,"left\n");*Angular=0.5;break;
            case 0b0000000000001000:St.addprintf(&PS3Debug,"triangle\n");break;
            case 0b0000000000000100:St.addprintf(&PS3Debug,"circle\n");break;
            case 0b0000000000000010:St.addprintf(&PS3Debug,"cross\n");break;
            case 0b0000000000000001:St.addprintf(&PS3Debug,"square\n");break;
        }
    }
    if(!PS3button&&stick)
    {
        *Vx = (float(ly)/128.0f)*-1.f;
        *Angular = ((float(lx)/128.0f)+(float(rx)/128.0f))*-1.f;
    }
    else if(!PS3button)
        *Vx=*Vy=*Angular=0.0f;
    if(b)
    {
        lx=ly=rx=ry=0;
        *Vx=*Vy=*Angular=0.0f;
    }
    return b;
}
int8_t PS3stick(ps3stickmode mode)
{
    switch(mode)
    {
        case lX:return lx;
        case lY:return ly;
        case rX:return rx;
        case rY:return ry;
    }
    return -1;
}
uint16_t PS3Button()
{
    return PS3button;
}
bool PS3PSButton()
{
    return psbt;
}
bool PS3Conect()
{
    return conect;
}
#endif