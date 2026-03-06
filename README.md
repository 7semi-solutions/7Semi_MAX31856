# 7Semi MAX31856 Arduino Library

Arduino driver for the MAX31856 thermocouple-to-digital converter.

The MAX31856 is a precision thermocouple interface that supports multiple thermocouple types and provides high-accuracy temperature measurement, cold junction compensation, and fault detection.


## Features

- Supports all MAX31856 thermocouple types

   - B, E, J, K, N, R, S, T

- Thermocouple temperature measurement

- Cold junction temperature reading

- Cold junction compensation

- Fault detection and status reporting

- Thermocouple temperature limit configuration

- Cold junction temperature limit configuration

- Fault mask configuration

- Continuous and one-shot conversion modes

- Noise filter configuration (50Hz / 60Hz)

- Cold junction offset calibration

---

## Connection
The MAX31856 communicates using SPI.
### SPI Connection
| MAX31856 Pin | MCU Pin   | Notes           |
| ------------ | --------- | --------------- |
| VIN          | 3.3V / 5V | Sensor power    |
| GND          | GND       | Common ground   |
| SCK          | SCK       | SPI clock       |
| SDO          | MISO      | SPI data output |
| SDI          | MOSI      | SPI data input  |
| CS           | GPIO      | Chip select     |

### SPI Notes

- Supported SPI speeds depend on the microcontroller.

- Recommended SPI clock:

  - 1 MHz – 5 MHz 

---

## Installation
- Arduino Library Manager

  1. Open Arduino IDE

  2. Go to Library Manager

  3. Search for 7Semi TMAG5273

  4. Click Install

- Manual Installation

  1. Download this repository as ZIP

  2. Arduino IDE → Sketch → Include Library → Add .ZIP Library

---

## Library Overview

## Reading Thermocouple Temperature
```cpp
float temperature;

temperature = thermo.readThermocoupleTemperature();

Serial.print("Temperature: ");
Serial.println(temperature);
```

Returns magnetic field strength in milliTesla (mT).

## Reading Cold Junction Temperature
```cpp
float cj;

cj = thermo.readColdJunctionTemperature();

Serial.print("Cold Junction: ");
Serial.println(cj);
```
Returns cold junction reference temperature in °C.

## Thermocouple Configuration

The TMAG5273 can be used for magnetic rotary encoders.
```cpp
thermo.setThermocoupleType(TC_K);
```
Available types:

TC_B

TC_E

TC_J

TC_K

TC_N

TC_R

TC_S

TC_T

## Fault Detection

The MAX31856 can detect several sensor and wiring faults.

Example:
```cpp
uint8_t fault = thermo.readFaultStatus();

if(fault)
{
  Serial.println("Sensor fault detected");
}
```

Typical fault conditions include:

- Thermocouple open circuit

- Thermocouple temperature high limit

- Thermocouple temperature low limit

- Cold junction high limit

- Cold junction low limit

- Over/under voltage


## Temperature Limit Configuration

Temperature limits can trigger fault events when exceeded.

Example:
```cpp

thermo.setThermocoupleHighC(100);
thermo.setThermocoupleLowC(10);
```

Cold junction limits:
```cpp
thermo.setColdJunctionHighC(40);
thermo.setColdJunctionLowC(0);
```
## Conversion Modes

The sensor supports different measurement modes.

### Continuous Mode

Sensor continuously measures temperature.

```cpp
thermo.setContinuousMode(true);
```

### One-Shot Conversion

Single temperature measurement when triggered.

```cpp
thermo.setOneShot(true);
```
## Noise Filter

Configure mains interference filtering.

Example:
```cpp
thermo.setNoiseFilter(FILTER_50HZ);
```
Options:

- FILTER_50HZ

- FILTER_60HZ

## Cold Junction Offset Calibration

Adjust cold junction compensation.
Example:
```cpp
thermo.setColdJunctionOffsetC(2.0);
```
---
## Example Applications

### Applications:

- Industrial furnace monitoring

- Temperature logging systems

- Embedded thermal controllers

- HVAC temperature sensing

- Laboratory instrumentation
