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

// Wi-Fi settings
#define WIFI_SSID     "Totalplay-BBA5"
#define WIFI_PASSWORD "BBA5DBC89x7wCcvU"
// #define WIFI_SSID     "Rog PC"
// #define WIFI_PASSWORD "z6bk18.h"
#define WIFI_TIMEOUT 15000

#endif // CONFIG_H
