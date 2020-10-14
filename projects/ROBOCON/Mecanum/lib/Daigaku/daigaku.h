/*---daigaku.h---*/
#ifndef _DAIGAKU_H_
#define _DAIGAKU_H_
#include "Arduino.h"
#include "SPI.h"

/*---ALL_BYTEは基板の枚数によって変えてください。---*/
#define ALL_BYTE 1                              //ALL_BYTE...送受信のデータ量：1~3[BYTE]

class Daigaku{
    public:
        unsigned char send_data[ALL_BYTE * 8];  //シフトレジスタに送る値を入れる。
        unsigned char sw_status[ALL_BYTE * 8];  //この変数は中身を書き換えないでください。各ポートのスイッチの状態が入る。
        Daigaku(int _RCLK,int _CS,int _MISO);   //初期処理。引数はRCLK,スレーブセレクト,マスターインスレーブアウト。MISOで受信データのmsbを読む。
        void shift(void);                       //共用体にデータを入れる。共用体のデータをシフトレジスタに送る。シフトレジスタからデータをもらう。
        void reset(void);                       //電磁弁関連の変数を初期化し、シフトレジスタ内の値もリセットする出力もリセットする。スイッチ関連の変数はそのまま。
        ~Daigaku();                             //終了処理。
    private:
        union {
            unsigned int all_data;
            unsigned char part_data[ALL_BYTE];
        }solenoid;
        unsigned int power(char val);           //計算用関数。
        void division(unsigned char *val);      //計算用関数。
        void output(void);                      //シフトレジスタ内の値を電磁弁の出力に反映する、及びスイッチの状態をシフトレジスタが読み取る。
        void read_msb(void);                    //受信するデータのmsbを取得する。ArduinoのSPI通信（受信）において必要な特殊な命令。
        unsigned char msb;                      //受信するデータのmsbを入れる。
        int rclk,cs,miso;                       //初期化用変数。
};

#endif