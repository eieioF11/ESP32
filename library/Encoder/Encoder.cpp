#include"Encoder.h"
#define Pi 3.1415926535897932384626433832795028841971f
Encoder::Encoder(int A,int B,int resolution_,pcnt_unit_t pcnt_)
{
    Resolution=resolution_;
    pinA=A;
    pinB=B;
    pcnt=pcnt_;
    pcnt_config_t pcnt_config_A;//設定用の構造体の宣言(A相)   
        pcnt_config_A.pulse_gpio_num = pinA;
        pcnt_config_A.ctrl_gpio_num = pinB;
        pcnt_config_A.lctrl_mode = PCNT_MODE_REVERSE;
        pcnt_config_A.hctrl_mode = PCNT_MODE_KEEP;
        pcnt_config_A.channel = PCNT_CHANNEL_0;
        pcnt_config_A.unit = pcnt;
        pcnt_config_A.pos_mode = PCNT_COUNT_INC;
        pcnt_config_A.neg_mode = PCNT_COUNT_DEC;
        pcnt_config_A.counter_h_lim = PCNT_H_LIM_VAL;
        pcnt_config_A.counter_l_lim = PCNT_L_LIM_VAL;

    pcnt_config_t pcnt_config_B;//設定用の構造体の宣言(B相)   
        pcnt_config_B.pulse_gpio_num = pinB;//↓A相用のconfigで設定したのと入れ替える
        pcnt_config_B.ctrl_gpio_num = pinA;//↑A相用のconfigで設定したのと入れ替える
        pcnt_config_B.lctrl_mode = PCNT_MODE_KEEP;//↓A相用のconfigで設定したのと入れ替える
        pcnt_config_B.hctrl_mode = PCNT_MODE_REVERSE;//↑A相用のconfigで設定したのと入れ替える
        pcnt_config_B.channel = PCNT_CHANNEL_1;//チャンネルを1に設定
        pcnt_config_B.unit = pcnt;//UNITはA相と同じものに
        pcnt_config_B.pos_mode = PCNT_COUNT_INC;
        pcnt_config_B.neg_mode = PCNT_COUNT_DEC;
        pcnt_config_B.counter_h_lim = PCNT_H_LIM_VAL;
        pcnt_config_B.counter_l_lim = PCNT_L_LIM_VAL;

    pcnt_unit_config(&pcnt_config_A);//ユニット初期化A相
    pcnt_unit_config(&pcnt_config_B);//ユニット初期化B相
    pcnt_counter_pause(pcnt);//カウンタ一時停止
    pcnt_counter_clear(pcnt);//カウンタ初期化
    pcnt_counter_resume(pcnt);//カウント開始
    pinMode(A,PULLDOWN);
    pinMode(B,PULLDOWN);
    reset();
    Reverse(false);
}
void Encoder::reset()
{
    nowtime=oldtime=sum=0;
    rps=0.0f;
    pcnt_counter_clear(pcnt);
}
void Encoder::Reverse(bool REV)
{
    if(REV)rev=-1;
    else rev=1;
}
void Encoder::spreset()
{
    rps=0.0f;
}
int16_t Encoder::get_sum()
{
    return sum;
}
float Encoder::get_rotation()
{
    return (float)sum/Resolution;
}
float Encoder::get_angle(bool mode)
{
    if(mode==true)return (float)sum*(360.0f/(float)Resolution);
    else return (float)sum*(2*Pi/(float)Resolution);
}
float Encoder::get_rps()
{
    return rps;
}
float Encoder::get_w()
{
    return 2.0f*Pi*rps;
}
void Encoder::Update()
{
    sum=0;
    pcnt_get_counter_value(pcnt, &sum);
    nowtime=micros();
    float t=float(nowtime-oldtime)*1.0E-6;
    rps=(get_rotation()/t)*(float)rev;
    oldtime=nowtime;
    pcnt_counter_clear(pcnt);
}