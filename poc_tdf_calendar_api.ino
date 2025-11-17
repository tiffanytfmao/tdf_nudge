#include <WiFi.h>
#include <secrets.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>
#include <Adafruit_NeoPixel.h>

// WiFi credentials
const char* ssid = SECRET_SSID;
const char* password = SECRET_PASSWORD;

String calendar_url = SECRET_CALENDAR_URL;

#define PIN_NEOPIXEL        0
#define NEOPIXEL_I2C_POWER  2
#define NUMPIXELS           1

Adafruit_NeoPixel pixel(NUMPIXELS, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(NEOPIXEL_I2C_POWER, OUTPUT);
  digitalWrite(NEOPIXEL_I2C_POWER, HIGH);
  pixel.begin();
  pixel.clear();
  pixel.show();

  connectToWiFi();
}


void loop() {
  fetchCalendarData();
  delay(60000);  
}

void connectToWiFi() {
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi...");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
}

void fetchCalendarData() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi lost. Reconnecting...");
    WiFi.reconnect();
    return;
  }

  HTTPClient http;
  http.begin(calendar_url);
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);

  int httpCode = http.GET();
  Serial.print("HTTP code: ");
  Serial.println(httpCode);

  if (httpCode == 200) {
    String payload = http.getString();
    Serial.println("Raw JSON:");
    Serial.println(payload);

    JSONVar events = JSON.parse(payload);
    if (JSON.typeof(events) == "undefined") {
      Serial.println("⚠️ JSON Parsing failed");
      return;
    }

    int numEvents = events.length();
    Serial.print("📅 Events found: ");
    Serial.println(numEvents);

    for (int i = 0; i < numEvents; i++) {
      Serial.print("Event ");
      Serial.println(i + 1);
      Serial.print("Title: ");
      Serial.println((const char*)events[i]["title"]);
      Serial.print("Start: ");
      Serial.println((const char*)events[i]["start"]);
      Serial.print("End: ");
      Serial.println((const char*)events[i]["end"]);
      Serial.println("----------------");
    }

    // Current logic: blink LED if events exist
    if (numEvents > 0) {
      blinkNeoPixel(255, 0, 255); // purple
    } else {
      pixel.clear();
      pixel.show();
    }
  }

  http.end();
}

void blinkNeoPixel(int r, int g, int b) {
  pixel.setPixelColor(0, pixel.Color(r, g, b));
  pixel.show();
  delay(300);
  pixel.clear();
  pixel.show();
  delay(300);
}
