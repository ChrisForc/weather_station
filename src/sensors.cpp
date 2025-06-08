#include "sensors.h"

static DHT              dht(DHTPIN, DHTTYPE);
static Adafruit_BMP085  bmp;
static BH1750           lightMeter;
static RTC_DS3231       rtc;
static const int        BUFFER_SIZE = 512;
static sensorEntry      buffer[BUFFER_SIZE];
static size_t           buffer_index = 0;
static DateTime         rtc_now; 

#define SD_FAT_TYPE 3                         // FAT16/FAT32 + exFAT
static constexpr uint32_t SPI_SPEED = SD_SCK_MHZ(4); // Sube/baja si parpadea
SdFs sd;  

void initSensors() {
  // RTC DS3231
  if (!rtc.begin()) {
    Serial.println(ANSI_RED "Couldn't find RTC DS3231" ANSI_RESET);
    Serial.flush();
    while (1) delay(10);
  }
  if (rtc.lostPower()) {
    Serial.println(ANSI_YELLOW "RTC lost power, let's set the time!" ANSI_RESET);
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
//   rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  // BMP180
  if (!bmp.begin()) {
    Serial.println(ANSI_RED "Could not find a valid BMP180 sensor, check wiring!" ANSI_RESET);
    Serial.flush();
    while (1) delay(10);
  }

  // BH1750
  if (!lightMeter.begin()) {
    Serial.println(F(ANSI_RED "Could not find a valid BH1750 sensor, check wiring!" ANSI_RESET));
    Serial.flush();
    while (1) delay(10);
  }

  // DHT22
  dht.begin();

  Serial.println(ANSI_GREEN "All sensors OK" ANSI_RESET);
}

void initSD() {
  Serial.print("Initializing SD card...");
  if (!sd.begin(SD_CS, SPI_SPEED)) {

        if (sd.card()->errorCode()) {         // Hubo tarjeta, pero falló
            Serial.print  (F(ANSI_RED "[SD] Error initializing SD: " ANSI_RESET));
            Serial.print  (F("[SD] errorCode: 0x"));
            Serial.print  (sd.card()->errorCode(), HEX);
            Serial.print  (F("  errorData: 0x"));
            Serial.println(sd.card()->errorData(), HEX);
        } else {                              // Ni siquiera detectó tarjeta
            Serial.print(F(ANSI_RED "[SD] No card or CS pin error. " ANSI_RESET));
        }
        Serial.println(F(ANSI_RED "[SD] Check wiring and lower SPI_SPEED if needed." ANSI_RESET));
        while (true) yield();                 // Sin SD no seguimos
    }
  // 3) Éxito: un poquito de info útil
    Serial.println(F(ANSI_GREEN "[SD] OK" ANSI_RESET));

    uint32_t sectors = sd.card()->sectorCount();
    uint32_t sizeMB  = (sectors + 2047) / 2048;   // 512 B * 2048 = 1 MB
    Serial.print(F(ANSI_PURPLE "[SD] Tamaño: "));
    Serial.print(sizeMB);
    Serial.println(F(" MB" ANSI_RESET));

    if (sd.fatType() <= 32) {
        Serial.print(F(ANSI_PURPLE "[SD] FAT"));
        Serial.print(sd.fatType()); 
        Serial.println(F(ANSI_RESET));
    } else {
        Serial.println(F(ANSI_PURPLE "[SD] exFAT" ANSI_RESET));
    }
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
  addToBuffer(DHT22_TEMP, temp);
  addToBuffer(DHT22_HUM, hum);
  addToBuffer(DHT22_FEELS, feels_like);
  return true;
}

void readBMP(float &BMPtemp, float &pres, float &alt, float seaLevelPres) {
  // internal temperature in °C
  BMPtemp = bmp.readTemperature();
  // Abs pressure in Pa → convert to hPa
  pres = bmp.readPressure() / 100.0f;
  // Altiude in meters, using sea level pressure
  alt  = bmp.readAltitude(seaLevelPres);

  addToBuffer(BMP180_PRES, pres);
  addToBuffer(BMP180_ALT, alt);
}

void readBH1750(float &lux) {
  // Luminosity in lux
  lux = lightMeter.readLightLevel();
  addToBuffer(BH1750_LUX, lux);
}

void readRTC(DateTime &now, float &RTCtemp) {
  // Read the current date and time from the RTC
  now = rtc.now();
  // Temperature in °C from RTC
  RTCtemp = rtc.getTemperature(); 
}

void addToBuffer(uint8_t sensor_id, float value) {
  DateTime now = rtc.now();
  if (buffer_index >= BUFFER_SIZE) {
    flushBufferToSD(now);
  } 
  buffer[buffer_index].sensor_id = sensor_id;
  buffer[buffer_index].timestamp = now.unixtime();
  buffer[buffer_index].value = value;
  buffer_index++;
}

bool flushBufferToSD(DateTime &now) {
  digitalWrite(TFT_CS, HIGH);
  if (buffer_index == 0) {
    Serial.println(ANSI_PURPLE "Buffer is empty, nothing to flush." ANSI_RESET);
    return false;
  }
  else {
    char dir_path[32];
    snprintf(dir_path, sizeof(dir_path), "/data/%04u-%02u-%02u/%02u/", now.year(), now.month(), now.day(), now.hour());

    if (!sd.exists(dir_path)) {
      if (!sd.mkdir(dir_path)) {
        Serial.printf(ANSI_RED "Failed to create directory: %s\n" ANSI_RESET, dir_path);
        return false;
      }
    }

    char file_path[64]; 
    snprintf(file_path, sizeof(file_path), "%sblock_%02u_%02u.bin", dir_path, now.minute(), now.second());

    FsFile data_block = sd.open(file_path, FILE_WRITE);
    if (!data_block) {
      Serial.printf(ANSI_RED "Failed to open file: %s\n" ANSI_RESET, file_path);
      return false;
    }
    size_t writing = buffer_index * sizeof(buffer[0]);
    size_t written = data_block.write(reinterpret_cast<uint8_t*>(buffer), writing);
    data_block.close();

    if (written != writing) {
      Serial.printf(ANSI_RED "Write error: expected %zu bytes, wrote %zu bytes\n" ANSI_RESET, writing, written);
      return false;
    }
    
    Serial.printf(ANSI_PURPLE "Flushed %zu sensor entries to %s\n" ANSI_RESET, buffer_index, file_path);
    buffer_index = 0;
  }
  return false;
}