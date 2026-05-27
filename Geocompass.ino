#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <math.h>
#include <ESP32Servo.h>

Servo myServo;

// Wi-Fi credentials — update before flashing
const char* ssid = "YourNetworkName";
const char* password = "YourPassword";

const int servoPin = 18;
const int redPin = 25;
const int greenPin = 26;

float targetlat = 0.0, targetlon = 0.0;
float currentlat = 0.0, currentlon = 0.0;
float previouslat = 0.0, previouslon = 0.0;

bool firstUpdate = true;
double angle = 0.0;
double servoAngle = 90;

AsyncWebServer server(80);

double calculateAngle(double x1, double y1, double x2, double y2) {
  double dotProduct = x1 * x2 + y1 * y2;
  double magnitude1 = sqrt(x1 * x1 + y1 * y1);
  double magnitude2 = sqrt(x2 * x2 + y2 * y2);
  double angleRad = acos(dotProduct / (magnitude1 * magnitude2));
  return angleRad * (180.0 / M_PI);
}

double roundToDecimals(double value, int decimals) {
  double scale = pow(10, decimals);
  return round(value * scale) / scale;
}

double mapToServoAngle(double vectorAngle) {
  double servoAngle = 90;
  if (vectorAngle >= -90 && vectorAngle <= 90) {
    servoAngle = map(vectorAngle, -90, 90, 180, 0);
  } else if (vectorAngle > 90 && vectorAngle <= 180) {
    servoAngle = 270 - vectorAngle;
  } else if (vectorAngle < -90 && vectorAngle >= -180) {
    servoAngle = map(vectorAngle, -90, -180, 90, 180) - 90;
  }
  return constrain(servoAngle, 0, 180);
}

void setup() {
  Serial.begin(115200);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  digitalWrite(redPin, LOW);
  digitalWrite(greenPin, LOW);
  myServo.attach(servoPin);
  myServo.write(90);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP());

  server.on("/targetlocation", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam("latitude") && request->hasParam("longitude")) {
      targetlat = roundToDecimals(request->getParam("latitude")->value().toFloat(), 5);
      targetlon = roundToDecimals(request->getParam("longitude")->value().toFloat(), 5);
      Serial.printf("Target: %.5f, %.5f\n", targetlat, targetlon);
    }
    request->send(200, "text/plain", "Location received");
  });

  server.on("/currentlocation", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam("latitude") && request->hasParam("longitude")) {
      previouslat = roundToDecimals(currentlat, 5);
      previouslon = roundToDecimals(currentlon, 5);
      currentlat = roundToDecimals(request->getParam("latitude")->value().toFloat(), 5);
      currentlon = roundToDecimals(request->getParam("longitude")->value().toFloat(), 5);
      if (!firstUpdate) {
        double vectorX = roundToDecimals(targetlon - currentlon, 5);
        double vectorY = roundToDecimals(targetlat - currentlat, 5);
        double vectorXprev = roundToDecimals(currentlon - previouslon, 5);
        double vectorYprev = roundToDecimals(currentlat - previouslat, 5);
        angle = calculateAngle(vectorX, vectorY, vectorXprev, vectorYprev);
        double crossProduct = vectorX * vectorYprev - vectorY * vectorXprev;
        if (crossProduct < 0) angle = -angle;
        angle = roundToDecimals(angle, 5);
        if (!isnan(angle)) servoAngle = mapToServoAngle(angle);
      } else {
        firstUpdate = false;
      }
      request->send(200, "text/plain", "Location updated");
    } else {
      request->send(400, "text/plain", "Missing parameters");
    }
  });
  server.begin();
}

void loop() {
  myServo.write(servoAngle);
  if (abs(angle) < 90) {
    digitalWrite(greenPin, HIGH);
    digitalWrite(redPin, LOW);
  } else {
    digitalWrite(greenPin, LOW);
    digitalWrite(redPin, HIGH);
  }
  delay(50);
}