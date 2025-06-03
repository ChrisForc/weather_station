#ifndef WIFI_UTILS_H
#define WIFI_UTILS_H

#include <Arduino.h>
#include "config.h"
#include "secrets.h"
#include <WiFi.h>
#include <HTTPClient.h> 
#include <ArduinoJson.h>
#include "secrets.h"
#include <ThingSpeak.h>

extern String country_name;
extern String city;
extern float latitude;
extern float longitude;
extern int thingspeak_http_code;
extern WiFiClient wifi_client;



/**
 * Inicializa la conexión Wi-Fi.
 * Utiliza los valores de WIFI_SSID y WIFI_PASSWORD definidos en config.h.
 */
void connectWifi();

/**
 * Obtiene la geolocalización del dispositivo a través de un servicio web.
 * Guarda los resultados en las variables globales:
 * - country_name
 * - city
 * - latitude
 * - longitude
 */
void getGeoLocation();

/**
 * Inicia la conexión al cliente de ThingSpeak.
 * Configura el cliente Wi-Fi y ThingSpeak.
 */
bool startThingSpeak();

/**
 * Envía los datos de los campos a ThingSpeak.
 * @param fields Array de valores a enviar a ThingSpeak.
 */
void sendToThingSpeak(float fields[]);

#endif // WIFI_UTILS_H