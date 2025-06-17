#include "tasks.h"

unsigned long thingspeak_timer = 0;
unsigned long DHT_timer = 0;
unsigned long BMP_timer = 0;
unsigned long BH1750_timer = 0;
unsigned long RTC_timer = 0;

void taskDHT(unsigned long curr_time) {
    if (curr_time - DHT_timer >= DHT_READ_PERIOD) {
        DHT_timer = curr_time;    
        if (!readDHT(temp, hum, feels_like)) {
        Serial.println("Error reading DHT22");
        }
        if (temp < 25) {
        digitalWrite(LED_RED, LOW);
        digitalWrite(LED_YELLOW, LOW);
        digitalWrite(LED_GREEN, HIGH);
        } else if (temp < 30) {
        digitalWrite(LED_RED, LOW);
        digitalWrite(LED_YELLOW, HIGH);
        digitalWrite(LED_GREEN, LOW);
        } else {
        digitalWrite(LED_RED, HIGH);
        digitalWrite(LED_YELLOW, LOW);
        digitalWrite(LED_GREEN, LOW);
        }    
    }
}

void taskBMP(unsigned long curr_time) {
    if (curr_time - BMP_timer >= BMP_READ_PERIOD) {
        BMP_timer = curr_time;
        readBMP(BMPtemp, pres, alt, seaLevelPres);
    }
}

void taskBH1750(unsigned long curr_time) {
    if (curr_time - BH1750_timer >= BH1750_READ_PERIOD) {
        BH1750_timer = curr_time;
        readBH1750(lux);
        // if (lux < 300) {
        // digitalWrite(LED_RED, HIGH);
        // digitalWrite(LED_YELLOW, LOW);
        // digitalWrite(LED_GREEN, LOW);
        // } else if (lux < 3000) {
        // digitalWrite(LED_RED, LOW);
        // digitalWrite(LED_YELLOW, HIGH);
        // digitalWrite(LED_GREEN, LOW);
        // } else {
        // digitalWrite(LED_RED, LOW);
        // digitalWrite(LED_YELLOW, LOW);
        // digitalWrite(LED_GREEN, HIGH);
        // }    
    }
}
    
void taskRTC(unsigned long curr_time) {
    if (curr_time - RTC_timer >= RTC_READ_PERIOD) {
        RTC_timer = curr_time;
        readRTC(now, RTCtemp);
    }
}
    
void taskThingSpeak(unsigned long curr_time) {
    if (curr_time - thingspeak_timer >= THINGSPEAK_PERIOD) {
        thingspeak_timer = curr_time;
        digitalWrite(LED_BUILTIN, HIGH);
        fields[0] = temp;          // Field 1: temp
        fields[1] = hum;           // Field 2: hum
        fields[2] = pres;          // Field 3: pres
        fields[3] = alt;           // Field 4: alt
        fields[4] = BMPtemp;       // Field 5: BMPtemp
        fields[5] = feels_like;    // Field 6: feels_like
        fields[6] = lux;           // Field 7: lux
        sendToThingSpeak(fields);
        digitalWrite(LED_BUILTIN, LOW);
    }
}
