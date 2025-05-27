#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <SoftwareSerial.h>

// y = 10.96257 - 0.005476351*x + 0.0001947447*x^2
// x = resistance
// y = water level (%)

// resistor = 100ohm
// sensor = 0 - 190ohm

// MONITOR STATE
#define DISPLAY_BUTTON 4
#define BT_BUTTON 6
bool displayState = true;
bool bluetoothState = true;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 100;

// Declaration for SSD1306 display connected using I2C
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(
    128, 64, &Wire,
    -1); // (width, height, wire, reset pin (-1 share Arduino reset))

SoftwareSerial BTserial(0, 1); // RX | TX

// FRESH WATER TANK
int analogPin = 0; // fresh water sensor PIN
int raw = 0;       // impedance difference reading
const int FRESH_WATER_MAX = 204;
const int BAR_HEIGHT_MAX = 76;

// WASTE TANK
#define WASTE_SENSOR 5 // waste tank full pin
#define LED 8          // waste tank led pin

int getPercentage(float sensorReading) {
  float a = 10.96257 - (0.005476351 * sensorReading);
  float b = 0.0001947447 * sensorReading * sensorReading;

  return (a + b);
}

void setup() {
  pinMode(LED, OUTPUT);
  pinMode(WASTE_SENSOR, INPUT_PULLUP);
  pinMode(DISPLAY_BUTTON, INPUT_PULLUP);
  pinMode(BT_BUTTON, INPUT_PULLUP);

  Serial.begin(9600);
  BTserial.begin(9600);

  // oled init
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.clearDisplay();
}

void loop() {
  raw = analogRead(analogPin);

  int displayButtonState = digitalRead(DISPLAY_BUTTON);
  int btButtonState = digitalRead(BT_BUTTON);
  int level = map(getPercentage(raw), 0, FRESH_WATER_MAX, 0, BAR_HEIGHT_MAX);

  if (displayButtonState == LOW) {
    if (millis() - lastDebounceTime > debounceDelay) {
      displayState = !displayState;

      if (displayState) {
        display.ssd1306_command(SSD1306_DISPLAYON);
        Serial.println("Display ON");
      } else {
        display.ssd1306_command(SSD1306_DISPLAYOFF);
        Serial.println("Display OFF");
      }

      lastDebounceTime = millis();
    }

    while (digitalRead(DISPLAY_BUTTON) == LOW);
    delay(debounceDelay);
  }

  if (btButtonState == LOW) {
    delay(debounceDelay);
    bluetoothState = !bluetoothState;

    if (bluetoothState) {
        BTserial.begin(9600);
        Serial.println("Bluetooth ON");
      } else {
        BTserial.end();
        Serial.println("Bluetooth OFF");
    }
    
    while (digitalRead(BT_BUTTON) == LOW);
  }

  if (displayState) {
    // markers
    display.setRotation(3);
    display.setTextColor(WHITE, BLACK);
    display.setCursor(4, 3);
    display.println("LV");

    display.setCursor(3, 104);
    display.println("waste tank");

    // fresh water tank
    display.setCursor(20, 3);
    display.setTextColor(WHITE, BLACK);
    display.println(getPercentage(raw));
    display.setCursor(50, 3);
    display.println("%");

    display.fillRect(3, 19, 56, level, WHITE); // variable bar

    if (digitalRead(WASTE_SENSOR) == LOW) {
      display.drawRect(1, 114, 60, 13, WHITE);
      display.setCursor(28, 117);
    } else {
      display.fillRect(1, 114, 60, 13, WHITE);
      display.setCursor(22, 117);
      display.setTextColor(BLACK, WHITE);
    }

    display.drawRect(1, 17, 60, 80, WHITE); // container box

    display.display();
    display.clearDisplay();
  }


  if (bluetoothState) {
    BTserial.print("Level: ");
    BTserial.println(getPercentage(raw));
  
    BTserial.print("Display: ");

    if (displayState) {
      BTserial.println("ON");
    } else {
      BTserial.println("OFF");
    }
    
    // waste tank
    if (digitalRead(WASTE_SENSOR) == LOW) {
      BTserial.println("Waste: EMPTY");
    } else {
      BTserial.println("Waste: FULL");
    }
  }

  // waste tank
  if (digitalRead(WASTE_SENSOR) == LOW) {
    digitalWrite(LED, LOW); // turn LED on
    Serial.println("Waste: EMPTY");
  } else {
    digitalWrite(LED, HIGH); // turn LED off
    Serial.println("Waste: FULL");
  }

  Serial.println(level);
  delay(500);
}
