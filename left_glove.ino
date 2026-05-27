// Import required libraries
#include <WiFi.h>
#include <ESPAsyncWebServer.h>

// Wi-Fi credentials — update before flashing
const char* ssid = "YourNetworkName";
const char* password = "YourPassword";

// Pin Definitions
#define TOUCH_PIN 4  // Touch sensor pin
#define LED_PIN 2    // LED strip pin
#define VIB_PIN 23   // Vibration motor pin

int count = 0;
bool trigger = false;

AsyncWebServer server(80);

void setup() {
  Serial.begin(115200);
  pinMode(TOUCH_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(VIB_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  digitalWrite(VIB_PIN, LOW);

  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Still trying to connect...");
  }
  Serial.println("Connected to WiFi!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.on("/left", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(VIB_PIN, HIGH);
    delay(1000);
    digitalWrite(VIB_PIN, LOW);
    request->send(204);
  });

  server.on("/arrive", HTTP_GET, [](AsyncWebServerRequest *request){
    for (int x = 0; x < 3; x++) {
      digitalWrite(VIB_PIN, HIGH); delay(300);
      digitalWrite(VIB_PIN, LOW); delay(300);
    }
    digitalWrite(VIB_PIN, HIGH); delay(1000);
    digitalWrite(VIB_PIN, LOW);
    request->send(204);
  });

  server.begin();
}

void loop() {
  if (digitalRead(TOUCH_PIN) == HIGH) {
    trigger = true;
    count = 0;
  }
  if (trigger) {
    if (count < 5) {
      digitalWrite(LED_PIN, HIGH); delay(500);
      digitalWrite(LED_PIN, LOW); delay(500);
      count += 1;
    } else {
      trigger = false;
    }
  }
}