/*
 * 7Semi MAX31856
 *
 * Driver for MAX31856 thermocouple-to-digital converter
 * Supports SPI communication with configurable thermocouple types
 *
 * MIT License
 * Copyright (c) 2025 - present 7Semi
 */
#include "7Semi_MAX31856.h"

MAX31856_7Semi::MAX31856_7Semi(){}


/**
 * Initialize the MAX31856 device
 *
 * - Sets up SPI communication
 * - Configures chip select pin
 * - Starts continuous conversion mode
 * - Default thermocouple type is K
 */
bool MAX31856_7Semi::begin(
        uint8_t cs,
        SPIClass &spiPort,
        uint32_t spiClock,
        uint8_t sck,
        uint8_t miso,
        uint8_t mosi)
{
    cs_pin = cs;
    spi = &spiPort;
    spi_speed = spiClock;

    pinMode(cs_pin, OUTPUT);
    digitalWrite(cs_pin, HIGH);

#ifdef ARDUINO_ARCH_ESP32
    if (sck != 255 && miso != 255 && mosi != 255)
        spi->begin(sck, miso, mosi, cs_pin);
    else
        spi->begin();
#else
    spi->begin();
#endif

    spiSettings = SPISettings(spi_speed, MSBFIRST, SPI_MODE1);

    // Start continuous conversions so temperature is always updated
    setContinuousMode(true);

    // Default thermocouple type
    setThermocoupleType(TC_K);

    return true;
}

/**
 * Select thermocouple type
 *
 * This tells the chip what kind of thermocouple is connected.
 * Make sure this matches your sensor (K, J, T etc.).
 */
void MAX31856_7Semi::setThermocoupleType(MAX31856_THERMOCOUPLE type)
{
    uint8_t reg = readReg(MAX31856_CR1);

    reg &= 0xF0;
    reg |= ((uint8_t)type & 0x0F);

    writeReg(MAX31856_CR1, reg);
}


/**
 * Read back currently configured thermocouple type
 */
MAX31856_THERMOCOUPLE MAX31856_7Semi::getThermocoupleType()
{
    return (MAX31856_THERMOCOUPLE)(readReg(MAX31856_CR1) & 0x0F);
}


/**
 * Enable or disable continuous conversion mode
 *
 * - When enabled, device continuously measures temperature
 * - When disabled, conversion must be triggered manually
 * - CR0 bit 7 controls conversion mode
 */
void MAX31856_7Semi::setContinuousMode(bool enable)
{
    writeBit(MAX31856_CR0, 7, enable);
}


/**
 * Trigger one-shot temperature conversion
 *
 * - Starts a single temperature measurement
 * - Used when continuous mode is disabled
 * - CR0 bit 6 controls one-shot conversion
 */
void MAX31856_7Semi::setOneShot(bool enable)
{
    writeBit(MAX31856_CR0, 6, enable);
}


/**
 * Configure open-circuit fault detection mode
 *
 * - Selects how thermocouple open circuit faults are detected
 * - Updates CR0 bits [5:4]
 * - Mode values defined in MAX31856_OC_MODE enum
 */
void MAX31856_7Semi::setOpenCircuitMode(MAX31856_OC_FAULT mode)
{
    uint8_t cr0 = readReg(MAX31856_CR0);

    cr0 &= ~(0x03 << 4);     // Clear OC fault mode bits
    cr0 |= ((uint8_t)mode << 4);

    writeReg(MAX31856_CR0, cr0);
}


/**
 * Enable or disable cold junction compensation
 *
 * - When enabled, device compensates for cold junction temperature
 * - Required for accurate thermocouple measurements
 * - Controlled by CR0 bit 3
 */
void MAX31856_7Semi::setColdJunction(bool enable)
{
    writeBit(MAX31856_CR0, 3, enable);
}


/**
 * Configure fault output mode
 *
 * - Selects interrupt or comparator fault output
 * - Uses CR0 bit 2
 * - Mode defined in MAX31856_FaultMode enum
 */
void MAX31856_7Semi::setFaultMode(MAX31856_FaultMode mode)
{
    bool v = false;

    if (mode == FAULT_INTERRUPT)
        v = true;

    writeBit(MAX31856_CR0, 2, v);
}


/**
 * Clear fault status
 *
 * - Clears fault flags inside the device
 * - Writing 1 resets fault status register
 * - Controlled by CR0 bit 1
 */
void MAX31856_7Semi::clearFault()
{
    writeBit(MAX31856_CR0, 1, true);
}

/**
 * Configure noise rejection filter
 *
 * - Selects filter for power line interference
 * - Improves measurement stability
 */
void MAX31856_7Semi::setNoiseFilter(MAX31856_Filter filter)
{
    writeBit(MAX31856_CR0, 0, filter);
}

/**
 * Configure thermocouple type and averaging mode
 *
 * - Sets thermocouple sensor type
 * - Configures sample averaging to reduce measurement noise
 */
