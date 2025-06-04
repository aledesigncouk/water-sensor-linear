/*
Alex Deidda 2025
alex.deidda@aledesign.co.uk
yoroxid.com

WATER GAUGE v2.1.0
*/

#include <U8g2lib.h>
#include <Arduino.h>
#include <SoftwareSerial.h>

// Pins
#define DISPLAY_BUTTON 4
#define BT_BUTTON 6
#define FRESH_WATER_SENSOR A0
#define WASTE_SENSOR 5
#define BT_LED 8
#define WASTE_LED 9

// OLED 128x128 using SH1107 over software I2C (Pro Micro: SDA=2, SCL=3)
U8G2_SH1107_128X128_2_HW_I2C u8g2(U8G2_R0, 3, 2, U8X8_PIN_NONE);

// Constants
const int SENSOR_MAX = 204;
const int BAR_HEIGHT_MAX = 100;
const unsigned long DEBOUNCE_DELAY = 100;

// States
bool u8g2State = true;
bool bluetoothState = true;

unsigned long lastDebounceu8g2 = 0;
unsigned long lastDebounceBT = 0;

// SoftwareSerial
SoftwareSerial BTserial(0, 1);

// =================== FUNCTIONS ===================

int getWaterLevelPercentage(float sensorReading) {
  float a = 10.96257 - (0.005476351 * sensorReading);
  float b = 0.0001947447 * sensorReading * sensorReading;
  return (int)(a + b);
}

void setup() {
  pinMode(WASTE_LED, OUTPUT);
  pinMode(WASTE_SENSOR, INPUT_PULLUP);
  pinMode(DISPLAY_BUTTON, INPUT_PULLUP);
  pinMode(BT_BUTTON, INPUT_PULLUP);

  Serial.begin(9600);
  u8g2.begin();
  BTserial.begin(9600);
}

void handleButtons() {
  // u8g2 toggle
  if (digitalRead(DISPLAY_BUTTON) == LOW) {
    if (millis() - lastDebounceu8g2 > DEBOUNCE_DELAY) {
      u8g2State = !u8g2State;
      Serial.println(u8g2State ? "u8g2 ON" : "u8g2 OFF");

      u8g2.setPowerSave(u8g2State ? 0 : 1);
      lastDebounceu8g2 = millis();
    }
    while (digitalRead(DISPLAY_BUTTON) == LOW)
      ; // wait for release
  }

  // BLUETOOTH toggle
  if (digitalRead(BT_BUTTON) == LOW) {
    if (millis() - lastDebounceBT > DEBOUNCE_DELAY) {
      bluetoothState = !bluetoothState;
      Serial.println(bluetoothState ? "Bluetooth ON" : "Bluetooth OFF");

      if (bluetoothState) {
        BTserial.begin(9600);
      } else {
        BTserial.end();
      }
      lastDebounceBT = millis();
    }
    while (digitalRead(BT_BUTTON) == LOW)
      ; // wait for release
  }
}

void updateu8g2(int levelPercent, int barHeight, bool wasteEmpty) {
  if (!u8g2State)
    return;

  u8g2.firstPage();
  do {
    // Layout configuration
    const int barY = 10;
    const int barHeight = 103;

    const int scaleX = 10;

    const int barX = 45;
    const int barWidth = 80;

    // Draw scale ticks and labels
    u8g2.setFont(u8g2_font_5x7_tr);
    for (int i = 0; i <= 100; i += 10) {
      int y = barY + barHeight - (i * barHeight / 100);
      int tickLen = (i % 20 == 0) ? 6 : 3;

      u8g2.drawLine(scaleX, y, scaleX + tickLen, y);

      if (i % 20 == 0) {
        char label[5];
        sprintf(label, "%d", i);
        u8g2.drawStr(scaleX + tickLen + 2, y + 3, label);
      }
    }

    // Draw water bar frame
    u8g2.drawFrame(barX, barY, barWidth, barHeight);

    // Fill water level with stylized dots
    int fillHeight = (barHeight * levelPercent) / 100;
    for (int y = barY + barHeight - fillHeight + 2; y < barY + barHeight - 2; y += 4) {
      for (int x = barX + 2; x < barX + barWidth - 2; x += 6) {
        u8g2.drawPixel(x, y);
        u8g2.drawPixel(x + 1, y + 1);
        u8g2.drawPixel(x + 2, y);
      }
    }

    // Waste tank status
     u8g2.setFont(u8g2_font_6x10_tr);
    const char* wasteStatus = wasteEmpty ? "Waste: OK" : "Waste: FULL";

    int textWidth = u8g2.getStrWidth(wasteStatus);
    int textX = barX + (barWidth - textWidth) / 2;

    if (!wasteEmpty) {
      int boxWidth = textWidth + 4;
      int boxX = barX + (barWidth - boxWidth) / 2;
      u8g2.drawBox(boxX, 117, boxWidth, 10);
      u8g2.setDrawColor(0);
      u8g2.drawStr(boxX + 2, 125, wasteStatus);
      u8g2.setDrawColor(1);
    } else {
      u8g2.drawStr(textX, 125, wasteStatus);
    }

  } while (u8g2.nextPage());
}

void updateBluetooth(int levelPercent, bool wasteEmpty) {
  if (!bluetoothState)
    return;

  BTserial.print("Level: ");
  BTserial.println(levelPercent);

  BTserial.print("u8g2: ");
  BTserial.println(u8g2State ? "ON" : "OFF");

  BTserial.print("Waste: ");
  BTserial.println(wasteEmpty ? "EMPTY" : "FULL");
}

void updateWasteLED(bool wasteEmpty) {
  digitalWrite(WASTE_LED, wasteEmpty ? HIGH : LOW);
  Serial.print("Waste: ");
  Serial.println(wasteEmpty ? "EMPTY" : "FULL");
}

void updateBTLed(bool bluetoothState) {
  digitalWrite(BT_LED, bluetoothState ? HIGH : LOW);
  Serial.print("BT: ");
  Serial.println(bluetoothState ? "ON" : "OFF");
}

// =================== MAIN LOOP ===================

void loop() {
  int rawReading = analogRead(FRESH_WATER_SENSOR);
  int levelPercent = getWaterLevelPercentage(rawReading);
  int levelHeight = map(levelPercent, 0, SENSOR_MAX, 0, BAR_HEIGHT_MAX);
  bool wasteEmpty = digitalRead(WASTE_SENSOR) == LOW;

  handleButtons();
  updateu8g2(levelPercent, levelHeight, wasteEmpty);
  updateBluetooth(levelPercent, wasteEmpty);
  updateWasteLED(wasteEmpty);
  updateBTLed(bluetoothState);

  Serial.print("Level %: ");
  Serial.println(levelPercent);

  delay(500);
}
