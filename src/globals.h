#ifndef GLOBALS_H
#define GLOBALS_H

#include <RTClib.h>

// RTC
extern DateTime now;   
extern float RTCtemp;
// DHT22
extern float temp;      
extern float hum;       
extern float feels_like;   
// BMP180
extern float BMPtemp;      
extern float pres;         
extern float alt;          
extern float seaLevelPres;
// BH1750
extern float lux;          

extern float fields[7];
extern char daysOfTheWeek[7][12];
extern unsigned long curr_time;

#endif // GLOBALS_H
