#include <Arduino.h>

// FRESH WATER TANK
int analogPin = 0;
int raw = 0;
int value = 0;
int linear = 0;

// WASTE TANK
#define FLOAT_SENSOR 2  // the number of the pushbutton pin
#define LED 8           // the number of the LED pin

void setup() {
  // initialize the LED pin as an output:
  pinMode(LED, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(FLOAT_SENSOR, INPUT_PULLUP);
  Serial.begin(9600);
}

void loop() {
  raw = analogRead(analogPin);

  // waste tank
  if (digitalRead(FLOAT_SENSOR) == LOW) {
    // turn LED on:
    digitalWrite(LED, HIGH);
    Serial.println("ok");
  } else {
    // turn LED off:
    digitalWrite(LED, LOW);
    Serial.println("FULL");
  }

  // // fresh water
  if (raw) {
    Serial.print("Raw: ");
    Serial.println(raw);
    if (raw < 20) {
      Serial.println("Empty");
    }
    if (20 <= raw && raw <= 350) {
      Serial.println("1");
    }
    if (350 < raw && raw <= 553) {
      Serial.println("2");
    }

    if (553 < raw && raw <= 610) {
      Serial.println("3");
    }

    if (610 < raw && raw <= 680) {
      Serial.println("4");
    }

    delay(1000);
  }
