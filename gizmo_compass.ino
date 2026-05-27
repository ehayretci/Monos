#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <math.h>
#include <ESP32Servo.h>

Servo myServo;

// Wi-Fi credentials — update before flashing
const char* ssid = "YourNetworkName";
const char* password = "YourPassword";

const int servoPin = 23;
const int redLed = 2;
const int greenLed = 4;
const int buzzerPin = 6;

float targetlat = 0.0, targetlon = 0.0;
float currentlat = 0.0, currentlon = 0.0;
float previouslat = 0.0, previouslon = 0.0;
bool firstUpdate = true;

AsyncWebServer server(80);

double calculateAngle(double x1, double y1, double x2, double y2) {
  double dotProduct = x1 * x2 + y1 * y2;
  double magnitude1 = sqrt(x1 * x1 + y1 * y1);
  double magnitude2 = sqrt(x2 * x2 + y2 * y2);
  double angleRad = acos(dotProduct / (magnitude1 * magnitude2));
  return angleRad * (180.0 / M_PI);
}

void setup() {
  Serial.begin(115200);
  pinMode(servoPin, OUTPUT);
  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  myServo.attach(servoPin);
  myServo.write(0);
  digitalWrite(redLed, LOW);
  digitalWrite(greenLed, LOW);
  digitalWrite(buzzerPin, LOW);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP());

  server.on("/targetlocation", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam("latitude") && request->hasParam("longitude")) {
      targetlat = request->getParam("latitude")->value().toFloat();
      targetlon = request->getParam("longitude")->value().toFloat();
    }
    request->send(200, "text/plain", "Location received");
  });

  server.on("/currentlocation", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam("latitude") && request->hasParam("longitude")) {
      previouslat = currentlat;
      previouslon = currentlon;
      currentlat = request->getParam("latitude")->value().toFloat();
      currentlon = request->getParam("longitude")->value().toFloat();
      if (!firstUpdate) {
        double vectorX = targetlon - currentlon;
        double vectorY = targetlat - currentlat;
        double vectorXprev = currentlon - previouslon;
        double vectorYprev = currentlat - previouslat;
        double angle = calculateAngle(vectorX, vectorY, vectorXprev, vectorYprev);
        Serial.printf("Angle: %.2f degrees\n", angle);
      } else {
        firstUpdate = false;
      }
    }
    request->send(200, "text/plain", "Location updated");
  });

  server.begin();
}

void loop() {}