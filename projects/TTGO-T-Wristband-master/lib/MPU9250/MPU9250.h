#ifndef MPU9250_H_
#define MPU9250_H_
#include <Arduino.h>
#include <Wire.h>
#include "stprint.h"

#define ESP_SERIAL Serial

#if defined __cplusplus
extern "C" {
#endif

/////////////////////////MPU9250////////////////////////////
#define MPU9250_ADDRESS 0x68//I2CでのMPU9250のスレーブアドレス

#define PWR_MGMT_1 0x6b//電源管理のアドレス，スリープモード解除用
#define PWR_MGMT_2 0x6C
#define INT_PIN_CFG 0x37//磁気センサのバイパスモード設定用のアドレス
#define INT_ENABLE 0x38
#define I2C_MST_CTRL 0x24
#define CONFIG       0x1A
#define USER_CTRL    0x6A  // Bit 7 enable DMP, bit 3 reset DMP
#define PWR1_SLEEP_BIT  6

#define ACCEL_CONFIG 0x1c//加速度センサ設定用のアドレス
#define ACCEL_FS_SEL_2G 0x00//加速度センサのレンジ(2G)
#define ACCEL_FS_SEL_4G 0x08//加速度センサのレンジ(4G)
#define ACCEL_FS_SEL_8G 0x10//加速度センサのレンジ(8G)
#define ACCEL_FS_SEL_16G 0x18//加速度センサのレンジ(16G)

#define GYRO_CONFIG 0x1b//ジャイロセンサ設定用のアドレス
#define GYRO_FS_SEL_250DPS 0x00//ジャイロセンサのレンジ(250DPS)
#define GYRO_FS_SEL_500DPS 0x08//ジャイロセンサのレンジ(500DPS)
#define GYRO_FS_SEL_1000DPS 0x10//ジャイロセンサのレンジ(1000DPS)
#define GYRO_FS_SEL_2000DPS 0x18//ジャイロセンサのレンジ(2000DPS)

#define AK8963_ADDRESS 0x0c//磁気センサのスレーブアドレス
#define CNTL1 0x0a//磁気センサ設定用のアドレス
#define CNTL1_MODE_SEL_8HZ 0x12//磁気センサの出力周期(8Hz)
#define CNTL1_MODE_SEL_100HZ 0x16//磁気センサの出力周期(100Hz)
#define ST1 0x02//データ読み込み用フラッグのアドレス
#define AK8963_ASAX      0x10  // Fuse ROM x-axis sensitivity adjustment value

#define MPU6050_DLPF_CFG 0x1A
#define SMPLRT_DIV        0x19

#define FIFO_EN            0x23
#define FIFO_COUNTH        0x72
#define FIFO_COUNTL        0x73
#define FIFO_R_W           0x74

#define XA_OFFSET_H        0x77
#define XA_OFFSET_L        0x78
#define YA_OFFSET_H        0x7A
#define YA_OFFSET_L        0x7B
#define ZA_OFFSET_H        0x7D
#define ZA_OFFSET_L        0x7E

#define XG_OFFSET_H       0x13  // User-defined trim values for gyroscope
#define XG_OFFSET_L       0x14
#define YG_OFFSET_H       0x15
#define YG_OFFSET_L       0x16
#define ZG_OFFSET_H       0x17
#define ZG_OFFSET_L       0x18

///////////////////////////////////////////////////////////

enum rmode
{
    AccX,
    AccY,
    AccZ,
    GyroX,
    GyroY,
    GyroZ,
    MagX,
    MagY,
    MagZ,
    Temp        
};
class MPU9250
{
  private:

    volatile float accRange;//計算で使用するので，選択したレンジを入力する定数
    volatile float gyroRange;//計算で使用するので，選択したレンジを入力する定数

    volatile uint8_t accGyroTempData[14];//センサからのデータ格納用配列
    volatile uint8_t magneticData[7];//センサからのデータ格納用配列
    volatile uint8_t ST1Bit;//磁気センサのフラッグ

    volatile int16_t ax = 0;//16bitの出力データ
    volatile int16_t ay = 0;//16bitの出力データ
    volatile int16_t az = 0;//16bitの出力データ
    volatile int16_t gx = 0;//16bitの出力データ
    volatile int16_t gy = 0;//16bitの出力データ
    volatile int16_t gz = 0;//16bitの出力データ
    volatile int16_t tempMPU9250Raw = 0;//16bitの出力データ
    volatile int16_t mx = 0;//16bitの出力データ
    volatile int16_t my = 0;//16bitの出力データ
    volatile int16_t mz = 0;//16bitの出力データ

    volatile float accX = 0;//加速度センサから求めた重力加速度
    volatile float accY = 0;//加速度センサから求めた重力加速度
    volatile float accZ = 0;//加速度センサから求めた重力加速度

    volatile float gyroX = 0;//ジャイロセンサから求めた角速度
    volatile float gyroY = 0;//ジャイロセンサから求めた角速度
    volatile float gyroZ = 0;//ジャイロセンサから求めた角速度

    volatile float gXoff = 0;//ジャイロセンサから求めた角速度offset
    volatile float gYoff = 0;//ジャイロセンサから求めた角速度offset
    volatile float gZoff = 0;//ジャイロセンサから求めた角速度offset

    volatile float mXmin = 0;
    volatile float mYmin = 0;
    volatile float mZmin = 0;

    volatile float mXmax = 0;
    volatile float mYmax = 0;
    volatile float mZmax = 0;

    volatile float mXoff = 0;//磁気センサから求めた角速度offset
    volatile float mYoff = 0;//磁気センサから求めた角速度offset
    volatile float mZoff = 0;//磁気センサから求めた角速度offset

    volatile float tempMPU9250 = 0;//MPU9250の温度

    volatile float magX = 0;//磁気センサから求めたuT
    volatile float magY = 0;//磁気センサから求めたuT
    volatile float magZ = 0;//磁気センサから求めたuT

    enum Mscale {
      MFS_14BITS = 0, // 0.6 mG per LSB
      MFS_16BITS      // 0.15 mG per LSB
    };

    // Choose either 14-bit or 16-bit magnetometer resolution
    uint8_t Mscale = MFS_16BITS;
    // 2 for 8 Hz, 6 for 100 Hz continuous magnetometer data read
    uint8_t Mmode = 0x02;

    void i2cRead(uint8_t Address, uint8_t Register, uint8_t NBytes, volatile uint8_t* Data);
    void i2cWriteByte(uint8_t Address, uint8_t Register, volatile uint8_t Data);
    void writeBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t data);
    uint8_t readByte(uint8_t address, uint8_t subAddress);
    void Offset();
  public:
    MPU9250();
    void setup();
    void update();
    void print();
    void setSleepEnabled(bool);
    float read(rmode mode);
    void reset();
};

#if defined __cplusplus
}
#endif
#endif