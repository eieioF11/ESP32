#include "remote_switch.h"
WiFiUDP udp;
Remote::Remote(){
    
}
Remote::Remote(int port){
    portData=port;
}
void Remote::setup(const char *id,const char *pass){
    WiFi.softAP(id,pass,7,1,1);
    WiFi.softAPConfig(ip, ip, subnet);
    udp.begin(portData);
}

void Remote::setup(const char *id,const char *pass,IPAddress x,IPAddress y){
    WiFi.softAP(id,pass);
    WiFi.softAPConfig(x, x, y);
    udp.begin(portData);
}

int Remote::available(){
    return udp.parsePacket();
}

uint8_t Remote::read(){
    return udp.read();
}