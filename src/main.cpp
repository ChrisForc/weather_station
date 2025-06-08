#include "Arduino.h"
#include "config.h"     // loads DHTPIN, TFT_CS, TFT_DC, etc.
#include "sensors.h"  // loads DHT, BMP085, BH1750, RTC_DS3231
#include "wifi_utils.h"
#include "tasks.h" 
#include "globals.h"

#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <Fonts/FreeSans9pt7b.h> 
#include <SPI.h>

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS,  TFT_DC, TFT_RST);

void printTime();
void printBMP();
void printDHT();
void printBHT();
void tftPrintWeather();


unsigned long ttf_timer = 0;  // borrar cuando se pase a GUI

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(SD_CS,  OUTPUT);
  pinMode(TFT_CS, OUTPUT);

  Serial.begin(9600);

  initSensors();
  connectWifi();
  getGeoLocation();
  startThingSpeak();

  SPI.begin();
  // digitalWrite(SD_CS,  HIGH);
  // digitalWrite(TFT_CS, HIGH);
  initSD();
  tft.init(240, 320); 
  tft.setFont(&FreeSans9pt7b);


}

void loop() {
  curr_time = millis();
  taskDHT(curr_time);
  taskBMP(curr_time);
  taskBH1750(curr_time);
  taskRTC(curr_time);
  taskThingSpeak(curr_time);
  
  // Hay que pasar la función de impresión a una tarea, pero no lo he hecho porque quiero cambiar a una gui
  if (curr_time - ttf_timer >= 1000) {
    ttf_timer = curr_time;
    printTime();
    printBMP();
    printDHT();
    printBHT();
    Serial.println("----------------------------------------");
    Serial.println();

    // tftPrintTime(); 
    // tftPrintBMP();
    // tftPrintDHT();
    // tftPrintBHT();

    tftPrintWeather();
  }
}

