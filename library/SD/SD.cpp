#include"SD.h"

SDcard sd;

bool SDcard::begin(gpio_num_t Miso,gpio_num_t Mosi,gpio_num_t Clk,gpio_num_t Cs)
{
    bool end=false;
    miso=Miso;
    mosi=Mosi;
    clk=Clk;
    cs=Cs;
    if(SDCH()==0)end=SDini_SPI();
    else Serial.println("\n\rSDcard is not found ");
    Serial.println(end);
    return end;
}
void SDcard::begin()
{
    if(SDCH()==0)SDini_SDMMC();
    else Serial.println("\n\rSDcard is not found");
}
bool SDcard::SDini_SPI()
{
    Serial.println();
    Serial.println("---------SD---------");
    Serial.println("Initializing SD card");
    Serial.println("Using SPI peripheral");
    gpio_set_pull_mode(miso, GPIO_PULLUP_ONLY);   // CMD, needed in 4- and 1- line modes
    gpio_set_pull_mode(mosi, GPIO_PULLUP_ONLY);    // D0, needed in 4- and 1-line modes
    gpio_set_pull_mode(clk, GPIO_PULLUP_ONLY);    // D1, needed in 4-line mode only
    gpio_set_pull_mode(cs, GPIO_PULLUP_ONLY);   // D2, needed in 4-line mode only
    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    sdspi_slot_config_t slot_config = SDSPI_SLOT_CONFIG_DEFAULT();
    slot_config.gpio_miso = miso;
    slot_config.gpio_mosi = mosi;
    slot_config.gpio_sck  = clk;
    slot_config.gpio_cs   = cs;
    //ファイルシステムをマウントするためのオプション。
    // format_if_mount_failedがtrueに設定されている場合、SDカードは分割されて
    //マウントに失敗した場合に備えてフォーマットされています。
    esp_vfs_fat_sdmmc_mount_config_t mount_config = 
    {
        .format_if_mount_failed = false,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024
    };
    //上で定義された設定を使用してSDカードを初期化し、FATファイルシステムをマウントします。
    //注：esp_vfs_fat_sdmmc_mountはオールインワンの便利な関数です。
    //開発時にソースコードを確認してエラー回復を実装してください
    //本番アプリケーション。
    esp_err_t ret = esp_vfs_fat_sdmmc_mount("/sdcard", &host, &slot_config, &mount_config, &card);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            Serial.println("Failed to mount filesystem. ");
        } else {
            Serial.println("Failed to initialize the card. ");
        }
        return false;
    }
    //カードは初期化されました、その特性を印刷します
    sdmmc_card_print_info(stdout, card);
    Serial.println("--------------------");
    SD_mount=true;
    return true;
}
void SDcard::SDini_SDMMC()
{
    Serial.println();
    Serial.println("---------SD---------");
    Serial.println("Initializing SD card");
    Serial.println("Using SDMMC peripheral");
    sdmmc_host_t host = SDMMC_HOST_DEFAULT();

    // This initializes the slot without card detect (CD) and write protect (WP) signals.
    // Modify slot_config.gpio_cd and slot_config.gpio_wp if your board has these signals.
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();

    // To use 1-line SD mode, uncomment the following line:
    // slot_config.width = 1;

    // GPIOs 15, 2, 4, 12, 13 should have external 10k pull-ups.
    // Internal pull-ups are not sufficient. However, enabling internal pull-ups
    // does make a difference some boards, so we do that here.
    gpio_set_pull_mode(GPIO_NUM_15, GPIO_PULLUP_ONLY);   // CMD, needed in 4- and 1- line modes
    gpio_set_pull_mode(GPIO_NUM_2, GPIO_PULLUP_ONLY);    // D0, needed in 4- and 1-line modes
    gpio_set_pull_mode(GPIO_NUM_4, GPIO_PULLUP_ONLY);    // D1, needed in 4-line mode only
    gpio_set_pull_mode(GPIO_NUM_12, GPIO_PULLUP_ONLY);   // D2, needed in 4-line mode only
    gpio_set_pull_mode(GPIO_NUM_13, GPIO_PULLUP_ONLY);   // D3, needed in 4- and 1-line modes

    //ファイルシステムをマウントするためのオプション。
    // format_if_mount_failedがtrueに設定されている場合、SDカードは分割されて
    //マウントに失敗した場合に備えてフォーマットされています。
    esp_vfs_fat_sdmmc_mount_config_t mount_config = 
    {
        .format_if_mount_failed = false,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024
    };
    //上で定義された設定を使用してSDカードを初期化し、FATファイルシステムをマウントします。
    //注：esp_vfs_fat_sdmmc_mountはオールインワンの便利な関数です。
    //開発時にソースコードを確認してエラー回復を実装してください
    //本番アプリケーション。
    esp_err_t ret = esp_vfs_fat_sdmmc_mount("/sdcard", &host, &slot_config, &mount_config, &card);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            Serial.println("Failed to mount filesystem. ");
        } else {
            Serial.println("Failed to initialize the card. ");
        }
        return;
    }
    //カードは初期化されました、その特性を印刷します
    sdmmc_card_print_info(stdout, card);
    Serial.println("--------------------");   
    SD_mount=true; 
}
bool SDcard::mount()
{
    return SD_mount;
}

void SDcard::unmount()
{
    esp_vfs_fat_sdmmc_unmount();
    Serial.println("SDcard  unmounted");
    SD_mount=false;
    UNMOUNT=true;
}