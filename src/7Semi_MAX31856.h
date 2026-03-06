/*
 * 7Semi MAX31856
 *
 * Driver for MAX31856 thermocouple-to-digital converter
 * Supports SPI communication with configurable thermocouple types
 *
 * MIT License
 * Copyright (c) 2025 - present 7Semi
 */

 #ifndef _7SEMI_MAX31856_H
#define _7SEMI_MAX31856_H

#include <Arduino.h>
#include <SPI.h>

// Registers
#define MAX31856_CR0      0x00
#define MAX31856_CR1      0x01
#define MAX31856_MASK     0x02
#define MAX31856_CJHF     0x03
#define MAX31856_CJLF     0x04
#define MAX31856_LTHFTH   0x05
#define MAX31856_LTHFTL   0x06
#define MAX31856_LTLFTH   0x07
#define MAX31856_LTLFTL   0x08
#define MAX31856_CJTO     0x09
#define MAX31856_CJTH     0x0A
#define MAX31856_CJTL     0x0B
#define MAX31856_LTCBH    0x0C
#define MAX31856_LTCBM    0x0D
#define MAX31856_LTCBL    0x0E
#define MAX31856_SR       0x0F



// Open-Circuit Detection (CR0 bits 5:4)
enum MAX31856_OC_FAULT
{
    OC_DISABLED      = 0b00, // Disabled
    OC_ENABLED_FAST  = 0b01, // Enabled, RS < 5kΩ, fast (typ 13.3 ms / max 15 ms)
    OC_ENABLED_MED   = 0b10, // Enabled, 5kΩ < RS < 40kΩ, time constant < 2ms
    OC_ENABLED_SLOW  = 0b11  // Enabled, 5kΩ < RS < 40kΩ, time constant > 2ms
};
// MAX31856 Fault Mode (CR1 bit 7)
enum MAX31856_FaultMode
{
    FAULT_COMPARATOR = 0, // Comparator Mode (default)
    FAULT_INTERRUPT  = 1  // Interrupt Mode
};
// MAX31856 Noise Filter Selection (CR0 bit 1)
enum MAX31856_Filter
{
    FILTER_60HZ = 0, // Reject 60 Hz and harmonics (default)
    FILTER_50HZ = 1  // Reject 50 Hz and harmonics
};
// MAX31856 Thermocouple Voltage Conversion Averaging Mode
enum MAX31856_AVG
{
    AVG_1_SAMPLE   = 0b000, // 1 sample (default)
    AVG_2_SAMPLES  = 0b001, // 2 samples averaged
    AVG_4_SAMPLES  = 0b010, // 4 samples averaged
    AVG_8_SAMPLES  = 0b011, // 8 samples averaged
    AVG_16_SAMPLES = 0b100, // 16 samples averaged (1xx also allowed)
};

// MAX31856 Thermocouple Type
enum MAX31856_THERMOCOUPLE
{
    TC_B = 0, // B type
    TC_E = 1, // E type
    TC_J = 2, // J type
    TC_K = 3, // K type (default)
    TC_N = 4, // N type
    TC_R = 5, // R type
    TC_S = 6, // S type
    TC_T = 7, // T type
    TC_VOLTAGE_GAIN8  = 0b1000,//0b1001,0b1010,0b1011, // Voltage mode, Gain = 8
    TC_VOLTAGE_GAIN32 = 0b1100// 0b1101,0b1110,0b1111, // Voltage mode, Gain = 32
};

// MAX31856 Fault Masks (MASK register 0x02)
enum MAX31856_FaultMask : uint8_t
{
    FAULT_CJ_HIGH = 5,   // Cold Junction High Threshold Mask
    FAULT_CJ_LOW  = 4,   // Cold Junction Low Threshold Mask
    FAULT_TC_HIGH = 3,   // Thermocouple High Threshold Mask
    FAULT_TC_LOW  = 2,   // Thermocouple Low Threshold Mask
    FAULT_OVUV    = 1,   // Over/Under Voltage Mask
    FAULT_OC      = 0    // Open Circuit Fault Mask
};

