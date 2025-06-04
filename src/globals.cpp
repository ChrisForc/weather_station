#include "globals.h"

// RTC
char daysOfTheWeek[7][12] = {"Domingo", "Lunes", "Martes", "Miércoles", "Jueves", "Viernes", "Sábado"};
DateTime now;
float RTCtemp = 0.0f;    // Temperature in °C from RTC
// DHT22
float temp = 0.0f;       // Temperature in °C
float hum = 0.0f;        // Humidity en %
float feels_like = 0.0f; // "Feels like" temperature in °C
// BMP180
float pres = 0.0f;       // Pressure in hPa (Pa to hPa)
float alt = 0.0f;        // Altitude in m
float BMPtemp = 0.0f;    // Temperature in °C from BMP180
float seaLevelPres = 101325.0f; // Sea level pressure in Pa (default 1013.25 hPa)
// BH1750
float lux = 0.0f;        // Light level in lux

// Fields for ThingSpeak
float fields[7] = {
  0.0f, // Field 1: temp
  0.0f, // Field 2: hum
  0.0f, // Field 3: pres
  0.0f, // Field 4: alt
  0.0f, // Field 5: BMPtemp
  0.0f, // Field 6: feels_like
  0.0f, // Field 7: lux
};

// Current time in milliseconds
unsigned long curr_time = 0;