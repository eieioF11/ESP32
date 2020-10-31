#include "ESP32mother.h"
#include "Setup.h"
#include "Mbed.h"

#if (ESP_MODE != ESP_PS3)
#define wificonNUM 1
WiFiUDP udp;
#endif
ESP32Mather ESP32M;

bool OTAFLAG=false;

float Vx=0.0f;
float Vy=0.0f;
float Angular=0.0f;

int mode,mdc=0;
int sel=0,melodysel=0;
const char*udpmode=" ";
i2c_err_t I2C_Error;
uint16_t stledinterval=500;

String sdprint="/";

/*beep&melody*/

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
                case Onbeep:if(ONbeep())melodysel=0;break;
                case ConectBeep:if(conectbeep(StatusLED))melodysel=0;break;
                case UnmountBeep:if(unmountbeep(StatusLED))melodysel=0;break;
                case ErrorBeep:if(errorbeep())melodysel=0;break;
                case StartBeep:if(startbeep())melodysel=0;break;
                case XPStartBeep:if(winxpstartbeep())melodysel=0;break;
                case Onbeep-1:
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

/*I2CTask*/
#if(I2CPORT == ON)
fptr_vv_t ESPtask_i2c_init = NULL;
fptr_vv_t ESPtask_i2c_while = [](){};

void ESPtask_i2c(void *arg)
{
    pinMode(ESP_SDA,PULLUP);
    pinMode(ESP_SCL,PULLUP);
    Wire.begin(ESP_SDA,ESP_SCL,400000);
    if(ESPtask_i2c_init != NULL)
        ESPtask_i2c_init();
    portTickType lt = xTaskGetTickCount();
    while(1)
    {
        vTaskDelayUntil(&lt, (sensor_interval*100)/portTICK_RATE_MS);
        if(!OTAFLAG)
            ESPtask_i2c_while();
    }
}
#endif

