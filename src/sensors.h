#ifndef SENSORS_H
#define SENSORS_H

#include "config.h"
#include <DHT.h>
#include <Adafruit_BMP085.h>
#include <BH1750.h>
#include <RTClib.h>
#include <Wire.h>

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