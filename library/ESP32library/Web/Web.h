#ifndef __WEB_H__
#define __WEB_H__
#include <Arduino.h>
#include <WiFi.h>
#include "WiFiClientSecure.h"
#include "WebSocketsServer.h"
#include "ESP32WebServer.h"
#include <ESPmDNS.h>
#include <SPIFFS.h>
#include <FS.h>
#include <string.h>
#include <map>
#include "Setup.h"
#define USE_SERIAL Serial

//const IPAddress ip(192,168,30,3);
//const IPAddress subnet(255,255,255,0);

//const char * ssid = "ESP-ROBOv2";
ESP32WebServer Server (80);
WebSocketsServer webSocket = WebSocketsServer(81);
bool IsConnected;

//-----------------------------------------------------------------------------

class WebObjectAdmin{
    public:
    static void BroadcastAllObjects(){
        for(auto itr = WebObjects.begin(); itr != WebObjects.end(); ++itr)
            printf("ID:%d = %x\n", itr->first, *(int*)(itr->second));
    }

    static void Receive(uint8_t id, void* rcvData, size_t n){
        memcpy(WebObjects[id], rcvData, n);
    }
    protected:
    static std::map<int, void*> WebObjects;

};

template<class ObjType>class WebObject : public WebObjectAdmin{
    public:
    WebObject();
    WebObject(uint8_t id);
    void SetID(uint8_t id);
    
    //~WebObject();

    void SetData(ObjType sData);
    WebObject& operator=(ObjType sData);

    ObjType GetData();
    operator ObjType();

    //void* onChange(void* rcvData);

    private:
    ObjType Data;
    uint8_t ID;
};

//-------------------------Implementation--------------------------------------------
std::map<int, void*> WebObjectAdmin::WebObjects;

template<class ObjType>
WebObject<ObjType>::WebObject(){}

template<class ObjType>
WebObject<ObjType>::WebObject(uint8_t id){
    SetID(id);
}

template<class ObjType>
void WebObject<ObjType>::SetID(uint8_t id){
    ID = id;
    WebObjects[ID] = &Data;
}

template<class ObjType>
void WebObject<ObjType>::SetData(ObjType sData){
    Data = sData;
}

template<class ObjType>
WebObject<ObjType>& WebObject<ObjType>::operator=(ObjType sData){
    SetData(sData);
    return *this;
}

template<class ObjType>
ObjType WebObject<ObjType>::GetData(){
    return Data;
}

template<class ObjType>
WebObject<ObjType>::operator ObjType(){
    return Data;
}

//-----------------------------------------------------------------------------

void WebSetup();
void WebSend(uint8_t* data, size_t len);
int WebClients();

void WebLoop(void *arg);
String getContentType(String filename);
bool handleFileRead(String path);
void handleNotFound();
void hexdump(const void *mem, uint32_t len, uint8_t cols);
void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length);
//extern bool IsConnected;

void WebSetup(){
  SPIFFS.begin();
  /*WiFi.setTxPower(WIFI_POWER_MINUS_1dBm);

  WiFi.softAP(ssid);
  delay(100);
  WiFi.softAPConfig(ip, ip, subnet);*/

  Server.onNotFound (handleNotFound);
  Server.begin();

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  xTaskCreatePinnedToCore(WebLoop, "WebLoop", 8192, NULL, 2, NULL, 0);  
}

void WebLoop(void *arg){
  while(1){
    Server.handleClient();
    webSocket.loop();
    delay(1);
  }
}

int WebClients(){
  return webSocket.connectedClients();
}

void WebSend(uint8_t * data, size_t len){
  webSocket.broadcastBIN(data, len);
}

String getContentType(String filename){
  if(filename.endsWith(".html") || filename.endsWith(".htm")) return "text/html";
  else if(filename.endsWith(".css")) return "text/css";
  else if(filename.endsWith(".js")) return "application/javascript";
  else if(filename.endsWith(".png")) return "image/png";
  else if(filename.endsWith(".gif")) return "image/gif";
  else if(filename.endsWith(".jpg")) return "image/jpeg";
  else return "text/plain";
}

bool handleFileRead(String path){
  Serial.println("handleFileRead: trying to read" + path);
  if(path.endsWith("/"))
    path += "index.html";
  String contentType = getContentType(path);
  if(SPIFFS.exists(path)){
    File file = SPIFFS.open(path, FILE_READ);
    Server.streamFile(file, contentType);
    file.close();
    Serial.println("handleFileRead sent" + path);
    return true; 
  }
  else{
    Serial.println("handleFileReead 404 not found");
    Server.send(404, "text/plain", "ESP: 404 not found");
    return false;
  }
}

void handleNotFound() {
  if(!handleFileRead(Server.uri())){
    printf("404 not found\n");
    Server.send(404, "test/plain", "File not found in Dongbeino...");
  }
}

void hexdump(const void *mem, uint32_t len, uint8_t cols = 16) {
	const uint8_t* src = (const uint8_t*) mem;
	//USE_SERIAL.printf("\n[HEXDUMP] Address: 0x%08X len: 0x%X (%d)", (ptrdiff_t)src, len, len);
	for(uint32_t i = 0; i < len; i++) {
		if(i % cols == 0) {
			//USE_SERIAL.printf("\n[0x%08X] 0x%08X: ", (ptrdiff_t)src, i);
		}
		//USE_SERIAL.printf("%02X ", *src);
		src++;
	}
	//USE_SERIAL.printf("\n");
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
    switch(type) {
        case WStype_DISCONNECTED:{
            IsConnected = false;
            //USE_SERIAL.printf("[%u] Disconnected!\n", num);
          }
            break;
        case WStype_CONNECTED:
            {
                IsConnected = true;
                IPAddress ip = webSocket.remoteIP(num);
                //USE_SERIAL.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
            }
            break;
        case WStype_TEXT:
            //USE_SERIAL.printf("[%u] get Text: %s\n", num, payload);
            
            break;
        case WStype_BIN:
            //USE_SERIAL.printf("[%u] get binary length: %u\n", num, length);
            hexdump(payload, length);
            WebObjectAdmin::Receive(payload[0], payload+1, length);
            break;
		case WStype_ERROR:			
		case WStype_FRAGMENT_TEXT_START:
		case WStype_FRAGMENT_BIN_START:
		case WStype_FRAGMENT:
		case WStype_FRAGMENT_FIN:
			break;
    }
}

#endif