// MAX31856 Fault Status bits (SR register)
enum MAX31856_FaultStatus : uint8_t
{
    CJ_RANGE =  7, // Cold-Junction out-of-range
    TC_RANGE =  6, // Thermocouple out-of-range
    CJ_HIGH  =  5, // CJ High threshold exceeded
    CJ_LOW   =  4, // CJ Low threshold exceeded
    TC_HIGH  =  3, // TC High threshold exceeded
    TC_LOW   =  2, // TC Low threshold exceeded
    OVUV     =  1, // Over/Undervoltage
    OPEN     =  0  // Thermocouple open-circuit
};

class MAX31856_7Semi
{
public:

MAX31856_7Semi();

/**
* Initialize the MAX31856 device
*
* - Call this once in setup() before using the sensor
* - Sets up SPI communication
* - Default SPI pins are used unless custom pins are provided
* - Chip select pin can be changed if needed
*/
bool begin(
        uint8_t cs = 10,
        SPIClass &spiPort = SPI,
        uint32_t spiClock = 10000000,
        uint8_t sck = -1,
        uint8_t miso = -1,
        uint8_t mosi = -1
);

/**
* Set thermocouple type
*
* - Selects thermocouple sensor type
* - Required for correct temperature conversion
*/
void setThermocoupleType(MAX31856_THERMOCOUPLE type);


/**
* Get configured thermocouple type
*
* - Returns thermocouple type
* - Value corresponds to MAX31856_THERMOCOUPLE enum
*/
MAX31856_THERMOCOUPLE getThermocoupleType();

/**
* Enable or disable continuous temperature measurement
*
* - When enabled, the device continuously updates thermocouple readings
* - When disabled, measurements must be triggered manually
* - Useful for real-time monitoring applications
*/
void setContinuousMode(bool enable);


/**
* Start a single temperature measurement
*
* - Triggers one temperature conversion
* - Used when continuous mode is disabled
* - Call when a new reading is required
*/
void setOneShot(bool enable);


/**
* Configure thermocouple open-circuit detection mode
*
* - Selects how the device checks for disconnected thermocouples
* - Helps detect wiring faults in the sensor
* - Mode values are defined in MAX31856_OC_MODE enum
*/
void setOpenCircuitMode(MAX31856_OC_FAULT mode);


/**
* Enable or disable cold junction compensation
*
* - Compensates thermocouple readings using internal reference temperature
* - Should normally remain enabled for accurate measurements
*/
void setColdJunction(bool enable);


/**
* Configure fault output behavior
*
* - Selects how the device reports fault conditions
* - Mode values are defined in MAX31856_FaultMode enum
*/
void setFaultMode(MAX31856_FaultMode mode);


/**
* Clear device fault status
*
* - Resets internal fault flags
* - Call after handling an error condition
*/
void clearFault();


/**
* Configure noise rejection filter
*
* - Selects filter for power line interference
* - Helps improve measurement stability in noisy environments
*/
void setNoiseFilter(MAX31856_Filter filter);

/**
* Configure thermocouple type and averaging settings
*
* - Sets thermocouple sensor type 
* - Configures internal averaging to improve measurement stability
* - Use averaging when readings are noisy
*/
void setCR1Config(MAX31856_THERMOCOUPLE type, MAX31856_AVG avg);

/**
 * Enable or disable masking of a specific fault
 *
 * - Masks prevent selected faults from triggering fault output
 * - Useful when certain fault conditions should be ignored
 */
void setMaskFault(MAX31856_FaultMask mask, bool enable);


/**
 * Check whether a specific fault is masked
 *
 * - Returns true if the selected fault is masked
 * - Returns false if the fault is active
 */
bool getMaskFault(MAX31856_FaultMask mask);


/**
 * Get complete fault mask register
 *
 * - Returns raw mask configuration
 * - Each bit represents a masked fault condition
 */
uint8_t getMaskFault();

/**
 * Set cold junction high threshold in Celsius
 *
 * - 1°C/LSB
 */
void setColdJunctionHighC(int8_t tempC);

/**
 * Get cold junction high threshold in Celsius
 */
int8_t getColdJunctionHighC();


/**
 * Set cold junction low threshold in Celsius
 */
void setColdJunctionLowC(int8_t tempC);

/**
 * Get cold junction low threshold in Celsius
 */
int8_t getColdJunctionLowC();



/**
 * Set thermocouple high threshold (raw value)
 */
void setThermocoupleHighRaw(int16_t raw);

/**
 * Get thermocouple high threshold (raw value)
 */
int16_t getThermocoupleHighRaw();

/**
 * Set thermocouple high threshold in Celsius
 */
void setThermocoupleHighC(float tempC);

/**
 * Get thermocouple high threshold in Celsius
 */
float getThermocoupleHighC();



/**
 * Set thermocouple low threshold (raw value)
 */
void setThermocoupleLowRaw(int16_t raw);

/**
 * Get thermocouple low threshold (raw value)
 */
int16_t getThermocoupleLowRaw();

/**
 * Set thermocouple low threshold in Celsius
 */
void setThermocoupleLowC(float tempC);

/**
 * Get thermocouple low threshold in Celsius
 */
float getThermocoupleLowC();



/**
 * Set cold junction offset (raw value)
 */
void setColdJunctionOffsetRaw(int8_t raw);

/**
 * Get cold junction offset (raw value)
 */
int8_t getColdJunctionOffsetRaw();

/**
 * Set cold junction offset in Celsius
 */
void setColdJunctionOffsetC(float offsetC);

/**
 * Get cold junction offset in Celsius
 */
float getColdJunctionOffsetC();

int16_t celsiusToThermocoupleRaw(float tempC);


/**
 * Read thermocouple temperature
 *
 * - Returns the measured thermocouple temperature in Celsius
 * - This is the main temperature reading from the connected sensor
 */
float readThermocoupleTemperature();
float rawToThermocoupleCelsius(int32_t raw);
int8_t celsiusToCJRaw(float tempC);
float rawToCJCelsius(int8_t raw);

/**
 * Read cold junction temperature
 *
 * - Returns the internal reference temperature used for compensation
 * - Helpful for debugging or verifying sensor environment
 */
float readColdJunctionTemperature();


/**
 * Read raw thermocouple register value
 *
 * - Returns the raw measurement data from the chip
 * - Mostly useful for debugging or custom processing
 */
int32_t readRawThermocouple();


/**
 * Read fault register
 *
 * - Returns the current fault flags reported by the device
 * - Used to check if the sensor or wiring has an issue
 */
uint8_t readFault();


/**
 * Read fault status
 *
 * - Returns the fault status byte from the device
 * - Can be used to decode specific fault conditions
 */
uint8_t readFaultStatus();

private:

