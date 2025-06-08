#ifndef CONFIG_H
#define CONFIG_H

// DHT22 pins
#define DHTPIN     4
#define DHTTYPE    DHT22

// LEDs'pins 
#define LED_BUILTIN  2   // LED interno de la placa (si existe)
#define LED_RED     27
#define LED_YELLOW  14
#define LED_GREEN   12

// TFT ST7789 pins
#define TFT_DC    13
#define TFT_CS    15
#define TFT_RST   -1    // -1 if connected directly to 3.3V (no reset pin)

// MicroSD card pins
#define SD_CS           5
#define SD_CONFIG       SdSpiConfig(SD_CS, SHARED_SPI)
#define SD_FAT_TYPE     3
// #define SD_MISO 19

// Wi-Fi settings
#define WIFI_SSID     "Totalplay-BBA5"
#define WIFI_PASSWORD "BBA5DBC89x7wCcvU"
// #define WIFI_SSID     "Rog PC"
// #define WIFI_PASSWORD "z6bk18.h"
#define WIFI_TIMEOUT 15000


// ANSI color codes for terminal output
#define ANSI_RESET   "\033[0m"
#define ANSI_RED     "\033[31m"
#define ANSI_GREEN   "\033[32m"
#define ANSI_BLUE    "\033[34m"
#define ANSI_PURPLE  "\033[35m"
#define ANSI_YELLOW  "\033[33m"


#endif // CONFIG_H
