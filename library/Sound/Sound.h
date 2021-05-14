#ifndef SOUND_H_
#define SOUND_H_
#include "Setup.h"
#include <Arduino.h>
#include "Setup.h"
#include "math.h"

#if defined __cplusplus
extern "C" {
#endif

#define C_(octave) 16.35*pow(2,octave)//ド
#define Cs_(octave) 17.32*pow(2,octave)//ド#
#define D_(octave) 18.36*pow(2,octave)//レ
#define Eb_(octave) 19.45*pow(2,octave)//ミ♭
#define E_(octave) 20.60*pow(2,octave)//ミ
#define F_(octave) 21.83*pow(2,octave)//ファ
#define Fs_(octave) 23.12*pow(2,octave)//ファ#
#define G_(octave) 24.50*pow(2,octave)//ソ
#define Gs_(octave) 25.96*pow(2,octave)//ソ#
#define A_(octave) 27.50*pow(2,octave)//ラ
#define Bb_(octave) 29.14*pow(2,octave)//シ#
#define B_(octave) 30.87*pow(2,octave)//シ


void noTone(int pin);
void tone(int pin,int freq);

void mel1();
void mel2();
void mel3();

#define Onbeep 20
bool ONbeep();
#define ConectBeep 21
bool conectbeep(int port);
#define UnmountBeep 22
bool unmountbeep(int port);
#define ErrorBeep 23
bool errorbeep();
#define StartBeep 24
bool startbeep();
#define XPStartBeep 25
bool winxpstartbeep();
#define BTErrorBeep 26
bool BTerrorbeep();

void melreset();

#if defined __cplusplus
}
#endif
#endif