    SPIClass *spi;
    SPISettings spiSettings;
    uint32_t spi_speed;
    uint8_t cs_pin;
    
/**
 * Read a single register 
 *
 * - Sends register address with read bit cleared
 * - Reads one byte from the selected register
 * - Used for configuration or status registers
 */
uint8_t readReg(uint8_t addr);

/**
 * Read two consecutive registers
 *
 * - Returns a 16-bit value read from the device
 * - High byte is read first followed by the low byte
 * - Used for registers that store 16-bit data values
 */
uint16_t read2Reg(uint8_t reg);

/**
 * Read multiple registers starting from a given address
 *
 * - Device automatically increments register address
 * - Useful for reading temperature data blocks
 * - Buffer must contain enough space for the requested length
 */
void readNReg(uint8_t addr, uint8_t *buffer, uint8_t length);


/**
 * Write a value to a address
 *
 * - Sets write bit in register address
 * - Writes one byte to the selected register
 * - Used for configuration registers
 */
void writeReg(uint8_t addr, uint8_t data);

/**
 * Write a 16-bit value to two consecutive registers
 *
 * - MSB first 
 * - Used for writing threshold values
 */
void write2Reg(uint8_t addr, uint16_t data);


/**
 * Read a specific bit from a register
 *
 * - Returns true if the selected bit is set
 * - Returns false if the bit is cleared
 * - Position range: 0 to 7
 */
bool readBit(uint8_t reg, uint8_t position);


/**
 * Modify a single bit in a register
 *
 * - Performs read-modify-write operation
 * - Only the selected bit is changed
 * - Other register bits remain unchanged
 */
void writeBit(uint8_t reg, uint8_t position, bool value);

/**
 * Convert Celsius temperature to MAX31856 raw value
 *
 * - Used when writing thermocouple threshold registers
 * - MAX31856 resolution is 0.0078125°C per LSB
 */
int16_t celsiusToRaw(float tempC)
{
    return (int16_t)(tempC * 128.0f);
}
/**
 * Convert thermocouple raw value to Celsius
 *
 * - MAX31856 thermocouple resolution is 0.0078125°C per LSB
 * - Used when converting raw register data to temperature
 */
};

#endif