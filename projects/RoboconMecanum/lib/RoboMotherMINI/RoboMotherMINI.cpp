#include "RoboMotherMINI.h"

void OTA_Setup(){
    WiFi.mode(WIFI_AP_STA);
    //WiFi.begin(router_ssid, router_pw);
    if(WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.println("Router Connection Failed!");
        //WiFi.softAP(ap_ssid, ap_pw);
    }

    ArduinoOTA
        .onStart([]() {
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH)
            type = "sketch";
        else // U_SPIFFS
            type = "filesystem";

        // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
        Serial.println("Start updating " + type);
        })
        .onEnd([]() {
        Serial.println("\nEnd");
        })
        .onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
        })
        .onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
        else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
        else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
        else if (error == OTA_END_ERROR) Serial.println("End Failed");
        });

    ArduinoOTA.begin();

    Serial.println("Ready");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

}

bool em=false;

void e_stop(uint8_t sw)
{
    if(sw==e_ON)
        digitalWrite(POWER_CONTROL,HIGH);
    else
        digitalWrite(POWER_CONTROL,LOW);
}

MCP_CAN CAN0(DEV1);

void CAN_Init()
{
    pinMode(CAN_INT, INPUT);

    if(CAN0.begin(MCP_ANY, CAN_1000KBPS, MCP_20MHZ) == CAN_OK)
        Serial.println("MCP2515 Initialized Successfully!");
    else
        Serial.println("Error Initializing MCP2515...");
    
    if(CAN0.setMode(MCP_NORMAL) == CAN_OK)
        Serial.println("CAN0 setMode Successfully!");
    else
        Serial.println("Error CAN0 setMode");
    delay(1000);
}