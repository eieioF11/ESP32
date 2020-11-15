#include "MPU9250.h"
#define samplesize 100

#define MXmin -37.061
#define MXmax 27.246
#define MYmin -23.145
#define MYmax 44.531
#define MZmin -19.043
#define MZmax 55.371

MPU9250::MPU9250(){};
void MPU9250::setup()
{
    // reset device
    // Write a one to bit 7 reset bit; toggle reset device
    i2cWriteByte(MPU9250_ADDRESS, PWR_MGMT_1, 0x00);//スリープモードを解除
    //LPF設定
    i2cWriteByte(MPU9250_ADDRESS,MPU6050_DLPF_CFG,0x01);// Set low-pass filter to 188 Hz
    i2cWriteByte(MPU9250_ADDRESS, SMPLRT_DIV, 0x00);  // Set sample rate to 1 kHz

    i2cWriteByte(MPU9250_ADDRESS, ACCEL_CONFIG, ACCEL_FS_SEL_16G);//加速度センサの測定レンジの設定
    accRange = 16.0;//計算で使用するので，選択したレンジを入力する
    i2cWriteByte(MPU9250_ADDRESS, GYRO_CONFIG, GYRO_FS_SEL_2000DPS);//ジャイロセンサの測定レンジの設定
    gyroRange = 2000.0;//計算で使用するので，選択したレンジを入力する
    reset();
    i2cWriteByte(MPU9250_ADDRESS, INT_PIN_CFG, 0x02);//bypass mode(磁気センサが使用出来るようになる)

    i2cWriteByte(AK8963_ADDRESS, CNTL1, CNTL1_MODE_SEL_100HZ);//磁気センサのAD変換開始
    Offset();
}

void MPU9250::Offset()
{
    gXoff=0.f;
    gYoff=0.f;
    gZoff=0.f;
    for(int i = 0; i < samplesize; i++)
    {
        update();
        gXoff += gyroX;
        gYoff += gyroY;
        gZoff += gyroZ;
        vTaskDelay(1 / portTICK_RATE_MS);
    }
    gXoff /= samplesize;
    gYoff /= samplesize;
    gZoff /= samplesize;

    mXmin=MXmin;
    mXmax=MXmax;
    mYmin=MYmin;
    mYmax=MYmax;
    mZmin=MZmin;
    mZmax=MZmax;
    mXoff=(mXmax-mXmin)/2.f + mXmin;
    mYoff=(mYmax-mYmin)/2.f + mYmin;
    mZoff=(mZmax-mZmin)/2.f + mZmin;
}

void MPU9250::update()
{
    //ACC&GRYO///////////////////////////////////////////////////////////////////////////
    i2cRead(MPU9250_ADDRESS, 0x3b, 14, accGyroTempData); //0x3bから，14バイト分をaccGyroDataにいれる

    //COMPASS////////////////////////////////////////////////////////////////////////////
    i2cRead(AK8963_ADDRESS, ST1, 1, &ST1Bit);//読み出し準備ができたか確認
    if ((ST1Bit & 0x01))
    {
        i2cRead(AK8963_ADDRESS, 0x03, 7, magneticData);//7番目の0x09(ST2)まで読まないとデータが更新されない
    }

    //Acc
    ax = (accGyroTempData[0] << 8) | accGyroTempData[1];//accGyroTempData[0]を左に8シフトし(<<)，accGyroTempData[1]を足し合わせる(|)
    ay = (accGyroTempData[2] << 8) | accGyroTempData[3];//accGyroTempData[2]を左に8シフトし(<<)，accGyroTempData[3]を足し合わせる(|)
    az = (accGyroTempData[4] << 8) | accGyroTempData[5];//accGyroTempData[4]を左に8シフトし(<<)，accGyroTempData[5]を足し合わせる(|)
    //Gyro
    gx = (accGyroTempData[8] << 8) | accGyroTempData[9];//accGyroTempData[8]を左に8シフトし(<<)，accGyroTempData[9]を足し合わせる(|)
    gy = (accGyroTempData[10] << 8) | accGyroTempData[11];//accGyroTempData[10]を左に8シフトし(<<)，accGyroTempData[11]を足し合わせる(|)
    gz = (accGyroTempData[12] << 8) | accGyroTempData[13];//accGyroTempData[12]を左に8シフトし(<<)，accGyroTempData[13]を足し合わせる(|)
    //Temp
    tempMPU9250Raw = (accGyroTempData[6] << 8) | accGyroTempData[7];//accGyroTempData[6]を左に8シフトし(<<)，accGyroTempData[7]を足し合わせる(|)
    //Magneto
    mx = (magneticData[3] << 8) | magneticData[2];//センサの軸が違うので順番が加速度とジャイロと違う
    my = (magneticData[1] << 8) | magneticData[0];//magneticData[1]を左に8シフトし(<<)，magneticData[0]を足し合わせる(|)
    mz = -((magneticData[5] << 8) | magneticData[4]);//加速度，ジャイロセンサと軸の向きが逆なので-を掛ける

    accX = ax * accRange / 32768.0;//[G]に変換
    accY = ay * accRange / 32768.0;//[G]に変換
    accZ = az * accRange / 32768.0;//[G]に変換

    gyroX = gx * gyroRange / 32768.0;//[deg/s]に変換
    gyroY = gy * gyroRange / 32768.0;//[deg/s]に変換
    gyroZ = gz * gyroRange / 32768.0;//[deg/s]に変換

    tempMPU9250 = ((tempMPU9250Raw - 0.0) / 333.87) + 21.0f;
    //MPU-9250 Product Specification Revision 1.0のP12の値と,
    //MPU-9250Register Map and Descriptions Revision 1.4のP33の式を使用

    magX = (mx / 32768.0f * 4800.0f);//[uT]に変換
    magY = (my / 32768.0f * 4800.0f);//[uT]に変換
    magZ = (mz / 32768.0f * 4800.0f);//[uT]に変換

    //磁気センサーオフセット計算
    if(magX <= mXmin)mXmin=magX;
    if(magX >= mXmax)mXmax=magX;
    if(magY <= mYmin)mYmin=magY;
    if(magY >= mYmax)mYmax=magY;
    if(magZ <= mZmin)mZmin=magZ;
    if(magZ >= mZmax)mZmax=magZ;

    mXoff=(mXmax-mXmin)/2.f + mXmin;
    mYoff=(mYmax-mYmin)/2.f + mYmin;
    mZoff=(mZmax-mZmin)/2.f + mZmin;
}

