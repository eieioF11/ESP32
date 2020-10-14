#include "MSPI.h"

MSPI::MSPI(SPIClass *spi_)
{
    spi = spi_;
    //spi -> begin(sclk_,miso_,mosi_,-1);
}

uint8_t MSPI::write(uint8_t data_)
{
    spi -> beginTransaction(SPISettings(100000, MSBFIRST, SPI_MODE0));
    uint8_t tmp = spi -> transfer(data_);
    spi -> endTransaction();
    return tmp;
}