void tftPrintWeather() {
  digitalWrite(SD_CS, HIGH);
  // Limpia la pantalla y desactiva el wrapping automático
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextWrap(false);
  tft.setFont(&FreeSans9pt7b);
  tft.setTextSize(1);               // Tamaño base para FreeSans9pt7b

  // Coordenadas iniciales y espaciado entre líneas (en píxeles)
  const int16_t x0 = 0;
  int16_t y  = 15;
  const int16_t lineHeight = 15;    

  // —— 1) Fecha y hora ——  
  // LINEA 1: DD/MM/YYYY HH:MM:SS
  tft.setCursor(x0, y);
  tft.setTextColor(ST77XX_WHITE);
  // Ejemplo: “02/06/2025 14:05:32”
  tft.printf("%02d/%02d/%04d %02d:%02d:%02d",
             now.day(), now.month(), now.year(),
             now.hour(), now.minute(), now.second());
  y += lineHeight;

  // LINEA 2: (DíaSemana)   TempRTC: XX.X C
  tft.setCursor(x0, y);
  tft.setTextColor(ST77XX_WHITE);
  // Ejemplo: “(Lunes)   TempRTC: 24.5 C”
  tft.printf("(%s) TempRTC: %.1f C",
             daysOfTheWeek[now.dayOfTheWeek()],
             RTCtemp);
  y += lineHeight;

  // Pequeño espacio extra antes de la siguiente sección
  y += 2;

  // —— 2) Presión y altitud ——  
  // LINEA 3: Pres:   XXX.X hPa
  tft.setCursor(x0, y);
  tft.setTextColor(ST77XX_CYAN);
  tft.printf("Pres:  %.1f hPa", pres);
  y += lineHeight;

  // LINEA 4: Alt:    XXX.X m
  tft.setCursor(x0, y);
  tft.setTextColor(ST77XX_CYAN);
  tft.printf("Alt:   %.1f m", alt);
  y += lineHeight;

  // LINEA 5: SeaPres: XXX.X hPa
  tft.setCursor(x0, y);
  tft.setTextColor(ST77XX_CYAN);
  tft.printf("SeaPres: %.1f hPa", seaLevelPres / 100.0f);
  y += lineHeight;

  // LINEA 6: TempBMP: XX.X C
  tft.setCursor(x0, y);
  tft.setTextColor(ST77XX_CYAN);
  tft.printf("TempBMP: %.1f C", BMPtemp);
  y += lineHeight;

  // Espacio extra antes de la siguiente sección
  y += 2;

  // —— 3) Temperatura y humedad DHT ——  
  // LINEA 7: Temp:  XX.X C
  tft.setCursor(x0, y);
  tft.setTextColor(ST77XX_YELLOW);
  tft.printf("Temp:  %.1f C", temp);
  y += lineHeight;

  // LINEA 8: Hum:   XX.X %
  tft.setCursor(x0, y);
  tft.setTextColor(ST77XX_YELLOW);
  tft.printf("Hum:   %.1f %%", hum);
  y += lineHeight;

  // LINEA 9: Feels: XX.X C
  tft.setCursor(x0, y);
  tft.setTextColor(ST77XX_YELLOW);
  tft.printf("Feels: %.1f C", feels_like);
  y += lineHeight;

  // Espacio extra antes de la siguiente sección
  y += 2;

  // —— 4) Luz ——  
  // LINEA 10: Luz:   XXXX.X lx
  tft.setCursor(x0, y);
  tft.setTextColor(ST77XX_GREEN);
  tft.printf("Luz:   %.1f lx", lux);
  y += lineHeight;

  // Espacio extra antes de la siguiente sección
  y += 4;

  // —— 5) Ubicación ——  
  // LINEA 11: Ciudad: XXXXXXXX
  tft.setCursor(x0, y);
  tft.setTextColor(ST77XX_MAGENTA);
  tft.print("Ciudad: ");
  tft.print(city);
  y += lineHeight;

  // LINEA 12: País:   XXXXXX
  tft.setCursor(x0, y);
  tft.setTextColor(ST77XX_MAGENTA);
  tft.print("Pais: ");
  tft.print(country_name);
  y += lineHeight;

  // Espacio extra antes de coord. GPS
  y += 2;

  // LINEA 13: lat: XX.XXXXXX
  tft.setCursor(x0, y);
  tft.setTextColor(ST77XX_MAGENTA);
  tft.printf("lat: %.6f", latitude);
  y += lineHeight;

  // LINEA 14: lon: XX.XXXXXX
  tft.setCursor(x0, y);
  tft.setTextColor(ST77XX_MAGENTA);
  tft.printf("lon: %.6f", longitude);
  // y += lineHeight; // si quisieras más líneas después

  // FIN de la función – todo se muestra sin overflow porque
  // controlamos la posición manualmente y evitamos \n en printf.
}