void MPU9250::i2cRead(uint8_t Address, uint8_t Register, uint8_t NBytes, volatile uint8_t* Data)
{//指定したアドレスのデータを読む関数
    Wire.beginTransmission(Address);//指定したアドレスと通信を始める
    Wire.write(Register);//レジスタを書き込む
    Wire.endTransmission();//通信を終了する

    Wire.requestFrom(Address, NBytes);//スレーブからNByteのデータを要求する
    uint8_t index = 0;
    while (Wire.available())
    {
        Data[index++] = Wire.read();//データを読み込む
    }
}
void MPU9250::i2cWriteByte(uint8_t Address, uint8_t Register, volatile uint8_t Data)
{//指定したアドレスにデータを書き込む関数
    Wire.beginTransmission(Address);//指定したアドレスと通信を始める
    Wire.write(Register);//指定するレジスタを書き込む
    Wire.write(Data);//データを書き込む
    Wire.endTransmission();//通信を終了する
}

float MPU9250::read(rmode mode)
{
    switch(mode)
    {
        case AccX:return accX;
        case AccY:return accY;
        case AccZ:return accZ;
        case GyroX:return gyroX-gXoff;
        case GyroY:return gyroY-gYoff;
        case GyroZ:return gyroZ-gZoff;
        case MagX:return magX-mXoff;
        case MagY:return magY-mYoff;
        case MagZ:return magZ-mZoff;
        case Temp:return tempMPU9250;
    }
    return -1;
}
void MPU9250::print()
{
    ESP_SERIAL.print("ax:");
    ESP_SERIAL.print(accX);
    ESP_SERIAL.print("/ay:");
    ESP_SERIAL.print(accY);
    ESP_SERIAL.print("/az:");
    ESP_SERIAL.print(accZ);
    ESP_SERIAL.print("/gx:");
    ESP_SERIAL.print(gyroX-gXoff);
    ESP_SERIAL.print("/gy:");
    ESP_SERIAL.print(gyroY-gYoff);
    ESP_SERIAL.print("/gz:");
    ESP_SERIAL.print(gyroZ-gZoff);
    ESP_SERIAL.print("/mx:");
    ESP_SERIAL.print(magX-mXoff);
    ESP_SERIAL.print("/my:");
    ESP_SERIAL.print(magY-mYoff);
    ESP_SERIAL.print("/mz:");
    ESP_SERIAL.print(magZ-mZoff);
    //ESP_SERIAL.printf("/M max min/x=(%.3f,%.3f),y=(%.3f,%.3f),z=(%.3f,%.3f)",mXmax,mXmin,mYmax,mYmin,mZmax,mZmin);
    //ESP_SERIAL.printf("/moff/x=%f,y=%f,z=%f",mXoff,mYoff,mZoff);
    ESP_SERIAL.print("/temp:");
    ESP_SERIAL.print(tempMPU9250);
    ESP_SERIAL.println("/");
}
void MPU9250::reset()
{
    ax = 0;//16bitの出力データ
    ay = 0;//16bitの出力データ
    az = 0;//16bitの出力データ


    gx = 0;//16bitの出力データ
    gy = 0;//16bitの出力データ
    gz = 0;//16bitの出力データ

    tempMPU9250Raw = 0;//16bitの出力データ
    mx = 0;//16bitの出力データ
    my = 0;//16bitの出力データ
    mz = 0;//16bitの出力データ

    accX = 0;//加速度センサから求めた重力加速度
    accY = 0;//加速度センサから求めた重力加速度
    accZ = 0;//加速度センサから求めた重力加速度

    gyroX = 0;//ジャイロセンサから求めた角速度
    gyroY = 0;//ジャイロセンサから求めた角速度
    gyroZ = 0;//ジャイロセンサから求めた角速度

    tempMPU9250 = 0;//MPU9250の温度

    magX = 0;//磁気センサから求めたuT
    magY = 0;//磁気センサから求めたuT
    magZ = 0;//磁気センサから求めたuT

    gXoff=0.f;
    gYoff=0.f;
    gZoff=0.f;

    mXmin=MXmin;
    mXmax=MXmax;
    mYmin=MYmin;
    mYmax=MYmax;
    mZmin=MZmin;
    mZmax=MZmax;

    mXoff=(mXmax-mXmin)/2.f + mXmin;//磁気センサから求めた角速度offset
    mYoff=(mYmax-mYmin)/2.f + mYmin;//磁気センサから求めた角速度offset
    mZoff=(mZmax-mZmin)/2.f + mZmin;//磁気センサから求めた角速度offset
}