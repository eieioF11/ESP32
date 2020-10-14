#ifndef _robo_h
#define _robo_h
#include <WiFi.h>
#include <WiFiUdp.h>

const IPAddress ip(192, 168, 4, 2);      
const IPAddress subnet(255, 255, 255, 0);

class Remote {
    int portData=10000;
public:
    Remote();
    Remote(int);
    void setup(const char *id,const char *pass);
    void setup(const char *id,const char *pass,IPAddress x,IPAddress y);
    int available();
    uint8_t read();
};

#endif