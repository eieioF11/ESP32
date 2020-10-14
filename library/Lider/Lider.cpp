#include "Lider.h"

Lider::Lider()
{
    dis=0;
    deg=0.f;
    predeg=0.f;
    nowtime=0ul;
    oldtime=0ul;
    starttime=0ul;
    stoptime=0ul;
}

uint16_t Lider::readangledata()
{
    // Read RAW_ANGLE value from encoder
    Wire.beginTransmission(AS5600_AS5601_DEV_ADDRESS);
    Wire.write(AS5600_AS5601_REG_RAW_ANGLE);
    Wire.endTransmission(false);
    Wire.requestFrom(AS5600_AS5601_DEV_ADDRESS, 2);
    uint16_t RawAngle = 0;
    RawAngle  = ((uint16_t)Wire.read() << 8) & 0x0F00;
    RawAngle |= (uint16_t)Wire.read();
    // Raw angle value (0x0000~0x0FFF) is stored in RawAngle
    return (RawAngle>AS5600_ini)?RawAngle-AS5600_ini:RawAngle+AS5600_max-AS5600_ini;
}

float Lider::readdeg()
{
    return (float)readangledata()*(359.0/AS5600_max);
}

void Lider::setup()
{
    gVL530X.init();//距離センサ(VL53L0X)の初期化
    gVL530X.setTimeout(10000);//タイムアウトまでの時間をセットする。1000⇒1000ms
    gVL530X.startContinuous(0);//連続測定を開始する、100⇒100ms間隔でサンプリングする、ここに0を指定した場合は可能な限り頻繁にサンプリングする
}

void Lider::update()
{
    //Posture calculation
    nowtime=micros();
    //Update time calculation
    float dt=(nowtime-oldtime)*1.0E-6;
    nowdeg = readdeg();
    starttime=micros();
    float w=(nowdeg-predeg)/dt;
    dis = gVL530X.readRangeContinuousMillimeters();
    stoptime=micros();
    float t =(stoptime-starttime)*1.0E-6;
    deg=nowdeg+w*t;
    oldtime=nowtime;
    predeg=nowdeg;
}

float Lider::read(LiderReadmode mode)
{
    switch(mode)
    {
        case Angle:return deg;
        case Distance:return (float)dis;
    }
    return -1;
}