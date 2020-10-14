#ifndef STPRINT_H_
#define STPRINT_H_
#include <Arduino.h>
#if defined __cplusplus
extern "C" {
#endif

class stprint
{
  public:
  String printdata;
  stprint();
  int printf(String *data,const char *format, ...);
  int addprintf(String *data,const char *format, ...);
  int printf(const char *format, ...);
  int addprintf(const char *format, ...);
};
extern stprint St;

#if defined __cplusplus
}
#endif
#endif