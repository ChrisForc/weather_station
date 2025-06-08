#ifndef SENSORS_H
#define SENSORS_H

#include "config.h"
#include <DHT.h>
#include <Adafruit_BMP085.h>
#include <BH1750.h>
#include <RTClib.h>
#include <Wire.h>
#include <SdFat.h>
#include "sdios.h"

extern SdFs sd;

enum : uint8_t {
    DHT22_TEMP = 0,
    DHT22_HUM = 1,
    DHT22_FEELS = 2,
    BMP180_PRES = 3,
    BMP180_ALT = 4,
    BH1750_LUX = 5,
};

struct sensorEntry {
    uint8_t sensor_id;
    uint32_t timestamp;
    float value;
};


/**
 * Inicializa la tarjeta SD.
 * Debe llamarse antes de usar cualquier función relacionada con la SD.
 */
void initSD();

/**
 * Agrega una entrada al buffer de sensores.
 * Si el buffer está lleno, se volcará a la tarjeta SD.
 * @param sensor_id ID del sensor (DHT22_TEMP, DHT22_HUM, etc.)
 * @param value Valor del sensor a agregar al buffer
 */
void addToBuffer(uint8_t sensor_id, float value);

/**
 * Vuelca el buffer de sensores a la tarjeta SD.
 * @param now (entrada) objeto DateTime con la fecha/hora actual
 * @return true si el volcado fue exitoso; false si hubo un error.
 */
bool flushBufferToSD(DateTime &now);

/**
 * Inicializa los sensores:
 * - RTC DS3231
 * - BMP180
 * - BH1750
 * - DHT22
 */
void initSensors();

/**
 * Lee DHT22 (temperatura en °C y humedad en %).
 * @param temp  (salida) temperatura en °C
 * @param hum   (salida) humedad en %
 * @param feels_like (salida) temperatura "siente como" en °C
 * @return true si la lectura fue correcta; false si el sensor devuelve NaN.
 */
bool readDHT(float &temp, float &hum, float &feels_like);

/**
 * Lee BMP180:
 * @param BMPtemp         (salida) temperatura en °C
 * @param pres         (salida) presión en hPa
 * @param alt          (salida) altitud en metros (usa seaLevelPres de config.h)
 */
void readBMP(float &BMPtemp, float &pres, float &alt, float seaLevelPres = 101325.0f);

/**
 * Lee BH1750 y devuelve nivel de luz en lux.
 * @param temp         (salida) lux medido. Si falla, devuelve un valor negativo.
 */
void readBH1750(float &lux);

/**
 * Lee el RTC DS3231 y devuelve un objeto DateTime con la fecha/hora actual.
 * @param now          (salida) objeto DateTime con la fecha/hora actual
 * @param RTCtemp      (salida) temperatura del RTC en °C
 */
void readRTC(DateTime &now, float &RTCtemp);

#endif // SENSORS_H