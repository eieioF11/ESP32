#include "Sound.h"
#include "Timer.h"

#define SOUNDER 15
#define LEDC_CHANNEL_2     2
#define LEDC_TIMER_13_BIT  13
#define LEDC_BASE_FREQ     5000

int mc=0,c=0;
bool c1=false;
void noTone(int pin)
{
    ledcWriteTone(LEDC_CHANNEL_2, 0.0) ;
}

void tone(int pin,int freq)
{
    ledcSetup(LEDC_CHANNEL_2, LEDC_BASE_FREQ, LEDC_TIMER_13_BIT) ;
    ledcAttachPin(pin, LEDC_CHANNEL_2) ; // CH2をSOUNDERに
    ledcWriteTone(LEDC_CHANNEL_2, freq) ;
}

void melreset()
{
    noTone(beep);
    mc=0;
    c=0;
    c1=false;
}

void mel1()
{
    const int mel[17]={4,5,6,0,1,2,5,5,3,5,4,5,3,3,5,4,100};
    const int t[17]={10,10,10,20,10,15,43,10,10,10,24,10,10,10,5,10,10};
    //static int mc=0,c=0;static bool c1=false;
    if(c<10*t[mc])
    {
        if(c1==false)
        {
            switch(mel[mc])
            {
                case 0:tone(beep,(int)C_(5));break;
                case 1:tone(beep,(int)D_(5));break;
                case 2:tone(beep,(int)E_(5));break;
                case 3:tone(beep,(int)F_(4));break;
                case 4:tone(beep,(int)G_(4));break;
                case 5:tone(beep,(int)A_(4));break;
                case 6:tone(beep,(int)B_(4));break;
                default:break;
            }
            c1=true;
        }
    }
    else
    {
        noTone(beep);
        mc++;
        if(mc>16)mc=0;
        c=0;
        c1=false;
    }
    c++;
    /*
           GABC DE A   AFAG AFFAG
    octave 4445 55 5   4444 44444
    */
}
void mel2()
{
    const char mel[17]={'B','D','A','G','A','B','D','A','B','D','a','g','D','C','B','A',100};
    const int t[17]={20,10,20,6,6,20,10,25,20,10,20,10,20,5,5,20,10};
    //static int mc=0,c=0;static bool c1=false;
    if(c<10*t[mc])
    {
        if(c1==false)
        {
            switch(mel[mc])
            {
                case 'F':tone(beep,(int)F_(3));break;
                case 'G':tone(beep,(int)G_(3));break;
                case 'A':tone(beep,(int)A_(3));break;
                case 'B':tone(beep,(int)B_(3));break;
                case 'C':tone(beep,(int)C_(4));break;
                case 'D':tone(beep,(int)D_(4));break;
                case 'E':tone(beep,(int)E_(4));break;
                case 'f':tone(beep,(int)F_(4));break;
                case 'g':tone(beep,(int)G_(4));break;
                case 'a':tone(beep,(int)A_(4));break;
                case 'b':tone(beep,(int)B_(4));break;
                default:break;
            }
            c1=true;
        }
    }
    else
    {
        noTone(beep);
        mc++;
        if(mc>16)mc=0;
        c=0;
        c1=false;
    }
    c++;
}
void mel3()
{
    const char mel[17]={'D','E','F','G','c','d','G','E','D','G','E','D','C','D','G','E',0};
    const int t[17]={20,6,6,20,10,20,10,20,20,10,20,5,5,20,10,10,10};
    //static int mc=0,c=0;static bool c1=false;
    if(c<10*t[mc])
    {
        if(c1==false)
        {
            switch(mel[mc])
            {
                case 'C':tone(beep,(int)C_(4));break;
                case 'D':tone(beep,(int)D_(4));break;
                case 'E':tone(beep,(int)E_(4));break;
                case 'F':tone(beep,(int)F_(4));break;
                case 'G':tone(beep,(int)G_(4));break;
                case 'A':tone(beep,(int)A_(4));break;
                case 'B':tone(beep,(int)B_(4));break;
                case 'c':tone(beep,(int)C_(5));break;
                case 'd':tone(beep,(int)D_(5));break;
                case 'e':tone(beep,(int)E_(5));break;
                default:break;
            }
            c1=true;
        }
    }
    else
    {
        noTone(beep);
        mc++;
        if(mc>16)mc=0;
        c=0;
        c1=false;
    }
    c++;
}
bool conectbeep(int port)
{
    const int mel[4]={246,0,493,0};
    const int t[4]={5,3,10,5};
    bool end=false;
    if(c<10*t[mc])
    {
        if(c1==false)
        {
            switch(mel[mc])
            {
                case 246:tone(beep,246);digitalWrite(port, HIGH);break;
                case 493:tone(beep,493);digitalWrite(port, HIGH);break;
                default:noTone(beep);digitalWrite(port, LOW);break;
            }
            c1=true;
        }
    }
    else
    {
        noTone(beep);
        mc++;
        if(mc>3)
        {
            mc=0;
            end=true;
        }
        c=0;
        c1=false;
    }
    c++;
    return end;
}


