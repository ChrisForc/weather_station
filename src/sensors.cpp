#include "sensors.h"

static DHT              dht(DHTPIN, DHTTYPE);
static Adafruit_BMP085  bmp;
static BH1750           lightMeter;
static RTC_DS3231       rtc;

void initSensors() {
  // RTC DS3231
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC DS3231");
    Serial.flush();
    while (1) delay(10);
  }
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
//   rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  // BMP180
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP180 sensor, check wiring!");
    Serial.flush();
    while (1) delay(10);
  }

  // BH1750
  if (!lightMeter.begin()) {
    Serial.println(F("Could not find a valid BH1750 sensor, check wiring!"));
    Serial.flush();
    while (1) delay(10);
  }

  // DHT22
  dht.begin();

  Serial.println("All sensors OK");
}

bool readDHT(float &temp, float &hum, float &feels_like) {
  // Reading temperature or humidity takes about 250 milliseconds!
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float hic = dht.computeHeatIndex(t, h, false);
  if (isnan(h) || isnan(t)) {
    return false;
  }
  temp = t;
  hum  = h;
  feels_like = hic;
  return true;
}

void readBMP(float &BMPtemp, float &pres, float &alt, float seaLevelPres) {
  // internal temperature in °C
  BMPtemp = bmp.readTemperature();
  // Abs pressure in Pa → convert to hPa
  pres = bmp.readPressure() / 100.0f;
  // Altiude in meters, using sea level pressure
  alt  = bmp.readAltitude(seaLevelPres);
}

void readBH1750(float &lux) {
  // Luminosity in lux
  lux = lightMeter.readLightLevel();
}

void readRTC(DateTime &now, float &RTCtemp) {
  // Read the current date and time from the RTC
  now = rtc.now();
  // Temperature in °C from RTC
  RTCtemp = rtc.getTemperature(); 
}
