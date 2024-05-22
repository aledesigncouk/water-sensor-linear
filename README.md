# Arduino Water Gauge

A simple digital gauge to monitor the water level of two tanks:

- Fresh water (linear level)
- Waste water (Full tank indicator)

## Description

...

### Hardware

- Sparkfun Pro Micro (or any compatible Arduino Board)
- Bluetooth module HC-06
- I2C Oled Display, 128x56
- led diode
- 2 x 330k resistor
- 1 x 100k resistor (depending on sensor range)
- jumper wires
- prototyping breadboard
- Float switch
- Fuel/Water floating sensor

### Software

The project is built with Platformio (https://platformio.org/)
If you are using the Arduino IDE, just copy the main.cpp file, rename it as .ino and import the required libraries.

### Schematics

| HC-06 | Arduino |
| :---- | :------ |
| `Rx`  | `Tx`    |
| `Tx`  | `Rx`    |
| `Vcc` | `Vcc`   |
| `Gnd` | `Gnd`   |

| I2C Oled  | Arduino (Sparkfun Pro Micro) |
| :-------- | :--------------------------- |
| `Sck`     | `3 (Scl)`                    |
| `Sda`     | `2 (Sda)`                    |
| `Vcc/Vdd` | `Vcc`                        |
| `Gnd`     | `Gnd`                        |

![alt text](./imgs/schematics.png)

### Pro Micro pinout reference

![alt text](./imgs/pinout.png)
