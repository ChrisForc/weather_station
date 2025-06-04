#ifndef TASKS_H
#define TASKS_H

#include "globals.h"
#include "sensors.h"
#include "wifi_utils.h"

#define THINGSPEAK_PERIOD 15000
#define DHT_READ_PERIOD 2000
#define BMP_READ_PERIOD 500
#define BH1750_READ_PERIOD 500
#define RTC_READ_PERIOD 500

/**
 * Tarea para leer el sensor DHT22.
 * Actualiza las variables globales: temp, hum, feels_like.
 * @param curr_time Tiempo actual en milisegundos.
 */
void taskDHT(unsigned long curr_time);

/**
 * Tarea para leer el sensor BMP180.
 * Actualiza las variables globales: BMPtemp, pres, alt.
 * @param curr_time Tiempo actual en milisegundos.
 */
void taskBMP(unsigned long curr_time);

/**
 * Tarea para leer el sensor BH1750.
 * Actualiza la variable global lux y controla los LEDs según el nivel de luz.
 * @param curr_time Tiempo actual en milisegundos.
 */
void taskBH1750(unsigned long curr_time);

/**
 * Tarea para leer el RTC DS3231.
 * Actualiza las variables globales: now, RTCtemp.
 * @param curr_time Tiempo actual en milisegundos.
 */
void taskRTC(unsigned long curr_time);

/**
 * Tarea para enviar datos a ThingSpeak.
 * Actualiza los campos de ThingSpeak con los valores actuales.
 * @param curr_time Tiempo actual en milisegundos.
 */
void taskThingSpeak(unsigned long curr_time);

#endif // TASKS_H