/*Gain setting*/
void setgein()
{
    #if (SD == ON)
    FILE* f = fopen("/sdcard/Gain setting.txt", "w");
    if (f == NULL)
    {
        sdprint="SDerroe/";
        stledinterval=100;
    }
    else
    {
        fprintf(f,"%f\n%f\n%f\n\n%f\n%f\n%f\n",((ESPMotor*)md[0])->Kp,((ESPMotor*)md[0])->Ki,((ESPMotor*)md[0])->Kd,((ESPMotor*)md[1])->Kp,((ESPMotor*)md[1])->Ki,((ESPMotor*)md[1])->Kd);
        sdprint="SDsave/";
    }
    fclose(f);
    #endif
}
void setfile_read()
{
    #if (SD == ON)
    if(SDCH()==0)
    {
        FILE *f = fopen("/sdcard/Gain setting.txt", "r");
        if (f == NULL)
        {
            stledinterval=100;
            Serial.println("Failed to open file for writing");
        }
        else
        {
            fscanf(f,"%f\n",&((ESPMotor*)md[0])->Kp);
            fscanf(f,"%f\n",&((ESPMotor*)md[0])->Ki);
            fscanf(f,"%f\n",&((ESPMotor*)md[0])->Kd);
            fscanf(f,"\n%f\n",&((ESPMotor*)md[1])->Kp);
            fscanf(f,"%f\n",&((ESPMotor*)md[1])->Ki);
            fscanf(f,"%f\n",&((ESPMotor*)md[1])->Kd);
        }
        fclose(f);
        ((ESPMotor*)md[0])->set();
        ((ESPMotor*)md[1])->set();
    }
    #endif
}
#if (SD == ON)
/*SD Log*/
void SDlog(void *arg)
{
    char FILEname[25];
    if(sd.begin(SD_MISO,SD_MOSI,SD_SCLK,SD_CS))tone(beep,110);
    setfile_read();
    if(SDCH()==0)
    {
        uint8_t mem=0;
        FILE* f = fopen("/sdcard/memo.txt", "r");
        if (f == NULL)
            Serial.println("Failed to open file for writing");
        else
            fscanf(f,"%d",(int*)&mem);
        fclose(f);
        mem++;
        sprintf(FILEname,"/sdcard/log/log_No%d.csv",mem);
        printf("Create %s file!!\n\r",FILEname);
        f = fopen(FILEname,"w");
        if (f == NULL)
            Serial.println("Failed to open file for writing");
        else
        {
            fprintf(f,"No.%d\n<Record start>\n\n",mem);
            fprintf(f,"time[ms],CPU[degree Celsius],Outside[degree Celsius],udpmode,I2CError,motor1[rps],motor2[rps],LightSensor 1,2,3,4,5,Switch,Accel x,y,z,Gyro x,y,z,Mag x,y,z,System Status,MAIN task,Battery voltage[V],Emergency Stop,\n");
            digitalWrite(STBY, HIGH);
            delay(500);
            digitalWrite(STBY, LOW);
        }
        fclose(f);
        if(mem>LOGNUM)mem=0;
        f = fopen("/sdcard/memo.txt", "w");
        if (f == NULL)
            Serial.println("Failed to open file for writing");
        else
            fprintf(f,"%d",(int)mem);
        fclose(f);
    }
    delay(200);
    noTone(beep);
    bool Erflag=false;
    Timer Ert;
    portTickType lt = xTaskGetTickCount();
    while(1)
    {
        if(SDCH()==0)
        {
            if(Ert.stand_by(2))
                Erflag=true;
            if(!sd.UNMOUNT&&!OTAFLAG)
            {
                stledinterval=500;
                FILE* f = fopen(FILEname,"a");
                if (f == NULL)
                {
                    sdprint="SDerror/";
                    if(!Erflag)
                        ESP32M.Error();
                    stledinterval=50;
                }
                else
                    fprintf(f,"%5ld,%.2f,%.2f,%s,%s,%7.3f,%7.3f,%3d,%3d,%3d,%3d,%3d,%d%d,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%s,%s,%f,%d\n",micros(),CPUTemp(),mpu.read(Temp),udpmode,i2cerror(I2C_Error),md[0]->now_val,md[1]->now_val,l1.read(0),l1.read(1),l1.read(2),l1.read(3),l1.read(4),l1.swread(sw1),l1.swread(sw2),mpu.read(AccX),mpu.read(AccY),mpu.read(AccZ),mpu.read(GyroX),mpu.read(GyroY),mpu.read(GyroZ),mpu.read(MagX),mpu.read(MagY),mpu.read(MagZ),ESP32M.Systemstatus(),ESP32M.Taskstatus(),Vmonitor(),ESP32M.EMARGENCYSTOP());
                fclose(f);
            }
            else
                sdprint="SDunmount/";
        }
        if(melodysel==UnmountBeep)sd.unmount();
        vTaskDelayUntil(&lt,(LOG_interval*100)/portTICK_RATE_MS);
    }
}
#endif

/*OTA*/
int otac=0;
String otastatus;
void OTAsetup()
{
    #if (ESP_OTAE == 1)
    ArduinoOTA
    .onStart([]() {
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH)
            type = "sketch";
        else // U_SPIFFS
            type = "filesystem";
        otastatus="Start updating"+ type;
        // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
        Serial.println(otastatus);
        wheel->Stop();
        digitalWrite(StatusLED, HIGH);
        digitalWrite(STBY, HIGH);
        OTAFLAG=true;
        ESP32M.Start();
    })
    .onEnd([]() {
        digitalWrite(StatusLED, LOW);
        digitalWrite(STBY, LOW);
        Serial.println(otastatus);
        OTAFLAG=false;
    })
    .onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
        if(otac>10)
        {
            digitalWrite(StatusLED, HIGH);
            digitalWrite(STBY, LOW);
        }
        else
        {
            digitalWrite(StatusLED, LOW);
            digitalWrite(STBY, HIGH);
        }
        otac++;
        if(otac>20)otac=0;
    })
    .onError([](ota_error_t error) {
        digitalWrite(StatusLED, HIGH);
        OTAFLAG=false;
        char buff[255];
        sprintf(buff,"Error[%u]: ", error);
        otastatus+=String(buff)+":";
        if (error == OTA_AUTH_ERROR) otastatus+="Auth Failed";
        else if (error == OTA_BEGIN_ERROR) otastatus+="Begin Failed";
        else if (error == OTA_CONNECT_ERROR) otastatus+="Connect Failed";
        else if (error == OTA_RECEIVE_ERROR) otastatus+="Receive Failed";
        else if (error == OTA_END_ERROR) otastatus+="End Failed";
        Serial.println(otastatus);
    });
    ArduinoOTA.begin();
    #endif
}

