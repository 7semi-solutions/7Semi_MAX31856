/**
 * 7Semi MAX31856 Basic Example
 *
 * Demonstrates how to read temperature from a thermocouple
 * using the MAX31856 thermocouple-to-digital converter.
 *
 * Hardware Connection Example
 * - MAX31856 CS  -> Arduino D10
 * - MAX31856 SCK -> SPI SCK
 * - MAX31856 MISO -> SPI MISO
 * - MAX31856 MOSI -> SPI MOSI
 * - VCC -> 3.3V
 * - GND -> GND
 */

#include <7Semi_MAX31856.h>

/**
 * Chip Select pin used for SPI communication
 */
#define CS_PIN 10

/**
 * Create MAX31856 object
 * - CS pin defines which SPI device is selected
 */
MAX31856_7Semi thermocouple;

void setup()
{
    /**
     * Initialize serial communication
     * - Used for temperature monitoring in Serial Monitor
     */
    Serial.begin(115200);

    /**
     * Small delay to allow serial monitor connection
     */
    delay(500);

    Serial.println();
    Serial.println("7Semi MAX31856 Thermocouple Example");

    thermocouple.begin(CS_PIN);

    /**
     * Configure thermocouple type
     * Supported types typically include:
     * - TC_K
     * - TC_J
     * - TC_T
     * - TC_N
     * - TC_S
     * - TC_R
     * - TC_B
     */
    thermocouple.setThermocoupleType(TC_K);

    Serial.println("Thermocouple type: K");
    Serial.println("Reading temperature...");
}

void loop()
{
    /**
     * Read thermocouple temperature
     * - Returns temperature in degrees Celsius
     * - Uses cold junction compensation internally
     */
    float temp = thermocouple.readThermocoupleTemperature();

    /**
     * Basic error handling
     * - If reading fails some implementations may return NaN
     */
    if (isnan(temp))
    {
        Serial.println("Error: Failed to read temperature");
    }
    else
    {
        Serial.print("Thermocouple Temp: ");
        Serial.print(temp);
        Serial.println(" °C");
    }

    /**
     * Delay between measurements
     * - 1 second sampling interval
     */
    delay(1000);
}