void MAX31856_7Semi::setCR1Config(MAX31856_THERMOCOUPLE type, MAX31856_AVG avg)
{
    uint8_t v = 0;
    // Apply new averaging configuration
    v |= ((avg & 0x07) << 4);

    // Apply thermocouple type
    v |= (type & 0x0F);

    writeReg(MAX31856_CR1, v);
}

/**
 * Enable or disable masking of a specific fault
 *
 * - Masks prevent selected faults from triggering fault output
 * - Used to ignore specific fault conditions if required
 */
void MAX31856_7Semi::setMaskFault(MAX31856_FaultMask mask, bool enable)
{
    writeBit(MAX31856_MASK, mask, enable);
}


/**
 * Check whether a specific fault is masked
 *
 * - Returns true if the fault mask is enabled
 * - Returns false if the fault is not masked
 */
bool MAX31856_7Semi::getMaskFault(MAX31856_FaultMask mask)
{
    return readBit(MAX31856_MASK, mask);
}


/**
 * Get complete fault mask register
 *
 * - Returns raw mask register value
 * - Each bit represents masking of a specific fault
 */
uint8_t MAX31856_7Semi::getMaskFault()
{
    return readReg(MAX31856_MASK);
}
/**
 * Set cold junction high threshold
 *
 * - Writes tempC value directly to the device register
 * - Each LSB represents 1°C
 */
void MAX31856_7Semi::setColdJunctionHighC(int8_t tempC)
{
    writeReg(MAX31856_CJHF, (uint8_t)tempC);
}


/**
 * Read cold junction high threshold
 */
int8_t MAX31856_7Semi::getColdJunctionHighC()
{
    return (int8_t)readReg(MAX31856_CJHF);
}


/**
 * Set cold junction low threshold 
 */
void MAX31856_7Semi::setColdJunctionLowC(int8_t tempC)
{
    writeReg(MAX31856_CJLF, (uint8_t)tempC);
}


/**
 * Read cold junction low threshold 
 */
int8_t MAX31856_7Semi::getColdJunctionLowC()
{
    return (int8_t)readReg(MAX31856_CJLF);
}

/**
 * Set thermocouple high threshold (raw register value)
 */
void MAX31856_7Semi::setThermocoupleHighRaw(int16_t raw)
{
    write2Reg(MAX31856_LTHFTH, raw);
}


/**
 * Read thermocouple high threshold (raw register value)
 */
int16_t MAX31856_7Semi::getThermocoupleHighRaw()
{
    return (int16_t)read2Reg(MAX31856_LTHFTH);
}


/**
 * Set thermocouple high threshold in Celsius
 */
void MAX31856_7Semi::setThermocoupleHighC(float tempC)
{
    setThermocoupleHighRaw((int16_t)(tempC * 16.0f));
}


/**
 * Read thermocouple high threshold in Celsius
 */
float MAX31856_7Semi::getThermocoupleHighC()
{
    return getThermocoupleHighRaw() * 0.0625f;
}


/**
 * Set thermocouple low threshold (raw register value)
 */
void MAX31856_7Semi::setThermocoupleLowRaw(int16_t raw)
{
    write2Reg(MAX31856_LTLFTH, raw);
}


int16_t MAX31856_7Semi::getThermocoupleLowRaw()
{
    return (int16_t)read2Reg(MAX31856_LTLFTH);
}


/**
 * Set thermocouple low threshold in Celsius
 */
void MAX31856_7Semi::setThermocoupleLowC(float tempC)
{
    setThermocoupleLowRaw((int16_t)(tempC * 16.0f));
}


float MAX31856_7Semi::getThermocoupleLowC()
{
    return getThermocoupleLowRaw() * 0.0625f;
}

/**
 * Set cold junction offset (raw value)
 */
void MAX31856_7Semi::setColdJunctionOffsetRaw(int8_t raw)
{
    writeReg(MAX31856_CJTO, (uint8_t)raw);
}


/**
 * Read cold junction offset (raw value)
 */
int8_t MAX31856_7Semi::getColdJunctionOffsetRaw()
{
    return (int8_t)readReg(MAX31856_CJTO);
}


/**
 * Set cold junction offset in Celsius
 */
void MAX31856_7Semi::setColdJunctionOffsetC(float offsetC)
{
    setColdJunctionOffsetRaw((int8_t)(offsetC * 16.0f));
}


/**
 * Read cold junction offset in Celsius
 */
float MAX31856_7Semi::getColdJunctionOffsetC()
{
    return getColdJunctionOffsetRaw() * 0.0625f;
}

/**
 * Convert thermocouple Celsius → raw register value
 *
 * Resolution = 0.0078125°C
 */
int16_t MAX31856_7Semi::celsiusToThermocoupleRaw(float tempC)
{
    return (int16_t)(tempC * 128.0f);
}


/**
 * Convert thermocouple raw value → Celsius
 */
float MAX31856_7Semi::rawToThermocoupleCelsius(int32_t raw)
{
    return raw * 0.0078125f;
}


