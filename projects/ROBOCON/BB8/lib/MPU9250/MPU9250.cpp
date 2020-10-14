#include "MPU9250.h"

MPU9250 mpu;
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
    setoffset();
    i2cWriteByte(MPU9250_ADDRESS, INT_PIN_CFG, 0x02);//bypass mode(磁気センサが使用出来るようになる)
    
    i2cWriteByte(AK8963_ADDRESS, CNTL1, CNTL1_MODE_SEL_100HZ);//磁気センサのAD変換開始
}

void MPU9250::setoffset()
{
    uint8_t data[12]; // data array to hold accelerometer and gyro x, y, z, data
    uint16_t ii, packet_count,fifo_count;
    int32_t gyro_bias[3]  = {0, 0, 0}, accel_bias[3] = {0, 0, 0};
    uint16_t  gyrosensitivity  = 131;   // = 131 LSB/degrees/sec
    uint16_t  accelsensitivity = 16384;  // = 16384 LSB/g

    // reset device
    // Write a one to bit 7 reset bit; toggle reset device
    i2cWriteByte(MPU9250_ADDRESS, PWR_MGMT_1, 0x80);
    delay(100);
   
    // get stable time source; Auto select clock source to be PLL gyroscope
    // reference if ready else use the internal oscillator, bits 2:0 = 001
    i2cWriteByte(MPU9250_ADDRESS, PWR_MGMT_1, 0x01);  
    i2cWriteByte(MPU9250_ADDRESS, PWR_MGMT_2, 0x00);
    delay(200);                                    

    // Configure device for bias calculation
    i2cWriteByte(MPU9250_ADDRESS, INT_ENABLE, 0x00);   // Disable all interrupts
    i2cWriteByte(MPU9250_ADDRESS, FIFO_EN, 0x00);      // Disable FIFO
    i2cWriteByte(MPU9250_ADDRESS, PWR_MGMT_1, 0x00);   // Turn on internal clock source
    i2cWriteByte(MPU9250_ADDRESS, I2C_MST_CTRL, 0x00); // Disable I2C master
    i2cWriteByte(MPU9250_ADDRESS, USER_CTRL, 0x00);    // Disable FIFO and I2C master modes
    i2cWriteByte(MPU9250_ADDRESS, USER_CTRL, 0x0C);    // Reset FIFO and DMP
    delay(15);
  
    // Configure MPU6050 gyro and accelerometer for bias calculation
    i2cWriteByte(MPU9250_ADDRESS, CONFIG, 0x01);      // Set low-pass filter to 188 Hz
    i2cWriteByte(MPU9250_ADDRESS, SMPLRT_DIV, 0x00);  // Set sample rate to 1 kHz
    i2cWriteByte(MPU9250_ADDRESS, GYRO_CONFIG, 0x00);  // Set gyro full-scale to 250 degrees per second, maximum sensitivity
    i2cWriteByte(MPU9250_ADDRESS, ACCEL_CONFIG, 0x00); // Set accelerometer full-scale to 2 g, maximum sensitivity

    // Configure FIFO to capture accelerometer and gyro data for bias calculation
    i2cWriteByte(MPU9250_ADDRESS, USER_CTRL, 0x40);   // Enable FIFO  
    i2cWriteByte(MPU9250_ADDRESS, FIFO_EN, 0x78);     // Enable gyro and accelerometer sensors for FIFO  (max size 512 bytes in MPU-9150)
    delay(40); // accumulate 40 samples in 40 milliseconds = 480 bytes

    // At end of sample accumulation, turn off FIFO sensor read
    i2cWriteByte(MPU9250_ADDRESS, FIFO_EN, 0x00);        // Disable gyro and accelerometer sensors for FIFO
    i2cRead(MPU9250_ADDRESS, FIFO_COUNTH, 2, &data[0]); // read FIFO sample count
    fifo_count = ((uint16_t)data[0] << 8) | data[1];
    packet_count = fifo_count/12;// How many sets of full gyro and accelerometer data for averaging

    for (ii = 0; ii < packet_count; ii++)
    {
      int16_t accel_temp[3] = {0, 0, 0}, gyro_temp[3] = {0, 0, 0};
      i2cRead(MPU9250_ADDRESS, FIFO_R_W, 12, &data[0]); // read data for averaging
      accel_temp[0] = (int16_t) (((int16_t)data[0] << 8) | data[1]  );  // Form signed 16-bit integer for each sample in FIFO
      accel_temp[1] = (int16_t) (((int16_t)data[2] << 8) | data[3]  );
      accel_temp[2] = (int16_t) (((int16_t)data[4] << 8) | data[5]  );
      gyro_temp[0]  = (int16_t) (((int16_t)data[6] << 8) | data[7]  );
      gyro_temp[1]  = (int16_t) (((int16_t)data[8] << 8) | data[9]  );
      gyro_temp[2]  = (int16_t) (((int16_t)data[10] << 8) | data[11]);
    
      accel_bias[0] += (int32_t) accel_temp[0]; // Sum individual signed 16-bit biases to get accumulated signed 32-bit biases
      accel_bias[1] += (int32_t) accel_temp[1];
      accel_bias[2] += (int32_t) accel_temp[2];
      gyro_bias[0]  += (int32_t) gyro_temp[0];
      gyro_bias[1]  += (int32_t) gyro_temp[1];
      gyro_bias[2]  += (int32_t) gyro_temp[2];
    }
    accel_bias[0] /= (int32_t) packet_count; // Normalize sums to get average count biases
    accel_bias[1] /= (int32_t) packet_count;
    accel_bias[2] /= (int32_t) packet_count;
    gyro_bias[0]  /= (int32_t) packet_count;
    gyro_bias[1]  /= (int32_t) packet_count;
    gyro_bias[2]  /= (int32_t) packet_count;

    if(accel_bias[2] > 0L) {accel_bias[2] -= (int32_t) accelsensitivity;}  // Remove gravity from the z-axis accelerometer bias calculation
    else {accel_bias[2] += (int32_t) accelsensitivity;}        

    // Construct the gyro biases for push to the hardware gyro bias registers, which are reset to zero upon device startup
    data[0] = (-gyro_bias[0]/4  >> 8) & 0xFF; // Divide by 4 to get 32.9 LSB per deg/s to conform to expected bias input format
    data[1] = (-gyro_bias[0]/4)       & 0xFF; // Biases are additive, so change sign on calculated average gyro biases
    data[2] = (-gyro_bias[1]/4  >> 8) & 0xFF;
    data[3] = (-gyro_bias[1]/4)       & 0xFF;
    data[4] = (-gyro_bias[2]/4  >> 8) & 0xFF;
    data[5] = (-gyro_bias[2]/4)       & 0xFF;

    // Push gyro biases to hardware registers
    i2cWriteByte(MPU9250_ADDRESS, XG_OFFSET_H, data[0]);
    i2cWriteByte(MPU9250_ADDRESS, XG_OFFSET_L, data[1]);
    i2cWriteByte(MPU9250_ADDRESS, YG_OFFSET_H, data[2]);
    i2cWriteByte(MPU9250_ADDRESS, YG_OFFSET_L, data[3]);
    i2cWriteByte(MPU9250_ADDRESS, ZG_OFFSET_H, data[4]);
    i2cWriteByte(MPU9250_ADDRESS, ZG_OFFSET_L, data[5]);

    // Output scaled gyro biases for display in the main program
    gyroBias[0] = (float) gyro_bias[0]/(float) gyrosensitivity;  
    gyroBias[1] = (float) gyro_bias[1]/(float) gyrosensitivity;
    gyroBias[2] = (float) gyro_bias[2]/(float) gyrosensitivity;

    // Construct the accelerometer biases for push to the hardware accelerometer bias registers. These registers contain
    // factory trim values which must be added to the calculated accelerometer biases; on boot up these registers will hold
    // non-zero values. In addition, bit 0 of the lower byte must be preserved since it is used for temperature
    // compensation calculations. Accelerometer bias registers expect bias input as 2048 LSB per g, so that
    // the accelerometer biases calculated above must be divided by 8.

    int32_t accel_bias_reg[3] = {0, 0, 0}; // A place to hold the factory accelerometer trim biases
    i2cRead(MPU9250_ADDRESS, XA_OFFSET_H, 2, &data[0]); // Read factory accelerometer trim values
    accel_bias_reg[0] = (int32_t) (((int16_t)data[0] << 8) | data[1]);
    i2cRead(MPU9250_ADDRESS, YA_OFFSET_H, 2, &data[0]);
    accel_bias_reg[1] = (int32_t) (((int16_t)data[0] << 8) | data[1]);
    i2cRead(MPU9250_ADDRESS, ZA_OFFSET_H, 2, &data[0]);
    accel_bias_reg[2] = (int32_t) (((int16_t)data[0] << 8) | data[1]);
  
    uint32_t mask = 1uL; // Define mask for temperature compensation bit 0 of lower byte of accelerometer bias registers
    uint8_t mask_bit[3] = {0, 0, 0}; // Define array to hold mask bit for each accelerometer bias axis
  
    for(ii = 0; ii < 3; ii++) {
      if((accel_bias_reg[ii] & mask)) mask_bit[ii] = 0x01; // If temperature compensation bit is set, record that fact in mask_bit
    }

    // Construct total accelerometer bias, including calculated average accelerometer bias from above
    accel_bias_reg[0] -= (accel_bias[0]/8); // Subtract calculated averaged accelerometer bias scaled to 2048 LSB/g (16 g full scale)
    accel_bias_reg[1] -= (accel_bias[1]/8);
    accel_bias_reg[2] -= (accel_bias[2]/8);

    data[0] = (accel_bias_reg[0] >> 8) & 0xFF;
    data[1] = (accel_bias_reg[0])      & 0xFF;
    data[1] = data[1] | mask_bit[0]; // preserve temperature compensation bit when writing back to accelerometer bias registers
    data[2] = (accel_bias_reg[1] >> 8) & 0xFF;
    data[3] = (accel_bias_reg[1])      & 0xFF;
    data[3] = data[3] | mask_bit[1]; // preserve temperature compensation bit when writing back to accelerometer bias registers
    data[4] = (accel_bias_reg[2] >> 8) & 0xFF;
    data[5] = (accel_bias_reg[2])      & 0xFF;
    data[5] = data[5] | mask_bit[2]; // preserve temperature compensation bit when writing back to accelerometer bias registers
 
    // Apparently this is not working for the acceleration biases in the MPU-9250
    // Are we handling the temperature correction bit properly?
    // Push accelerometer biases to hardware registers
    i2cWriteByte(MPU9250_ADDRESS, XA_OFFSET_H, data[0]);
    i2cWriteByte(MPU9250_ADDRESS, XA_OFFSET_L, data[1]);
    i2cWriteByte(MPU9250_ADDRESS, YA_OFFSET_H, data[2]);
    i2cWriteByte(MPU9250_ADDRESS, YA_OFFSET_L, data[3]);
    i2cWriteByte(MPU9250_ADDRESS, ZA_OFFSET_H, data[4]);
    i2cWriteByte(MPU9250_ADDRESS, ZA_OFFSET_L, data[5]);

    // Output scaled accelerometer biases for display in the main program
    accelBias[0] = (float)accel_bias[0]/(float)accelsensitivity; 
    accelBias[1] = (float)accel_bias[1]/(float)accelsensitivity;
    accelBias[2] = (float)accel_bias[2]/(float)accelsensitivity;
    
    //AK8963 init
    // First extract the factory calibration for each magnetometer axis
    uint8_t rawData[3];  // x/y/z gyro calibration data stored here
    i2cWriteByte(AK8963_ADDRESS,CNTL1, 0x00); // Power down magnetometer  
    delay(10);
    i2cWriteByte(AK8963_ADDRESS,CNTL1, 0x0F); // Enter Fuse ROM access mode
    delay(10);
    i2cRead(AK8963_ADDRESS, AK8963_ASAX, 3, &rawData[0]);  // Read the x-, y-, and z-axis calibration values
    magCalibration[0] =  (float)(rawData[0] - 128)/256. + 1.;   // Return x-axis sensitivity adjustment values, etc.
    magCalibration[1] =  (float)(rawData[1] - 128)/256. + 1.;  
    magCalibration[2] =  (float)(rawData[2] - 128)/256. + 1.; 
    i2cWriteByte(AK8963_ADDRESS,CNTL1, 0x00); // Power down magnetometer  
    delay(10);
    // Configure the magnetometer for continuous read and highest resolution
    // set Mscale bit 4 to 1 (0) to enable 16 (14) bit resolution in CNTL register,
    // and enable continuous mode data acquisition Mmode (bits [3:0]), 0010 for 8 Hz and 0110 for 100 Hz sample rates
    i2cWriteByte(AK8963_ADDRESS,CNTL1, Mscale << 4 | Mmode); // Set magnetometer data resolution and sample ODR
    delay(10);
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
    /*
    magX = (mx / 32768.0f * 4800.0f)*magCalibration[0];//[uT]に変換
    magY = (my / 32768.0f * 4800.0f)*magCalibration[1];//[uT]に変換
    magZ = (mz / 32768.0f * 4800.0f)*magCalibration[2];//[uT]に変換
    */
    
    // User environmental x-axis correction in milliGauss, should be
    // automatically calculated
    magbias[0] = +470.;
    // User environmental x-axis correction in milliGauss TODO axis??
    magbias[1] = +120.;
    // User environmental x-axis correction in milliGauss
    magbias[2] = +125.;

    magX = ((float)mx*magCalibration[0] / 32768.0f * 4921.0f * 10.0f) - magbias[0];//[mGauss]に変換
    magY = ((float)my*magCalibration[1] / 32768.0f * 4921.0f * 10.0f) - magbias[1];//[mGauss]に変換
    magZ = ((float)mz*magCalibration[2] / 32768.0f * 4921.0f * 10.0f) - magbias[2];//[mGauss]に変換]
    

    /*
    magX = (mx + 344.0f) / 32768.0f * 4921.0f * 10.0f;//[mGauss]に変換
    magY = (my - 234.0f) / 32768.0f * 4921.0f * 10.0f;//[mGauss]に変換
    magZ = (mz - 410.0f) / 32768.0f * 4921.0f * 10.0f;//[mGauss]に変換
    */
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
        case GyroX:return gyroX;
        case GyroY:return gyroY;
        case GyroZ:return gyroZ;
        case MagX:return magX;
        case MagY:return magY;
        case MagZ:return magZ;
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
    ESP_SERIAL.print(gyroX);
    ESP_SERIAL.print("/gy:");
    ESP_SERIAL.print(gyroY);
    ESP_SERIAL.print("/gz:");
    ESP_SERIAL.print(gyroZ);
    ESP_SERIAL.print("/mx:");
    ESP_SERIAL.print(magX);
    ESP_SERIAL.print("/my:");
    ESP_SERIAL.print(magY);
    ESP_SERIAL.print("/mz:");
    ESP_SERIAL.print(magZ);

    ESP_SERIAL.print("/mx:");
    ESP_SERIAL.print(mx);
    ESP_SERIAL.print("/my:");
    ESP_SERIAL.print(my);
    ESP_SERIAL.print("/mz:");
    ESP_SERIAL.print(mz);

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
    rx=ry=rz=false;

    accX=0;
    accY=0;
    accZ=0;

    gyroX=0;
    gyroY=0;

    gyroZ=0;

    degx=degy=degz=0;
}