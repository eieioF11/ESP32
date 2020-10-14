#ifndef MAG_H_
#define MAG_H_
#include <Arduino.h>
#include <EEPROM.h>
#if defined __cplusplus
extern "C" {
#endif

class Compass
{
  private:
    float dx, dy, dz, f; 
    const float lr=0.00001;
    const int addr0=0;
    const int addr1=addr0+sizeof(b[0]);
    const int addr2=addr1+sizeof(b[0]);
    const int addr3=addr2+sizeof(b[0]);
    bool up;
  public:
    float b[4];          // 推定対象[x_0, y_0, z_0, r]
    bool inistart;
    Compass();
    void begin();
    void update(float x, float y, float z);
    void save();
    void reset();
};

extern Compass CO;

#if defined __cplusplus
}
#endif
#endif