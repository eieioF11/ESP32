#ifndef SD_H_
#define SD_H_
#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "driver/sdmmc_host.h"
#include "driver/sdspi_host.h"
#include "sdmmc_cmd.h"
#include <Arduino.h>
#include "Setup.h"

#if defined __cplusplus
extern "C" {
#endif

class SDcard
{
    private:
        bool SDini_SPI();
        void SDini_SDMMC();
        gpio_num_t miso,mosi,clk,cs;
        bool SD_mount=false;
    public:
        sdmmc_card_t* card;
        bool UNMOUNT=false;
        void begin();
        bool begin(gpio_num_t Miso,gpio_num_t Mosi,gpio_num_t Clk,gpio_num_t Cs);
        bool mount();
        void unmount();
};
extern SDcard sd;

#if defined __cplusplus
}
#endif

#endif