void printTime() {
  Serial.print(now.day(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.year(), DEC);
  Serial.print(" (");
  Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
  Serial.print(") ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();

  Serial.print("Temperature: ");
  Serial.print(RTCtemp);
  Serial.println(" C");

  Serial.println();
}

void printBMP() {
  Serial.print("Temperature = ");
  Serial.print(BMPtemp);
  Serial.println(" *C");
  
  Serial.print("Pressure = ");
  Serial.print(pres);
  Serial.println(" Pa");
  
  // Calculate altitude assuming 'standard' barometric
  // pressure of 1013.25 millibar = 101325 Pascal
  Serial.print("Altitude = ");
  Serial.print(alt);
  Serial.println(" meters");

  Serial.print("Pressure at sealevel (calculated) = ");
  Serial.print(seaLevelPres); // Convert Pa to hPa
  Serial.println(" Pa");

// you can get a more precise measurement of altitude
// if you know the current sea level pressure which will
// vary with weather and such. If it is 1015 millibars
// that is equal to 101500 Pascals.
  Serial.print("Real altitude = ");
  Serial.print(alt);
  Serial.println(" meters");
  
  Serial.println();
}

void printDHT() {
  Serial.print(F("Humidity: "));
  Serial.print(hum);
  Serial.print(F("%  Temperature: "));
  Serial.print(temp);
  Serial.print(F("°C  Heat index: "));
  Serial.print(feels_like);
  Serial.println(F("°C "));
}

void printBHT() {
  Serial.print("Light: ");
  Serial.print(lux);
  Serial.println(" lx");
  Serial.println();
}

/*
void tftSetup() {
    Serial.println(F("Initialized"));

  uint16_t time = millis();
  tft.fillScreen(ST77XX_BLACK);
  time = millis() - time;

  Serial.println(time, DEC);
  delay(500);

  // large block of text
  tft.fillScreen(ST77XX_BLACK);
  testdrawtext("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Curabitur adipiscing ante sed nibh tincidunt feugiat. Maecenas enim massa, fringilla sed malesuada et, malesuada sit amet turpis. Sed porttitor neque ut ante pretium vitae malesuada nunc bibendum. Nullam aliquet ultrices massa eu hendrerit. Ut sed nisi lorem. In vestibulum purus a tortor imperdiet posuere. ", ST77XX_WHITE);
  delay(1000);

  // tft print function!
  tftPrintTest();
  delay(4000);

  // a single pixel
  tft.drawPixel(tft.width()/2, tft.height()/2, ST77XX_GREEN);
  delay(500);

  // line draw test
  testlines(ST77XX_YELLOW);
  delay(500);

  // optimized lines
  testfastlines(ST77XX_RED, ST77XX_BLUE);
  delay(500);

  testdrawrects(ST77XX_GREEN);
  delay(500);

  testfillrects(ST77XX_YELLOW, ST77XX_MAGENTA);
  delay(500);

  tft.fillScreen(ST77XX_BLACK);
  testfillcircles(10, ST77XX_BLUE);
  testdrawcircles(10, ST77XX_WHITE);
  delay(500);

  testroundrects();
  delay(500);

  testtriangles();
  delay(500);

  mediabuttons();
  delay(500);

  Serial.println("done");
  delay(1000);
}

void testlines(uint16_t color) {
  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawLine(0, 0, x, tft.height()-1, color);
    delay(0);
  }
  for (int16_t y=0; y < tft.height(); y+=6) {
    tft.drawLine(0, 0, tft.width()-1, y, color);
    delay(0);
  }

  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawLine(tft.width()-1, 0, x, tft.height()-1, color);
    delay(0);
  }
  for (int16_t y=0; y < tft.height(); y+=6) {
    tft.drawLine(tft.width()-1, 0, 0, y, color);
    delay(0);
  }

  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawLine(0, tft.height()-1, x, 0, color);
    delay(0);
  }
  for (int16_t y=0; y < tft.height(); y+=6) {
    tft.drawLine(0, tft.height()-1, tft.width()-1, y, color);
    delay(0);
  }

  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawLine(tft.width()-1, tft.height()-1, x, 0, color);
    delay(0);
  }
  for (int16_t y=0; y < tft.height(); y+=6) {
    tft.drawLine(tft.width()-1, tft.height()-1, 0, y, color);
    delay(0);
  }
}

void testdrawtext(char *text, uint16_t color) {
  tft.setCursor(0, 0);
  tft.setTextColor(color);
  tft.setTextWrap(true);
  tft.print(text);
}

void testfastlines(uint16_t color1, uint16_t color2) {
  tft.fillScreen(ST77XX_BLACK);
  for (int16_t y=0; y < tft.height(); y+=5) {
    tft.drawFastHLine(0, y, tft.width(), color1);
  }
  for (int16_t x=0; x < tft.width(); x+=5) {
    tft.drawFastVLine(x, 0, tft.height(), color2);
  }
}

void testdrawrects(uint16_t color) {
  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawRect(tft.width()/2 -x/2, tft.height()/2 -x/2 , x, x, color);
  }
}

void testfillrects(uint16_t color1, uint16_t color2) {
  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x=tft.width()-1; x > 6; x-=6) {
    tft.fillRect(tft.width()/2 -x/2, tft.height()/2 -x/2 , x, x, color1);
    tft.drawRect(tft.width()/2 -x/2, tft.height()/2 -x/2 , x, x, color2);
  }
}

void testfillcircles(uint8_t radius, uint16_t color) {
  for (int16_t x=radius; x < tft.width(); x+=radius*2) {
    for (int16_t y=radius; y < tft.height(); y+=radius*2) {
      tft.fillCircle(x, y, radius, color);
    }
  }
}

void testdrawcircles(uint8_t radius, uint16_t color) {
  for (int16_t x=0; x < tft.width()+radius; x+=radius*2) {
    for (int16_t y=0; y < tft.height()+radius; y+=radius*2) {
      tft.drawCircle(x, y, radius, color);
    }
  }
}

void testtriangles() {
  tft.fillScreen(ST77XX_BLACK);
  uint16_t color = 0xF800;
  int t;
  int w = tft.width()/2;
  int x = tft.height()-1;
  int y = 0;
  int z = tft.width();
  for(t = 0 ; t <= 15; t++) {
    tft.drawTriangle(w, y, y, x, z, x, color);
    x-=4;
    y+=4;
    z-=4;
    color+=100;
  }
}

void testroundrects() {
  tft.fillScreen(ST77XX_BLACK);
  uint16_t color = 100;
  int i;
  int t;
  for(t = 0 ; t <= 4; t+=1) {
    int x = 0;
    int y = 0;
    int w = tft.width()-2;
    int h = tft.height()-2;
    for(i = 0 ; i <= 16; i+=1) {
      tft.drawRoundRect(x, y, w, h, 5, color);
      x+=2;
      y+=3;
      w-=4;
      h-=6;
      color+=1100;
    }
    color+=100;
  }
}

void tftPrintTest() {
  tft.setTextWrap(false);
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(0, 30);
  tft.setTextColor(ST77XX_RED);
  tft.setTextSize(1);
  tft.println("Hello World!");
  tft.setTextColor(ST77XX_YELLOW);
  tft.setTextSize(2);
  tft.println("Hello World!");
  tft.setTextColor(ST77XX_GREEN);
  tft.setTextSize(3);
  tft.println("Hello World!");
  tft.setTextColor(ST77XX_BLUE);
  tft.setTextSize(4);
  tft.print(1234.567);
  delay(1500);
  tft.setCursor(0, 0);
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(0);
  tft.println("Hello World!");
  tft.setTextSize(1);
  tft.setTextColor(ST77XX_GREEN);
  tft.print(p, 6);
  tft.println(" Want pi?");
  tft.println(" ");
  tft.print(8675309, HEX); // print 8,675,309 out in HEX!
  tft.println(" Print HEX!");
  tft.println(" ");
  tft.setTextColor(ST77XX_WHITE);
  tft.println("Sketch has been");
  tft.println("running for: ");
  tft.setTextColor(ST77XX_MAGENTA);
  tft.print(millis() / 1000);
  tft.setTextColor(ST77XX_WHITE);
  tft.print(" seconds.");
}

void mediabuttons() {
  // play
  tft.fillScreen(ST77XX_BLACK);
  tft.fillRoundRect(25, 10, 78, 60, 8, ST77XX_WHITE);
  tft.fillTriangle(42, 20, 42, 60, 90, 40, ST77XX_RED);
  delay(500);
  // pause
  tft.fillRoundRect(25, 90, 78, 60, 8, ST77XX_WHITE);
  tft.fillRoundRect(39, 98, 20, 45, 5, ST77XX_GREEN);
  tft.fillRoundRect(69, 98, 20, 45, 5, ST77XX_GREEN);
  delay(500);
  // play color
  tft.fillTriangle(42, 20, 42, 60, 90, 40, ST77XX_BLUE);
  delay(50);
  // pause color
  tft.fillRoundRect(39, 98, 20, 45, 5, ST77XX_RED);
  tft.fillRoundRect(69, 98, 20, 45, 5, ST77XX_RED);
  // play color
  tft.fillTriangle(42, 20, 42, 60, 90, 40, ST77XX_GREEN);
}
 */