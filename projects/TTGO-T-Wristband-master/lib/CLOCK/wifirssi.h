#ifndef WIFIRSSI_H_
#define WIFIRSSI_H_
#include <Arduino.h>
#include <stdio.h>
#include <WiFi.h>
#include "SetUp.h"
#if defined __cplusplus
extern "C" {
#endif

void drawwifi(TFT_eSPI *tft,int color,int color2,int fx,int fy,int backcolor=TFT_BLACK)
{
    int rssimark[11][13]=
    {
        {1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    };
    const int rssimark2[4][10]=
    {
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {0, 0, 1, 1, 1, 1, 1, 1, 1, 1},
        {0, 0, 0, 0, 1, 1, 1, 1, 1, 1},
        {0, 0, 0, 0, 0, 0, 1, 1, 1, 1},
    };
    const int great=-30;
    const int good =-67;
    const int ok   =-70;
    const int bad  =-80;
    long rssi = WiFi.RSSI();
    uint8_t num=0;
    if(rssi)
    {
        if(rssi>=great-5)     num=4;
        else if(rssi>=good-5) num=3;
        else if(rssi>=ok-5)   num=2;
        else if(rssi>bad-5)   num=1;
        else num=0;
    }
    else 
        num=5;
    switch(num)
    {
        case 4:
        for(int y=0;y<10;y++)
        {
            if(rssimark2[0][y])
                rssimark[y+1][6]=2;
        }
        case 3:
        for(int y=0;y<10;y++)
        {
            if(rssimark2[1][y])
                rssimark[y+1][8]=2;
        }
        case 2:
        for(int y=0;y<10;y++)
        {
            if(rssimark2[2][y])
                rssimark[y+1][10]=2;
        }
        case 1:
        for(int y=0;y<10;y++)
        {
            if(rssimark2[3][y])
                rssimark[y+1][12]=2;
        }
        break;
        default:
            rssimark[0][6]=rssimark[0][10]=rssimark[1][7]=rssimark[1][9]=rssimark[2][8]=rssimark[3][7]=rssimark[3][9]=rssimark[4][6]=rssimark[4][10]=3;
        break;
    }

    for(int y=0;y<11;y++)
    {
        for(int x=0;x<13;x++)
        {
            if(rssimark[y][x]==1)
                tft->drawPixel(fx+x,fy+y,color);
            else if(rssimark[y][x]==2)
                tft->drawPixel(fx+x,fy+y,color2);
            else if(rssimark[y][x]==3)
                tft->drawPixel(fx+x,fy+y,TFT_RED);
            else 
                tft->drawPixel(fx+x,fy+y,backcolor);
        }
    }

}

void wifi_scan(TFT_eSPI *tft,char *buff)
{
    tft->setTextColor(TFT_GREEN, TFT_BLACK);
    tft->fillScreen(TFT_BLACK);
    tft->setTextDatum(MC_DATUM);
    tft->setTextSize(1);

    tft->drawString("Scan Network", tft->width() / 2, tft->height() / 2);

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);

    int16_t n = WiFi.scanNetworks();
    tft->fillScreen(TFT_BLACK);
    if (n == 0) {
        tft->drawString("no networks found", tft->width() / 2, tft->height() / 2);
    } else {
        tft->setTextDatum(TL_DATUM);
        tft->setCursor(0, 0);
        for (int i = 0; i < n; ++i) {
            sprintf(buff,
                    "[%d]:%s(%d)",
                    i + 1,
                    WiFi.SSID(i).c_str(),
                    WiFi.RSSI(i));
            Serial.println(buff);
            tft->println(buff);
        }
    }
    WiFi.mode(WIFI_OFF);
}

#if defined __cplusplus
}
#endif
#endif