#ifndef DIGITALOUT_H_
#define DIGITALOUT_H_
#include <Arduino.h>

#if defined __cplusplus
extern "C" {
#endif

class DigitalOut{
        private:
        gpio_num_t gpio;
        int gpio_level;
        public:
        DigitalOut();
        void init(gpio_num_t gpio_num);
        DigitalOut(gpio_num_t gpio_num);
        DigitalOut& operator = (uint32_t level);
        operator int ();
};

#if defined __cplusplus
}
#endif

#endif