#define TOUCH_PIN 4
#define LED_PIN 2
#define VIB_PIN 23

int count = 0;
bool trigger = false;

void setup() {
  pinMode(TOUCH_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
}

void loop() {
  if (digitalRead(TOUCH_PIN) == HIGH) {
    trigger = true;
    count = 0;
  }
  if (trigger) {
    if (count < 5) {
      digitalWrite(LED_PIN, HIGH);
      digitalWrite(23, HIGH);
      delay(500);
      digitalWrite(LED_PIN, LOW);
      digitalWrite(23, LOW);
      delay(500);
      count += 1;
    } else {
      trigger = false;
    }
  }
}