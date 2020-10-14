#ifndef MSPI_H_
#define MSPI_H_
#include <Arduino.h>
#include <SPI.h>

#if defined __cplusplus
extern "C" {
#endif

class MSPI{
    public:
    SPIClass *spi;
    MSPI(SPIClass *spi_);
    uint8_t write(uint8_t data_);
};

#if defined __cplusplus
}
#endif

#endif