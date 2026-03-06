/**
* MAX31856 Fault Detection Configuration Example
*
* * Demonstrates how to configure temperature fault limits
* * Reads thermocouple and cold junction temperatures
* * Prints fault status if any error occurs
*
* Pin Connections
*
* MAX31856    MCU
* ---
* VIN      -> 3.3V / 5V
* GND      -> GND
* SCK      -> SCK
* SDO     -> MISO
* SDI     -> MOSI
* CS       -> D10
  */

#include <7Semi_MAX31856.h>

// Arduino Uno
#define SCK 13
#define SDO 12
#define SDI 11
#define CS  10

// // ESP32
// #define SCK 18
// #define SDO 19
// #define SDI 23
// #define CS 5

#define CLOCK 1000000

MAX31856_7Semi thermo;

void setup() {
  Serial.begin(115200);

  // Initialize sensor (CS pin = D10)
  thermo.begin(CS, SPI, CLOCK, SCK, SDO, SDI);

  // Select thermocouple type
  thermo.setThermocoupleType(TC_K);

  Serial.println("Configuring fault limits...");

  // Cold junction limits
  thermo.setColdJunctionHighC(40);
  Serial.print("CJ HIGH: ");
  Serial.println(thermo.getColdJunctionHighC());

  thermo.setColdJunctionLowC(20);
  Serial.print("CJ LOW: ");
  Serial.println(thermo.getColdJunctionLowC());

  // Thermocouple limits
  thermo.setThermocoupleHighC(36);
  Serial.print("TC HIGH: ");
  Serial.println(thermo.getThermocoupleHighC());

  thermo.setThermocoupleLowC(20);
  Serial.print("TC LOW: ");
  Serial.println(thermo.getThermocoupleLowC());

  // Clear any old fault flags
  thermo.clearFault();

  Serial.println("Setup complete\n");
}

void loop() {
  uint8_t fault = thermo.readFaultStatus();

  if (fault) {
    Serial.println("Sensor Fault Detected");
    Serial.print("Fault Code: ");
    Serial.println(fault);

    // Clear fault so next cycle can check again
    thermo.clearFault();
  }

  float temp = thermo.readThermocoupleTemperature();

  Serial.print("Temperature: ");
  Serial.print(temp);
  Serial.println(" C");

  float cj_temp = thermo.readColdJunctionTemperature();

  Serial.print("CJ Temperature: ");
  Serial.print(cj_temp);
  Serial.println(" C");

  Serial.println();

  delay(1000);
}
