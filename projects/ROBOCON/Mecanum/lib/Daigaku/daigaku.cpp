/*---daigaku.cpp---*/
#include "daigaku.h"

Daigaku::Daigaku(int _RCLK,int _CS,int _MISO){
    int count = 0;
    rclk = _RCLK;
    cs = _CS;
    miso = _MISO;
    pinMode(rclk,OUTPUT);                                       //RCLKをディジタル出力として宣言。
    pinMode(cs,OUTPUT);                                         //CSをディジタル出力として宣言。
    digitalWrite(rclk,LOW);                                     //RCLKをLOWにして送信可能にする。
    digitalWrite(cs,HIGH);                                      //CSをHIGHにしてMISO,MOSIをハイインピーダンスにする。
    for(count = 0;count < ALL_BYTE * 8;count++){
        sw_status[count] = 0;                                   //受信用変数のリセット。
    }
    reset();
}

void Daigaku::shift(void){
    int count = 0;
    unsigned char sw[ALL_BYTE] = {0};                           //スイッチの状態を一時的に保存。
    for(count = 0;count < ALL_BYTE * 8;count++){
        solenoid.all_data += send_data[count] * power(count);   //受け取ったデータを10進数にして、共用体にぶっ込む。
    }
    output();                                                   //スイッチの状態をシフトレジスタに読ませる。
    digitalWrite(cs,LOW);                                       //CSをLOWにしてMISO,MOSI有効化する。
    for(count = 0;count < ALL_BYTE;count++){                    //受信データは1バイトずつ処理する。
        read_msb();                                             //受信データのmsbを読む。
        sw[count] = msb * 128 + (SPI.transfer(solenoid.part_data[ALL_BYTE - 1- count]) >> 1);         //msbからlsbまでを統合する。シフトレジスタに値をぶっ込む。
    }
    digitalWrite(cs,HIGH);                                      //CSをHIGHにしてMISO,MOSIをハイインピーダンスに戻す。
    division(sw);                                               //スイッチの状態を配列に反映。
    output();                                                   //電磁弁の出力に反映。
    solenoid.all_data = 0;                                      //これやらないと火だるま。試しにコメントアウトしてみろ。
}

void Daigaku::reset(void){
    solenoid.all_data = 0;
    int count = 0;
    digitalWrite(cs,LOW);                                       //CSをLOWにしてMISO,MOSI有効化する。
    for(count = 0;count < ALL_BYTE;count++){
        SPI.transfer(solenoid.part_data[count]);                //シフトレジスタをリセット。
    }
    digitalWrite(cs,HIGH);                                      //CSをHIGHにしてMISO,MOSIをハイインピーダンスに戻す。
    for(count = 0;count < ALL_BYTE * 8;count++){
        send_data[count] = 0;                                   //送信用変数のリセット。
    }
    output();
}

void Daigaku::output(void){
    digitalWrite(rclk,HIGH);   //ストレージレジスタのクロックを立ち上げて出力に反映。また、この時にシフトレジスタが値を読み直す。
    digitalWrite(rclk,LOW);    //RCLKをLOWにして送信可能にする。
}

Daigaku::~Daigaku(){
    reset();
    SPI.end();                 //通信終了！
}

unsigned int Daigaku::power(char val){
    unsigned int temp = 1,count = 0;
    for(count = 0;count < val;count++){
        temp *= 2;
    }
    return temp;
}

void Daigaku::division(unsigned char *val){
    int count_1 = 0,count_2 = 0,count_3 = 0;
    for(count_3 = 0;count_3 < ALL_BYTE;count_3++){
        for(count_1 = 0;val[count_3] > 0;count_1++){
            sw_status[count_1 + 8 * count_3] = val[count_3] % 2;
            val[count_3] /= 2;
        }
        for(count_2 = 0;count_2 < ALL_BYTE * 8 - count_1;count_2++){
            sw_status[count_1 + count_2 + 8 * count_3] = 0;
        }
    }
}

void Daigaku::read_msb(void){
  pinMode(miso,INPUT);         //spi通信のバスを一時的にIOとして使う。
  msb = digitalRead(miso);
}