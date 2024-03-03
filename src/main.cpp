#include <Arduino.h>

// ########## WASTE TANK
// #define FLOAT_SENSOR 2  // the number of the pushbutton pin
// #define LED 8           // the number of the LED pin

// void setup() {
//   // initialize the LED pin as an output:
//   pinMode(LED, OUTPUT);
//   // initialize the pushbutton pin as an input:
//   pinMode(FLOAT_SENSOR, INPUT_PULLUP);
// }

// void loop() {
//   if (digitalRead(FLOAT_SENSOR) == LOW) {
//     // turn LED on:
//     digitalWrite(LED, HIGH);
//   } else {
//     // turn LED off:
//     digitalWrite(LED, LOW);
//   }
// }

int analogPin = 0;
int raw = 0;

void setup() { Serial.begin(9600); }

void loop() {
  raw = analogRead(analogPin);

  if (raw) {
    Serial.print("Raw: ");
    Serial.println(raw);

    delay(1000);
  }
}