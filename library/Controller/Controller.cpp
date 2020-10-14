#include "Controller.h"
#include "Setup.h"

WiFiUDP udp;

bool beeps=false;

byte send_byte = 102;//送信する値
bool connect=false;
int melodysel=0;
uint16_t stledinterval=500;
bool OTAFLAG=false;

#define BottonBeep XPStartBeep+1
bool bottonbeep()
{
    static int c=0;
    bool end=false;
    if(c<200)
        tone(beep,B_(5));
    else
    {
        noTone(beep);
        end=true;
    }
    c++;
    if(end)
        c=0;
    return end;
}

void melodytask(void *arg)
{
    int stc=0,ledsec=0;
    portTickType lt = xTaskGetTickCount();
    while(1)
    {
        if(stc>100)
        {
            switch(melodysel)
            {
                case 1:mel1();break;
                case 2:mel2();break;
                case 3:mel3();break;
                case BottonBeep:if(bottonbeep())melodysel=0;break;
                case ConectBeep:if(conectbeep(StatusLED))melodysel=0;break;
                case UnmountBeep:if(unmountbeep(StatusLED))melodysel=0;break;
                case ErrorBeep:if(errorbeep())melodysel=0;break;
                case StartBeep:if(startbeep())melodysel=0;break;
                case XPStartBeep:if(winxpstartbeep())melodysel=0;break;
                case ConectBeep-1:
                default:melodysel=0;break;
            }
        }
        else stc++;
        if(melodysel!=ConectBeep&&melodysel!=UnmountBeep&&!OTAFLAG)
        {
            if(ledsec>(stledinterval/2))
                digitalWrite(StatusLED, HIGH);
            else
                digitalWrite(StatusLED, LOW);
            if(ledsec>stledinterval)ledsec=0;
            else ledsec++;
        }
        vTaskDelayUntil(&lt,5/portTICK_RATE_MS);
    }
}

void led3ota(void *arg)
{
    while(1)
    {
        digitalWrite(StatusLED,LOW);
        wait(0.05);
        digitalWrite(StatusLED, HIGH);
        wait(0.05);
    }
}

Timer t[0];
int mdc=0,udpdata=0;
void ControllerUpdate()
{
    static int stopt=0,stc=0;static bool send=false;
    if(!digitalRead(up)){send_byte=49;send=false;}
    else if(!digitalRead(down)){send_byte=52;send=false;}
    else if(!digitalRead(right)){send_byte=51;send=false;}
    else if(!digitalRead(left)){send_byte=50;send=false;}
    else if(!digitalRead(A)){send_byte=53;send=false;}
    else if(!digitalRead(B)){send_byte=54;send=false;}
    else if(!digitalRead(C)){send_byte=55;send=false;}
    else if(!digitalRead(D)){send_byte=56;send=false;}
    else if(!digitalRead(set)){send_byte=97;send=false;}
    else if(stopt>30&&send_byte!=102){send_byte=48;stopt=0;send=false;}
    else if(!send)stopt++;
    if(!digitalRead(stop)){send_byte=102;send=false;}
    if(!mount())
        mdc++;
    else
    {
        if(mdc>10)
        {
            melodysel++;
            melreset();
        }
        mdc=0;
    }
    
    if(!digitalRead(stsw)){send_byte=193;send=false;melodysel=BottonBeep;}
    if (udp.parsePacket())udpdata=udp.read();
    if(connect&&send==false)
    {
        if(udpdata&&(send_byte!=102||send_byte!=48))
            melodysel=ErrorBeep;
        udp.beginPacket(ip,LocalPort);
        udp.write(send_byte);
        udp.endPacket();
        printf("udp send data=%d\n",send_byte);
        if(send_byte==102||send_byte==48)stc++;
        if(stc>10){send=true;stc=0;}
        printf("%d\n",send);
    }
    //printf("udp data=%d\n",udpdata);
    /*OTA*/
    ArduinoOTA.handle();
    delay(Delta_T*100);//Execution interval
}
void Controllerinit()
{
    Serial.begin(115200);
    tone(beep,246);
    pinMode(up, INPUT);
    pinMode(down, INPUT);
    pinMode(right, INPUT);
    pinMode(left, INPUT);
    pinMode(A, INPUT);
    pinMode(B, INPUT);
    pinMode(C, INPUT);
    pinMode(D, INPUT);
    pinMode(set, INPUT);
    pinMode(stop, INPUT);
    pinMode(stsw, INPUT_PULLUP);
    pinMode(StatusLED, OUTPUT);

    xTaskCreatePinnedToCore(melodytask,"Melody task",1024,NULL,0,NULL,1);

    WiFi.mode(WIFI_STA);
    WiFi.config(ip2, gateway, subnet, DNS);
    WiFi.begin(ESP_SSID,ESP_WIFIPASS);
    while (WiFi.status() != WL_CONNECTED) 
    {
        Serial.print(".");
        digitalWrite(StatusLED, HIGH);
        delay(100);
        Serial.print(".");
        digitalWrite(StatusLED, LOW);
        delay(100);
    }
    melodysel=ConectBeep;
    connect=true;
    Serial.println("Succeed to Wifi connecting");
    ArduinoOTA.begin();
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    udp.begin(LocalPort);
    // Port defaults to 3232                            // ArduinoOTA.setPort(3232);
    // Hostname defaults to esp3232-[MAC]               // ArduinoOTA.setHostname("myesp32");
    // No authentication by default                     // ArduinoOTA.setPassword("admin");
    // Password can be set with it's md5 value as well  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
    // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");
    ArduinoOTA.onStart([]() {
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH)
            type = "sketch";
        else // U_SPIFFS
            type = "filesystem";
        // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
        Serial.println("Start updating " + type);
        OTAFLAG=true;
        melodysel=StartBeep;
        xTaskCreatePinnedToCore(led3ota,"led3ota",1024,NULL,1,NULL,1);
    });
    ArduinoOTA.onEnd([]() {
        Serial.println("\nEnd");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR)         Serial.println("Auth Failed");
        else if (error == OTA_BEGIN_ERROR)   Serial.println("Begin Failed");
        else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
        else if (error == OTA_END_ERROR)     Serial.println("End Failed");
    });

    Serial.printf("ESP: Finish init\n");
    digitalWrite(StatusLED,LOW);
    noTone(beep);
}