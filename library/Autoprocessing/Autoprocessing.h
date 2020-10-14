#ifndef AUTOPROCESSING_H_
#define AUTOPROCESSING_H_
#include <stdio.h>
#include "Arduino.h"
#include "Timer.h"

#if defined __cplusplus
extern "C" {
#endif

#define Debug_function ON//(ON or OFF)デバック関数を登録する場合はON

class Autoprocessing
{
	private:
                int taskstatus,Number_of_tasks;
                int times,told;
                bool tauto;
                bool Execution;
                const char *taskname[20];
                bool (*taskp[20])(void);
                void (*debugtask)(void);
                void Setup();
                bool taskrun();
                float t;
                Timer debug_t;
                int ernum;
	public:
                bool stop;///trueの時デバック用の関数以外の関数の処理が停止する/falseの時一度trueにしたときはその続きから処理が実行される 
                /*初期設定*/
                Autoprocessing();//登録する関数が決まっていない時はこっち
                Autoprocessing(int Times);//登録する関数の数を指定する場合はこっち     
                void reset();//変数のリセット　初めからやり直す時に実行
                /*実行すしたい関数の登録をする関数*/
                void setfunction(const char*name,bool (*tfp)(void),int tasknum);//実行する番号を手動で割り当てる場合　tasknumは1~
                void setfunction(const char*name,bool (*tfp)(void));//実行する番号を自動で割り当てる場合　登録した順に実行される
                void setfunction(void (*tfp)(void),float t);//デバックをするための関数の登録 t[sec]デバッグ関数の実行間隔
                /*その他の関数*/
                bool run();//一定間隔で常に実行すること
                void selecttask(int statusnum);//選択された関数を実行する　statusnumは1~
                int statusread();//実行中の関数の番号を返す
                int errornum();//発生しているエラーのナンバーを返す　エラーがなければ0を返す
                const char *error();//発生しているエラーを返す　エラーがなければNORMALを返す
                const char *status();//実行中の関数の登録された名前を返す　すべての関数を実行し終わるとall task endを返す
};

/*
使用例(Debug_function ON)
#include "Autoprocessing.h"

Autoprocessing Auto;

void debug();
bool task1();
bool task2(); 
bool task3();

void setup() 
{
    Serial.begin(115200);
    Auto.setfunction("starttask",task1);//1番目に実行される関数の登録
    Auto.setfunction("task2",task2);　　//2番目に実行される関数の登録
    Auto.setfunction("endtask",task3);　//3番目に実行される関数の登録
    Auto.setfunction(debug,0.2);            //デバック用の関数の登録
}

void loop() 
{
  Auto.run();//登録した関数とデバッグ用の関数を実行する。
  delay(10);//10[ms]で実行
}

void debug()//デバッグ関数　この例の場合0.2秒ごとに実行される
{
    Serial.printf("task %s,error %s\n",Auto.status(),Auto.error());
}

//終了する場合はtrueを返すと次に登録した関数が実行される　例　task1関数がtrueを返すとtask2関数が実行される
bool task1()
{
    bool end=false;
    //task1で実行したい処理 
    //end=true;//終了する場合に実行
    return end;
}
bool task2()
{
    bool end=false;
    //task2で実行したい処理
    //end=true;//終了する場合に実行
    return end;
}
bool task3()
{
    bool end=false;
    //task3で実行したい処理
    //end=true;//終了する場合に実行
    return end;
}

*/

#define normal 0//正常
#define debugtask_error 1//デバッグ関数が登録されていないもしくはデバッグ関数が実行できない
#define function_error 2 //登録した関数が実行できない

#define ON 0
#define OFF 1
#if defined __cplusplus
}
#endif
#endif