/*Other*/

ESP32Mather::ESP32Mather()
{
}
int ESP32Mather::setup()
{
    /*UART setup*/
    Serial.begin(115200);
    /*BTSerial Initialize*/
    #if (ESP_MODE == ESP_BTS)
    BTSerial.begin(ESP_BTSNAME);
    #endif
    ESP_SERIAL.printf("\n\rESP:Initializing.");
    /*Digitalpin setup*/
    pinMode(STBY, OUTPUT);
    pinMode(SD_CHECK, INPUT);
    pinMode(SD_MOUNT, INPUT);
    pinMode(stsw, INPUT_PULLUP);
    pinMode(StatusLED, OUTPUT);
    digitalWrite(StatusLED, HIGH);
    digitalWrite(STBY, LOW);
    Vmonitor_setup();
    ESP_SERIAL.printf(".");
    tone(beep,310);
    /*Motor Initialize*/
    motorsetup();
    /*Wifi Initialize*/
    ESP_SERIAL.printf(".");
    #if (ESP_MODE != ESP_PS3)
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP(ESP_SSID, ESP_WIFIPASS);           // SSIDとパスの設定
    delay(100);                        // 追記：このdelayを入れないと失敗する場合がある
    WiFi.softAPConfig(ip, ip, subnet); // IPアドレス、ゲートウェイ、サブネットマスクの設定
    Serial.print("AP IP address: ");
    IPAddress myIP = WiFi.softAPIP();
    Serial.println(myIP);
    Serial.println("Starting UDP");
    udp.begin(LocalPort);  // UDP通信の開始(引数はポート番号)
    Serial.print("Local port: ");
    Serial.println(LocalPort);
    WiFi.setTxPower(WIFI_POWER_MINUS_1dBm);
    /*OTA Initialize*/
    #if (ESP_OTAE == 1)
    OTAsetup();
    otastatus=" ";
    #else
    otastatus="OTA off";
    #endif
    #else
    otastatus="WIFI off";
    #endif
    ESP_SERIAL.printf(".\n\r");
    ESP_SERIAL.println("ESP:System task settings");
    /*Encoder Initialize*/
    #if (MOTORMODE ==PID_M)
    ESP_SERIAL.println("ESP:Encoder task settings");
    ((ESPMotor*)md[0])->Reset();
    ((ESPMotor*)md[1])->Reset();
    xTaskCreatePinnedToCore(enctask,"Encoder task",1024,NULL,2,NULL,0);
    #endif
    /*SDcard Initialize*/
    #if (SD == ON)
    ESP_SERIAL.println("ESP:Log task settings");
    xTaskCreatePinnedToCore(SDlog,"Log task",4048,NULL,1,NULL,1);
    #endif
    /*I2CTask Initialize*/
    #if(I2CPORT == ON)
    ESP_SERIAL.println("ESP:I2C task settings");
    xTaskCreatePinnedToCore(ESPtask_i2c,"I2C task",8096,NULL,3,NULL,0);
    #endif
    /*Odmetry update Task Initialize*/
    #if(I2CPORT == ON && MOTORMODE != DUTY_M)
    ESP_SERIAL.println("ESP:Odmetry task settings");
    xTaskCreatePinnedToCore(Odmetryupdate,"Odmetry task",8096,NULL,0,NULL,1);
    #endif
    /*ROS Initialize*/
    #if (ROS == ON)
    ESP_SERIAL.println("ESP:ROS task settings");
    xTaskCreatePinnedToCore(ESPROStask,"ROStask",8096,NULL,1,NULL,1);
    #endif
    /*melody*/
    ESP_SERIAL.println("ESP:Melody task settings");
    xTaskCreatePinnedToCore(melodytask,"Melody task",1024,NULL,0,NULL,1);
    ESP_SERIAL.println("ESP:Completed all system task settings");
    #if (ESP_MODE == ESP_PS3)
    ESP_SERIAL.println("ESP:PS3 controller settings");
    PS3init();
    #endif
    tasksel=STARTTASK;
    /*end*/
    run.flag.Start=true;
    ESP_SERIAL.printf("ESP:Finish Initialization!\n");
    tone(beep,246);
    delay(100);
    noTone(beep);
    digitalWrite(StatusLED,LOW);
    return 0;
}
void ESP32Mather::setfunction(const char*name,void (*tfp)(Flag_t *))
{
    run.setfunction(name,tfp);
}
void ESP32Mather::update()
{
    /*Variable declaration*/
    static bool mflag=false;
    bool sel=false;
    EMS=Emergency_Stop();
    if((LOWV=LowV())==true)
    {
        if(!mflag)
            melodysel=UnmountBeep;
        mflag=true;
    }
    /*OTA*/
    #if (ESP_OTAE == 1)
    ArduinoOTA.handle();
    #endif
    if(OTAFLAG)
        return;
    /*USBSerial*/
    run.flag.SerialData=0;
    if(Serial.available())run.flag.SerialData = Serial.read();
    /*wifi udp read*/
    #if (ESP_MODE != ESP_PS3)
    if (udp.parsePacket())run.flag.UDPData=udp.read();
    if(run.flag.UDPData==1)
        EMS=true;
    if(run.flag.UDPData==193)sel=true;
    /*
    if(tasksel==wificonNUM)
    {
        if(EMS)
        {
            udp.beginPacket(ip2,LocalPort);
            udp.write((byte)1);
            udp.endPacket();
        }
    }*/
    #endif
    /*debug*/
    if(run.flag.SerialMonitor)
    {
        run.flag.Debug=pdt.stand_by(d_interval);
        if(run.flag.Debug)
        {
            ESP_SERIAL.printf("/%s/%s/%.2f[℃]/%.2f[V],LOWV %d,EM %d/",run.status(),i2cerror(I2C_Error),CPUTemp(),Vmonitor(),LOWV,EMS);
            ESP_SERIAL.print(sdprint);
            sdprint="/";
        }
    }
    else
        run.flag.Debug=false;

    #if (ESP_MODE == ESP_PS3)
    if(PS3PSButton())sel=true;
    #endif
    /*select task*/
    STARTSWITCH<=!STSW()+sel;
    if(run.flag.SerialData=='!')
    {
        tasksel++;
        #if (ESP_MODE != ESP_PS3)
        run.flag.UDPData=48;
        #endif
    }
    switch(STARTSWITCH)
    {
        case 1:
            melodysel=Onbeep;
            run.flag.Start=true;
            tasksel++;
            #if (ESP_MODE != ESP_PS3)
            run.flag.UDPData=48;
            #endif
        break;
        case 2:
            #if (ESP_MODE != ESP_PS3)
            melodysel=Onbeep;
            run.flag.Start=true;
            tasksel=wificonNUM;
            run.flag.UDPData=48;
            #endif
        break;
        case 3:
        break;
        default:
        break;
    }
    /*taskrun*/
    run.select(&tasksel);
    if(EMS)
    {
        digitalWrite(STBY, LOW);
    }
    /*Wifi connection check*/
    #if (ESP_MODE != ESP_PS3)
    if(WiFi.softAPgetStationNum())
    {
        if(wificonnect==false)
        {
            melodysel=ConectBeep;
            wificonnect=true;
            tasksel=wificonNUM;
        }
    }
    else wificonnect=false;
    #endif
    if(run.flag.Melody)
        MOUNT<=!mount();
    switch(MOUNT)
    {
        case 1:
            melodysel++;
            melreset();
        break;
        case 2:

        break;
        case 3:
            #if (SD == ON)
            melodysel=UnmountBeep;
            #endif
        break;
        default:
        break;
    }
    #if (ESP_MODE == ESP_PS3)
    static bool ps3con=false;
    PS3Update();
    if(PS3Conect()&&!ps3con)
    {
        //tasksel=1;
        melodysel=ConectBeep;
        ps3con=true;
    }
    #endif
    delay(Delta_T*100);//Execution interval
}

