#include "ururuMD.h"

ururuMD::ururuMD(){
    Mode = SPEED_MODE;
}

ururuMD::ururuMD(uint8_t cs){
    Mode = SPEED_MODE;
    CS = cs;
    pinMode(CS, OUTPUT);
    digitalWrite(CS, 1);
}

ururuMD::ururuMD(uint8_t cs, int ControlMode){
    Mode = ControlMode;
    CS = cs;
    pinMode(CS, OUTPUT);
    digitalWrite(CS, 1);
}

void ururuMD::Move(double val){
  if(Mode == SPEED_MODE){
    val *= PulseResolution;
    val *= sampleT;
    Send(PID_SetSpeed, RotaryEncorder, (uint16_t)val);
    now_val =  (double)(int16_t)data.data.val / PulseResolution / sampleT;
    //printf(" Speed = %6.2lf ", now_val);
  }
  else if(Mode == ANGLE_MODE){
    val /= 360.0;
    val *= AngleResolution;
    Send(PID_SetAngle, RotaryEncorder, (uint16_t)val);
    printf("val = %d ", (uint16_t)val);
    now_val = ((float)data.data.val / (float)AngleResolution) * 360.0;
    printf(" Angle = %6.2f ", now_val);
  }
  else if(Mode == DUTY_MODE){
    val*=511;
    //Duty limit
    if(val>511)
      val=511;
    else if(val<-511)
      val=-511;
    Send(SetDuty, Nothing, (uint16_t)val+512);
  }
}

void ururuMD::SetGain(){
  K[0] = (int16_t)(Kp + Ki * sampleT + Kd/sampleT);
  K[1] = ((int16_t)(- Kp -  (2.0 * Kd / sampleT)));
  K[2] = (int16_t)(Kd / sampleT);
  printf(" P = %5f I = %5f D = %5f K1 = %5d K2 = %5d K3 = %5d\n", Kp, Ki, Kd, K[0], K[1], K[2]);
  
  delay(100);
  for(int i = 0; i < 3; i++){
    Send((MD_Command_t)(PID_SetGainK1 + i), Nothing, K[i]);
    delay(30);
  }
  Send(PID_SaveSetting, Nothing, 0);
  delay(100);
}

void ururuMD::GetGain(){
  for(int i = 0; i < 3; i++){
    Send(SetDuty, (MD_Inquire_t)(PID_GainK1 + i), 512);
    K[i] = data.data.val;
    delay(30);
  }
}

double ururuMD::GetTempureture(){
  Send(Information, H_Bridge_Tempreture, 0);
  double Vth = FVR / 1024.0 * (double)data.data.val;
  double Rth = Rp / (Vin/Vth - 1.0);
  double Tempreture = 1.0 / (1.0/Bconst * log(Rth/Rth0) + 1.0/T0); 
  Tempreture -= 273.15;
  printf("Tempreture = %.2f ℃\n", Tempreture);
  return Tempreture;
}

bool ururuMD::VoltageDetect(){
  Send(Information, H_Bridge_Voltage, 0);
  if(data.data.val > V_Threshold)
    return false;
  else
    return true;    
}

uint8_t ururuMD::Send(MD_Command_t cmd, MD_Inquire_t inq, uint16_t val){
  int DataCount;

  MD_Receive_t md_receive;
  MD_Send_t md_send;
  md_receive.data.cmd = inq;
  md_receive.data.cmd |= cmd << 3;
  md_receive.data.val = val;
  md_receive.data.crc = CRC8(md_receive.byte, 3);  

  digitalWrite(CS, LOW);
  delayMicroseconds(100);
  for(DataCount = 0; DataCount < DataNum; DataCount++){
    md_send.byte[DataCount] = SPI.transfer(md_receive.byte[DataCount]);
    delayMicroseconds(100);
  }
  digitalWrite(CS, HIGH);

  uint8_t crc = CRC8(md_send.byte+1, 3);

  if(!crc)
    data = md_send;
  /* 
  for(DataCount = 0; DataCount < DataNum; DataCount++)
    printf("%3d ", md_send.byte[DataCount]);  
  printf(" val = %6d\n", (int16_t)data.data.val);
  printf("md_rcv val = %d\n",(int16_t)val);
  */
  return 0;
}

uint8_t ururuMD::CRC8(uint8_t *data, int size){
  uint8_t crc8 = 0x00;
  for(int i = 0; i < size; i++){
    crc8 ^= *data++;
    for(int j = 0; j < 8; j++){
      if(crc8 & 0x80){
        crc8 <<= 1;
        crc8 ^= Polynomial;
      }
      else
        crc8 <<= 1;
    }
  }
  return crc8;
}