bool ONbeep()
{
    const int mel[4]={1,0};
    const int t[4]={2,2};
    bool end=false;
    if(c<10*t[mc])
    {
        if(c1==false)
        {
            switch(mel[mc])
            {
                case 1:tone(beep,B_(5));break;
                default:noTone(beep);break;
            }
            c1=true;
        }
    }
    else
    {
        noTone(beep);
        mc++;
        if(mc>1)
        {
            mc=0;
            end=true;
        }
        c=0;
        c1=false;
    }
    c++;
    return end;
}

bool unmountbeep(int port)
{
    const int mel[4]={493,0,246,0};
    const int t[4]={5,3,10,10};
    bool end=false;
    if(c<10*t[mc])
    {
        if(c1==false)
        {
            switch(mel[mc])
            {
                case 246:tone(beep,246);digitalWrite(port, HIGH);break;
                case 493:tone(beep,493);digitalWrite(port, HIGH);break;
                default:noTone(beep);digitalWrite(port, LOW);break;
            }
            c1=true;
        }
    }
    else
    {
        noTone(beep);
        mc++;
        if(mc>3)
        {
            mc=0;
            end=true;
        }
        c=0;
        c1=false;
    }
    c++;
    return end;
}
bool errorbeep()
{
    const char mel[4]={'B',0,'B',0};
    const int t[4]={2,2,10,0};
    bool end=false;
    if(c<10*t[mc])
    {
        if(c1==false)
        {
            switch(mel[mc])
            {
                case 'B':tone(beep,B_(5));break;
                default:noTone(beep);break;
            }
            c1=true;
        }
    }
    else
    {
        noTone(beep);
        mc++;
        if(mc>3)
        {
            mc=0;
            end=true;
        }
        c=0;
        c1=false;
    }
    c++;
    return end;
}
bool startbeep()
{
    const char mel[7]={'b',0,'b',0,'b',0,'B'};
    const int t[7]={3,15,3,15,3,15,15};
    bool end=false;
    if(c<10*t[mc])
    {
        if(c1==false)
        {
            switch(mel[mc])
            {
                case 'B':tone(beep,B_(4));break;
                case 'b':tone(beep,B_(3));break;
                default:noTone(beep);break;
            }
            c1=true;
        }
    }
    else
    {
        noTone(beep);
        mc++;
        if(mc>6)
        {
            mc=0;
            end=true;
        }
        c=0;
        c1=false;
    }
    c++;
    return end;
}
bool winxpstartbeep()
{
    const char mel[7]={'e','E','B','A','E','e','B'};
    const int t[7]={5,3,5,7,3,5,15};
    bool end=false;
    if(c<10*t[mc])
    {
        if(c1==false)
        {
            switch(mel[mc])
            {
                case 'A':tone(beep,A_(4));break;
                case 'B':tone(beep,B_(4));break;
                case 'E':tone(beep,E_(4));break;
                case 'e':tone(beep,E_(5));break;
                default:noTone(beep);break;
            }
            c1=true;
        }
    }
    else 
    {
        noTone(beep);
        mc++;
        if(mc>6)
        {
            mc=0;
            end=true;
        }
        c=0;
        c1=false;
    }
    c++;
    return end;
}

bool BTerrorbeep()
{
    const char mel[4]={'B',0,'B',0};
    const int t[4]={2,10,2,10};
    bool end=false;
    if(c<10*t[mc])
    {
        if(c1==false)
        {
            switch(mel[mc])
            {
                case 'B':tone(beep,B_(5));break;
                default:noTone(beep);break;
            }
            c1=true;
        }
    }
    else
    {
        noTone(beep);
        mc++;
        if(mc>3)
        {
            mc=0;
            end=true;
        }
        c=0;
        c1=false;
    }
    c++;
    return end;
}