/**
 * Read cold junction temperature
 *
 * This is the internal reference temperature used
 * for thermocouple compensation.
 */
float MAX31856_7Semi::readColdJunctionTemperature()
{
    uint8_t buffer[2];

    readNReg(MAX31856_CJTH, buffer, 2);

    int16_t temp =
        ((int16_t)buffer[0] << 8) |
        buffer[1];

    temp >>= 2;

    return temp * 0.015625;
}


/**
 * Read thermocouple temperature
 *
 * Returns the temperature measured by the thermocouple.
 * Value is converted to Celsius.
 */
float MAX31856_7Semi::readThermocoupleTemperature()
{
    uint8_t buffer[3];

    readNReg(MAX31856_LTCBH, buffer, 3);

    int32_t temp =
        ((int32_t)buffer[0] << 16) |
        ((int32_t)buffer[1] << 8)  |
        buffer[2];

    temp >>= 5;
    return temp * 0.0078125;
}



int32_t MAX31856_7Semi::readRawThermocouple()
{
    uint8_t buffer[3];

    readNReg(MAX31856_LTCBH, buffer, 3);

    return ((int32_t)buffer[0] << 16) |
           ((int32_t)buffer[1] << 8) |
           buffer[2];
}

uint8_t MAX31856_7Semi::readFault()
{
    return readReg(MAX31856_SR);
}

/**
 * Read fault status register
 *
 * If something goes wrong (open thermocouple,
 * over-voltage, etc.), the chip reports it here.
 */
uint8_t MAX31856_7Semi::readFaultStatus()
{
    return readReg(MAX31856_SR);
}

/**
 * Read a single register from the chip
 */
uint8_t MAX31856_7Semi::readReg(uint8_t reg)
{
    spi->beginTransaction(spiSettings);

    digitalWrite(cs_pin, LOW);

    spi->transfer(reg & 0x7F);
    uint8_t value = spi->transfer(0x00);

    digitalWrite(cs_pin, HIGH);

    spi->endTransaction();

    return value;
}


/**
 * Write one byte to a register
 */
void MAX31856_7Semi::writeReg(uint8_t addr, uint8_t data)
{
    spi->beginTransaction(spiSettings);

    digitalWrite(cs_pin, LOW);

    spi->transfer(addr | 0x80);
    spi->transfer(data);

    digitalWrite(cs_pin, HIGH);

    spi->endTransaction();
}

/**
 * Read a single register from MAX31856
 *
 * - Sends register address with MSB = 0 (read operation)
 * - Reads one byte from the device
 * - SPI transaction is protected using beginTransaction()
 */
uint16_t MAX31856_7Semi::read2Reg(uint8_t reg)
{
    spi->beginTransaction(spiSettings);

    digitalWrite(cs_pin, LOW);

    spi->transfer(reg & 0x7F);   // Ensure read operation
    uint8_t msb = spi->transfer(0x00);
    uint8_t lsb = spi->transfer(0x00);

    digitalWrite(cs_pin, HIGH);

    spi->endTransaction();

    return (msb<<8)|lsb;
}


/**
 * Read multiple consecutive registers
 *
 * - Used for reading temperature data blocks
 * - Address automatically increments inside MAX31856
 * - Buffer must have at least "length" bytes
 */
void MAX31856_7Semi::readNReg(uint8_t addr, uint8_t *buffer, uint8_t length)
{
    spi->beginTransaction(spiSettings);

    digitalWrite(cs_pin, LOW);

    spi->transfer(addr & 0x7F);

    for (uint8_t i = 0; i < length; i++)
    {
        buffer[i] = spi->transfer(0x00);
    }

    digitalWrite(cs_pin, HIGH);

    spi->endTransaction();
}

/**
 * Write a 16-bit value to 2 registers
 *
 * - MSB first 
 * - Used for registers that store 16-bit data values
 */
void MAX31856_7Semi::write2Reg(uint8_t addr, uint16_t data)
{
    spi->beginTransaction(spiSettings);

    digitalWrite(cs_pin, LOW);

    spi->transfer(addr | 0x80);

    spi->transfer((data >> 8) & 0xFF);  // High byte
    spi->transfer(data & 0xFF);         // Low byte

    digitalWrite(cs_pin, HIGH);

    spi->endTransaction();
}



/**
 * Read a specific bit from a register
 *
 * - Returns true if the bit is set
 * - Position range: 0–7
 */
bool MAX31856_7Semi::readBit(uint8_t reg, uint8_t position)
{
    uint8_t v = readReg(reg);

    return (v & (1U << position)) != 0;
}


/**
 * Write a specific bit inside a register
 *
 * - Performs read-modify-write
 * - Only the selected bit is changed
 * - Other bits remain untouched
 */
void MAX31856_7Semi::writeBit(uint8_t reg, uint8_t position, bool value)
{
    uint8_t v = readReg(reg);

    if (value)
        v |= (1U << position);
    else
        v &= ~(1U << position);

    writeReg(reg, v);
}
