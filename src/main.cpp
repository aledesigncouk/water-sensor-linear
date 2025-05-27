#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <SoftwareSerial.h>

// Pins
#define DISPLAY_BUTTON 4
#define BT_BUTTON 6
#define FRESH_WATER_SENSOR A0
#define WASTE_SENSOR 5
#define WASTE_LED 8

// OLED
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(128, 64, &Wire, -1);

// Constants
const int SENSOR_MAX = 204;
const int BAR_HEIGHT_MAX = 76;
const unsigned long DEBOUNCE_DELAY = 100;

// States
bool displayState = true;
bool bluetoothState = true;

unsigned long lastDebounceDisplay = 0;
unsigned long lastDebounceBT = 0;

// SoftwareSerial
SoftwareSerial BTserial(0,1);

// =================== FUNCTIONS ===================

int getWaterLevelPercentage(float sensorReading) {
  float a = 10.96257 - (0.005476351 * sensorReading);
  float b = 0.0001947447 * sensorReading * sensorReading;
  return (int)(a + b);
}

void setupDisplay() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (true);
  }
  display.clearDisplay();
  display.display();
}

void setup() {
  pinMode(WASTE_LED, OUTPUT);
  pinMode(WASTE_SENSOR, INPUT_PULLUP);
  pinMode(DISPLAY_BUTTON, INPUT_PULLUP);
  pinMode(BT_BUTTON, INPUT_PULLUP);

  Serial.begin(9600);
  BTserial.begin(9600);

  setupDisplay();
}

void handleButtons() {
  // DISPLAY toggle
  if (digitalRead(DISPLAY_BUTTON) == LOW) {
    if (millis() - lastDebounceDisplay > DEBOUNCE_DELAY) {
      displayState = !displayState;
      Serial.println(displayState ? "Display ON" : "Display OFF");

      display.ssd1306_command(displayState ? SSD1306_DISPLAYON : SSD1306_DISPLAYOFF);
      lastDebounceDisplay = millis();
    }
    while (digitalRead(DISPLAY_BUTTON) == LOW); // wait for release
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
    while (digitalRead(BT_BUTTON) == LOW); // wait for release
  }
}

void updateDisplay(int levelPercent, int barHeight, bool wasteEmpty) {
  if (!displayState) return;

  display.setRotation(3);
  display.clearDisplay();
  display.setTextColor(WHITE, BLACK);

  display.setCursor(4, 3);
  display.println("LV");

  display.setCursor(3, 104);
  display.println("waste tank");

  display.setCursor(20, 3);
  display.println(levelPercent);
  display.setCursor(50, 3);
  display.println("%");

  display.fillRect(3, 19, 56, barHeight, WHITE);
  display.drawRect(1, 17, 60, 80, WHITE); // tank outline

  if (wasteEmpty) {
    display.drawRect(1, 114, 60, 13, WHITE);
    display.setCursor(28, 117);
  } else {
    display.fillRect(1, 114, 60, 13, WHITE);
    display.setCursor(22, 117);
    display.setTextColor(BLACK, WHITE);
  }

  display.println(wasteEmpty ? "EMPTY" : "FULL");

  display.display();
}

void updateBluetooth(int levelPercent, bool wasteEmpty) {
  if (!bluetoothState) return;

  BTserial.print("Level: ");
  BTserial.println(levelPercent);
  
  BTserial.print("Display: ");
  BTserial.println(displayState ? "ON" : "OFF");

  BTserial.print("Waste: ");
  BTserial.println(wasteEmpty ? "EMPTY" : "FULL");
}

void updateWasteLED(bool wasteEmpty) {
  digitalWrite(WASTE_LED, wasteEmpty ? HIGH : LOW);
  Serial.print("Waste: ");
  Serial.println(wasteEmpty ? "EMPTY" : "FULL");
}

// =================== MAIN LOOP ===================

void loop() {
  int rawReading = analogRead(FRESH_WATER_SENSOR);
  int levelPercent = getWaterLevelPercentage(rawReading);
  int levelHeight = map(levelPercent, 0, SENSOR_MAX, 0, BAR_HEIGHT_MAX);
  bool wasteEmpty = digitalRead(WASTE_SENSOR) == LOW;

  handleButtons();
  updateDisplay(levelPercent, levelHeight, wasteEmpty);
  updateBluetooth(levelPercent, wasteEmpty);
  updateWasteLED(wasteEmpty);

  Serial.print("Level %: ");
  Serial.println(levelPercent);

  delay(500);
}
