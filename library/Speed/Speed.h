#ifndef SPEED_H_
#define SPEED_H_
#include <Arduino.h>

#if defined __cplusplus
extern "C" {
#endif

#define Speed_V 0
#define Speed_W 1

class Speed
{
        private:
            uint8_t mode;
            float rps;
            float R;
            float lim;
        public:
            Speed(uint8_t mode, float R,float lim=1.0); //R[m]
            Speed& operator = (float val);//[m/s] or [rad/s]
            operator float ();
};

#if defined __cplusplus
}
#endif

#endif