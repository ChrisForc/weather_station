#include "wifi_utils.h" 

static HTTPClient http;
static JsonDocument doc;

String payload = "";
String country_name = "";
String city = "";
float latitude = 0.0f;
float longitude = 0.0f;
int thingspeak_http_code = 0;
WiFiClient wifi_client;

void connectWifi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  // Ahora esperamos a que cambie el estado a WL_CONNECTED:
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println(" connected!");
}

void getGeoLocation(){
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Getting geolocation...");

    http.begin("https://ipapi.co/json/");

    if (http.GET() > 0) {
      payload = http.getString();
      http.end();
      deserializeJson(doc, payload);

      const char* cn = doc["country_name"].as<const char*>();   Serial.println(cn);
      const char* ct = doc["city"].as<const char*>();           Serial.println(ct);
      latitude = doc["latitude"].as<double>();                  Serial.println(latitude);
      longitude = doc["longitude"].as<double>();                Serial.println(longitude);

      country_name = String(cn);
      city = String(ct);
      }
  } else {
    Serial.println("WiFi not connected, cannot get geolocation.");
  }
}

void get1HourForecast() {
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Getting 1-hour forecast...");
    
    http.begin("");
  }
  else {
    Serial.println("WiFi not connected, cannot get 1-hour forecast.");
  }
    
}

void sendToThingSpeak(float fields[]) {
  if (WiFi.status() == WL_CONNECTED) {
    for (int i = 0; i < 7; i++) {
      ThingSpeak.setField(i+1, fields[i]);
    }
    thingspeak_http_code = ThingSpeak.writeFields(THINGSPEAK_CHANNEL_ID, THINGSPEAK_WRITE_APIKEY);
    if(thingspeak_http_code == 200){
      Serial.println("Channel updates successfuly sent to ThingSpeak.");
    }
    else {
      Serial.print("Failed to send data to ThingSpeak, HTTP error code: ");
      Serial.println(thingspeak_http_code);
    }
  }    
  
  else {
    Serial.println("WiFi not connected, cannot send data to ThingSpeak.");
  }
}

bool startThingSpeak() {
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Starting ThingSpeak...");
    if (!ThingSpeak.begin(wifi_client)) {
      Serial.println("Failed to initialize ThingSpeak.");
    } else {
      Serial.println("ThingSpeak initialized successfully.");
    }
    return true;
  }
  else {
    Serial.println("WiFi not connected, cannot start ThingSpeak.");
    return false;
  }
}