void ESP32Mather::Error()
{
    melodysel=ErrorBeep;
}
void ESP32Mather::Start()
{
    melodysel=StartBeep;
}
void ESP32Mather::Start_()
{
    melodysel=XPStartBeep;
}

bool ESP32Mather::EMARGENCYSTOP()
{
    return EMS;
}


const char* ESP32Mather::Systemstatus()
{
    switch(melodysel)
    {
        case ErrorBeep:return "Abnormal";
        case ConectBeep:return "Wifi Conect";
        case XPStartBeep:
        case StartBeep:return "Start";
    }
    if(LOWV&&!EMS)
    {
        return "Low battery voltage";
    }
    return "Normal";
}
const char* ESP32Mather::Taskstatus()
{
    return run.status();
}
const char* ESP32Mather::OTAstatus()
{
    return otastatus.c_str();
}

void test(Flag_t *flag)
{
    if(flag->Start)
    {
        flag->Melody = true;
        flag->SerialMonitor = true;
        flag->Start=false;
    }
    static float vx=0,angle=0;
    if(flag->Debug)
    {
        #if (MOTORMODE ==PID_)
        ESP_SERIAL.printf("/md%d/vx/%.2f/%.2f/m1(Kp=%f,Ki=%f,Kd=%f)rpm=%7.3f/m2(Kp=%f,Ki=%f,Kd=%f)rpm=%7.3f/\n\r",mdc,vx,angle,((ESPMotor*)md[0])->Kp,((ESPMotor*)md[0])->Ki,((ESPMotor*)md[0])->Kd,md[0]->now_val,((ESPMotor*)md[1])->Kp,((ESPMotor*)md[1])->Ki,((ESPMotor*)md[1])->Kd,md[1]->now_val);
        #else
        ESP_SERIAL.printf("/MD Vx %.2f,Angular %.2f/\n\r",vx,angle);
        #endif
    }
    if(ESP32M.EMARGENCYSTOP())
        return;
    switch(flag->SerialData)
    {
        case 'w':vx+=0.1;break;
        case 's':vx-=0.1;break;
        case 'a':angle+=0.1;break;
        case 'd':angle-=0.1;break;
        case 'S':vx=angle=0;wheel->Stop();break;
        #if (MOTORMODE ==PID_)
        case 'm':mdc++;break;
        case 'P':mode='P';break;
        case 'I':mode='I';break;
        case 'D':mode='D';break;
        case 'g':((ESPMotor*)md[0])->set();((ESPMotor*)md[1])->set();break;
        case ' ':setgein();break;
        case 'q':mode=0;break;
        #endif
    }
    #if (MOTORMODE ==PID_)
    if(mdc==2)mdc=0;
    switch(mode)
    {
        case 'P':
            if(flag->SerialData=='.')((ESPMotor*)md[mdc])->Kp+=0.01;
            if(flag->SerialData==',')((ESPMotor*)md[mdc])->Kp-=0.01;
            break;
        case 'I':
            if(flag->SerialData=='.')((ESPMotor*)md[mdc])->Ki+=0.0001;
            if(flag->SerialData==',')((ESPMotor*)md[mdc])->Ki-=0.0001;
            break;
        case 'D':
            if(flag->SerialData=='.')((ESPMotor*)md[mdc])->Kd+=0.000001;
            if(flag->SerialData==',')((ESPMotor*)md[mdc])->Kd-=0.000001;
            break;
    }
    #endif
    wheel->Move(vx,0,angle);
}
#if (ESP_MODE != ESP_PS3)
void wificon(Flag_t *flag)
{
    if(flag->Start)
    {
        flag->Melody = true;
        flag->SerialMonitor = true;
        flag->Start=false;
    }
    static float sp=0.3;
    if(flag->Debug)ESP_SERIAL.printf("%d/%s/speed %3.2f/\n\r",flag->UDPData,udpmode,sp);
    if(ESP32M.EMARGENCYSTOP())
        return;
    switch(flag->UDPData)
    {
        case 49:
            if(l1.swread(sw1)&&l1.swread(sw2))wheel->Stop();
            else wheel->Move(sp,0,0);
            if(l1.swread(sw2)){md[0]->Brake();}
            if(l1.swread(sw1)){md[1]->Brake();}
            udpmode="^";
            break;//^
        case 52:wheel->Move(sp*-1,0,0); udpmode="v";break;//v
        case 51:
            if(l1.swread(sw1)&&l1.swread(sw2))wheel->Stop();
            else wheel->Move(0,0,sp*-1);
            udpmode=">";
            break;//>
        case 50:
            if(l1.swread(sw1)&&l1.swread(sw2))wheel->Stop();
            else wheel->Move(0,0,sp);
            udpmode="<";
            break;//<
        case 53:
            if(l1.swread(sw2))wheel->Stop();
            else ((ESPMotor*)md[0])->Move(sp);
            udpmode="a";
            break;//a
        case 54:((ESPMotor*)md[0])->Move(sp*-1);udpmode="b";break;//b
        case 55:
            if(l1.swread(sw1))wheel->Stop();
            else ((ESPMotor*)md[1])->Move(sp);
            udpmode="c";
            break;//c
        case 56:((ESPMotor*)md[1])->Move(sp*-1);udpmode="d";break;//d
        case 97:sel++;       udpmode="set";break;//set
        case 48:wheel->Stop();udpmode=" ";break;//brake
        case 102:wheel->Stop();udpmode="stop";break;//stop
    }
    if(sp>=1.0)sp=0.3;
    if(flag->UDPData!=97)
    {
        if(sel>10){sp+=0.1;}
        sel=0;
    }
}
#endif
#if (ESP_MODE == ESP_PS3)
void ps3task(Flag_t *flag)
{
    if(flag->Start)
    {
        flag->Melody = true;
        flag->SerialMonitor = true;
        flag->Start=false;
    }
    if(flag->Debug)
    {
        ESP_SERIAL.printf("%f/%f/%f/lx=%d/ly=%d/rx=%d/ry=%d\n\r",Vx,Vy,Angular,PS3stick(lX),PS3stick(lY),PS3stick(rX),PS3stick(rY));
        ESP_SERIAL.print(PS3Debug);
    }
    if(ESP32M.EMARGENCYSTOP())
        return;
    bool b=PS3Controller(&Vx,&Vy,&Angular);
    if(b||(l1.swread(sw1)&&l1.swread(sw2)&&Vy>0))wheel->Stop();
    else wheel->Move(Vy,Vx,Angular+Vx);
}
#endif

void ESPinit()
{
    ESP32M.setup();
    /*task setup*/
    #if(DEFAULTTASK == ON)
    #if (MOTORMODE == PID_M)
    ESP32M.setfunction("gainsetup",test);
    #else
    ESP32M.setfunction("Motor Test",test);
    #endif
    #if (ESP_MODE != ESP_PS3)
    ESP32M.setfunction("wificontroller",wificon);
    #else
    ESP32M.setfunction("ps3controller",ps3task);
    #endif
    #endif
}
/*
FreeRTOS memo
core 0 Background task execution
core 1 Main task execution
High priority  Large value
Low priority   Small value
setup and loop task priority 1
*/