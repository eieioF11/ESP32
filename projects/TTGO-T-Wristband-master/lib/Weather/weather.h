#ifndef WEATHER_H_
#define WEATHER_H_
#include <Arduino.h>
#include <stdio.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "SetUp.h"
#if defined __cplusplus
extern "C" {
#endif

//天気関連
String now_weather,now_temp,tom_weather,tom_temp;
const String endpoint = "http://api.openweathermap.org/data/2.5/weather?q=Tokyo,jp&APPID=";
const String endpoint_forecast = "http://api.openweathermap.org/data/2.5/forecast?q=Tokyo,jp&APPID=";
const String key = "31abb09e5d4b51e798fa7dc2ea43a6aa";//取得したAPIキー

uint8_t now_hour=0;
bool httperror=false;

//12時間後の天気を取得する関数
void get_tomorrow_weather(){
  HTTPClient http;
 
    http.begin(endpoint_forecast + key); //URLを指定
    int httpCode = http.GET();  //GETリクエストを送信
 
    if (httpCode > 0) { //返答がある場合

        httperror=false;
        String payload = http.getString();  //返答（JSON形式）を取得

        //jsonオブジェクトの作成
        DynamicJsonBuffer jsonBuffer;
        String json = payload;
        JsonObject& weatherdata = jsonBuffer.parseObject(json);

        //パースが成功したかどうかを確認
        if(!weatherdata.success()){
           httperror=true;
          Serial.println("parseObject() failed");
        }
        
        //抜き出すデータの番号
        int data_number = now_hour/3 + 4;
        //12時間後のデータを抜き出し
        const char* weather = weatherdata["list"][data_number]["weather"][0]["main"].as<char*>();
        const double temp = weatherdata["list"][data_number]["main"]["temp"].as<double>();
        //表示用変数に各要素をセット
        tom_weather = weather;
        tom_temp = String(temp-273.15);
      }
 
    else {
      httperror=true;
      Serial.println("Error on HTTP request");
    }
 
    http.end();
}

//今日の天気を取得する関数
void get_today_weather(){
  HTTPClient http;
 
    http.begin(endpoint + key); //URLを指定
    int httpCode = http.GET();  //GETリクエストを送信
 
    if (httpCode > 0) { //返答がある場合

        httperror=false;
        String payload = http.getString();  //返答（JSON形式）を取得
        Serial.println(httpCode);
        Serial.println(payload);

        //jsonオブジェクトの作成
        DynamicJsonBuffer jsonBuffer;
        String json = payload;
        JsonObject& weatherdata = jsonBuffer.parseObject(json);

        //パースが成功したかどうかを確認
        if(!weatherdata.success()){
          httperror=true;
          Serial.println("parseObject() failed");
        }

        //各データを抜き出し
        const char* weather = weatherdata["weather"][0]["main"].as<char*>();
        const double temp = weatherdata["main"]["temp"].as<double>();

        //表示用変数に各要素をセット
        now_weather = weather;
        now_temp = String(temp-273.15);
      }
 
    else {
      httperror=true;
      Serial.println("Error on HTTP request");
    }
 
    http.end(); //Free the resources
}

//ticker用の天気データまとめ関数
void get_weather_data(){
  get_today_weather();
  get_tomorrow_weather();
}

void weathertask(void *arg)
{
  //get_weather_data();
  portTickType lt = xTaskGetTickCount();
  while(1)
  {
    if(!otaStart)
      get_weather_data();
    vTaskDelayUntil(&lt,(60*1000)/portTICK_RATE_MS);//60*1000
  }
}

#define brocknum 5

const int mark[brocknum][4][7][7]=
{
  {  //sunny
    {
      {1, 0, 0, 1, 0, 0, 1},
      {0, 0, 0, 0, 0, 0, 0},
      {0, 0, 1, 1, 1, 0, 0},
      {1, 0, 1, 1, 1, 0, 1},
      {0, 0, 1, 1, 1, 0, 0},
      {0, 0, 0, 0, 0, 0, 0},
      {1, 0, 0, 1, 0, 0, 1},
    },
    {
      {0, 0, 0, 0, 0, 0, 0},
      {0, 1, 0, 1, 0, 1, 0},
      {0, 0, 1, 1, 1, 0, 0},
      {0, 1, 1, 1, 1, 1, 0},
      {0, 0, 1, 1, 1, 0, 0},
      {0, 1, 0, 1, 0, 1, 0},
      {0, 0, 0, 0, 0, 0, 0},
    },
    {
      {1, 0, 0, 1, 0, 0, 1},
      {0, 0, 0, 0, 0, 0, 0},
      {0, 0, 1, 1, 1, 0, 0},
      {1, 0, 1, 1, 1, 0, 1},
      {0, 0, 1, 1, 1, 0, 0},
      {0, 0, 0, 0, 0, 0, 0},
      {1, 0, 0, 1, 0, 0, 1},
    },
    {
      {0, 0, 0, 0, 0, 0, 0},
      {0, 1, 0, 1, 0, 1, 0},
      {0, 0, 1, 1, 1, 0, 0},
      {0, 1, 1, 1, 1, 1, 0},
      {0, 0, 1, 1, 1, 0, 0},
      {0, 1, 0, 1, 0, 1, 0},
      {0, 0, 0, 0, 0, 0, 0},
    },
  },
  {  //clowdy
    {
      {0, 0, 0, 0, 0, 0, 0},
      {0, 0, 1, 0, 1, 0, 0},
      {0, 1, 1, 1, 1, 1, 0},
      {1, 1, 1, 1, 1, 1, 1},
      {1, 1, 1, 1, 1, 1, 1},
      {0, 1, 1, 1, 1, 1, 0},
      {0, 0, 1, 0, 1, 0, 0},
    },
    {
      {0, 0, 0, 0, 0, 0, 0},
      {0, 1, 0, 1, 1, 0, 0},
      {0, 1, 1, 1, 1, 1, 0},
      {1, 1, 1, 1, 1, 1, 1},
      {1, 1, 1, 1, 1, 1, 1},
      {0, 1, 1, 1, 1, 1, 0},
      {0, 0, 1, 1, 0, 1, 0},
    },
    {
      {0, 0, 0, 0, 0, 0, 0},
      {0, 0, 1, 1, 0, 0, 0},
      {0, 1, 1, 1, 1, 1, 0},
      {1, 1, 1, 1, 1, 1, 1},
      {1, 1, 1, 1, 1, 1, 1},
      {0, 1, 1, 1, 1, 1, 0},
      {0, 0, 0, 0, 1, 1, 0},
    },
    {
      {0, 0, 0, 0, 0, 0, 0},
      {0, 0, 1, 1, 0, 1, 0},
      {0, 1, 1, 1, 1, 1, 0},
      {1, 1, 1, 1, 1, 1, 1},
      {1, 1, 1, 1, 1, 1, 1},
      {0, 1, 1, 1, 1, 1, 0},
      {0, 1, 0, 0, 1, 1, 0},
    },
  },
  {  // rainy
    {
      {0, 1, 0, 0, 0, 0, 1},
      {0, 1, 0, 0, 0, 0, 1},
      {1, 0, 0, 1, 0, 0, 0},
      {1, 0, 0, 1, 0, 0, 0},
      {0, 1, 0, 0, 0, 1, 0},
      {0, 1, 0, 0, 0, 1, 0},
      {0, 0, 0, 1, 0, 0, 0},
    },
    {
      {1, 0, 0, 1, 0, 0, 0},
      {0, 1, 0, 0, 0, 0, 1},
      {0, 1, 0, 0, 0, 0, 1},
      {1, 0, 0, 1, 0, 0, 0},
      {1, 0, 0, 1, 0, 0, 0},
      {0, 1, 0, 0, 0, 1, 0},
      {0, 1, 0, 0, 0, 1, 0},
    },
    {
      {1, 0, 0, 1, 0, 0, 0},
      {1, 0, 0, 1, 0, 0, 0},
      {0, 1, 0, 0, 0, 0, 1},
      {0, 1, 0, 0, 0, 0, 1},
      {1, 0, 0, 1, 0, 0, 0},
      {1, 0, 0, 1, 0, 0, 0},
      {0, 1, 0, 0, 0, 1, 0},
    },
    {
      {0, 1, 0, 0, 0, 1, 0},
      {1, 0, 0, 1, 0, 0, 0},
      {1, 0, 0, 1, 0, 0, 0},
      {0, 1, 0, 0, 0, 0, 1},
      {0, 1, 0, 0, 0, 0, 1},
      {1, 0, 0, 1, 0, 0, 0},
      {1, 0, 0, 1, 0, 0, 0},
    },
  },
  {  // mist
    {
      {0, 1, 0, 1, 0, 1, 0},
      {1, 0, 1, 0, 1, 0, 1},
      {0, 1, 0, 1, 0, 1, 0},
      {1, 0, 1, 0, 1, 0, 1},
      {0, 1, 0, 1, 0, 1, 0},
      {1, 0, 1, 0, 1, 0, 1},
      {0, 1, 0, 1, 0, 1, 0},
    },
    {
      {1, 0, 1, 0, 1, 0, 1},
      {0, 1, 0, 1, 0, 1, 0},
      {1, 0, 1, 0, 1, 0, 1},
      {0, 1, 0, 1, 0, 1, 0},
      {1, 0, 1, 0, 1, 0, 1},
      {0, 1, 0, 1, 0, 1, 0},
      {1, 0, 1, 0, 1, 0, 1},
    },
    {
      {0, 1, 0, 1, 0, 1, 0},
      {1, 0, 1, 0, 1, 0, 1},
      {0, 1, 0, 1, 0, 1, 0},
      {1, 0, 1, 0, 1, 0, 1},
      {0, 1, 0, 1, 0, 1, 0},
      {1, 0, 1, 0, 1, 0, 1},
      {0, 1, 0, 1, 0, 1, 0},
    },
    {
      {1, 0, 1, 0, 1, 0, 1},
      {0, 1, 0, 1, 0, 1, 0},
      {1, 0, 1, 0, 1, 0, 1},
      {0, 1, 0, 1, 0, 1, 0},
      {1, 0, 1, 0, 1, 0, 1},
      {0, 1, 0, 1, 0, 1, 0},
      {1, 0, 1, 0, 1, 0, 1},
    },
  },
  {  // Snow
    {
      {0, 1, 0, 0, 0, 0, 1},
      {0, 0, 0, 0, 0, 0, 0},
      {1, 0, 0, 1, 0, 1, 0},
      {0, 0, 0, 0, 1, 0, 0},
      {0, 1, 0, 0, 0, 1, 1},
      {1, 0, 0, 0, 0, 0, 0},
      {1, 1, 1, 1, 1, 1, 1},
    },
    {
      {0, 0, 0, 0, 1, 0, 0},
      {0, 1, 0, 0, 0, 0, 1},
      {0, 0, 0, 0, 0, 0, 0},
      {1, 0, 0, 1, 0, 1, 0},
      {0, 0, 0, 0, 1, 0, 0},
      {0, 1, 0, 0, 0, 1, 1},
      {1, 1, 1, 1, 1, 1, 1},
    },
    {
      {1, 0, 0, 1, 0, 1, 0},
      {0, 0, 0, 0, 1, 0, 0},
      {0, 1, 0, 0, 0, 0, 1},
      {0, 0, 0, 0, 0, 0, 0},
      {1, 0, 0, 1, 0, 0, 0},
      {0, 0, 0, 0, 1, 1, 0},
      {1, 1, 1, 1, 1, 1, 1},
    },
    {
      {0, 0, 0, 0, 0, 0, 0},
      {1, 0, 0, 1, 0, 1, 0},
      {0, 0, 0, 0, 1, 0, 0},
      {0, 1, 0, 0, 0, 0, 1},
      {0, 0, 0, 0, 0, 0, 0},
      {1, 0, 0, 1, 0, 1, 0},
      {1, 1, 1, 1, 1, 1, 1},
    },
  },
};

void drawmark(TFT_eSPI *tft,int num,int t,int color,int x,int y,int backcolor=TFT_BLACK)
{
  for(int i=0;i<7;i+=1)
  {
    for(int j=0;j<7;j+=1)
    {
      if(mark[num][t][j][i])
        tft->drawPixel(x+i,y+j,color);
      else
        tft->drawPixel(x+i,y+j,backcolor);
    }
  }
}

#if defined __cplusplus
}
#endif
#endif
