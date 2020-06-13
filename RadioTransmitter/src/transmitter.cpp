#include "Arduino.h"
#include <stdint.h>
#include <SPI.h>
#include <RF24.h>
#include <BME280I2C.h>
#include <Wire.h>

// Hardware configuration to setup BME280 sensor
BME280I2C::Settings settings(
    BME280::OSR_X1,
    BME280::OSR_X1,
    BME280::OSR_X1,
    BME280::Mode_Forced,
    BME280::StandbyTime_1000ms,
    BME280::Filter_16,
    BME280::SpiEnable_False,
    BME280I2C::I2CAddr_0x76);

BME280I2C bme(settings);

// Hardware configuration: Set up nRF24L01 radio on SPI bus (pins 10, 11, 12, 13) plus pins 7 & 8
RF24 radio(7, 8);

// We need only one pipe to Write data
byte addresses[6] = "10000";

const unsigned int PAYLOAD_SIZE = 3 * sizeof(float) + sizeof(uint16_t);
char payload[PAYLOAD_SIZE];
const int TEMP_OFFSET = 0;
const int HUMIDITY_OFFSET = 4;
const int PRESSURE_OFFSET = 8;
const int LIGHT_OFFSET = 12;

// -----------------------------------------------------------------------------
// SETUP   SETUP   SETUP   SETUP   SETUP   SETUP   SETUP   SETUP   SETUP
// -----------------------------------------------------------------------------
void setup()
{
    Serial.begin(57600);
    Serial.println("THIS IS THE TRANSMITTER CODE - YOU NEED THE OTHER ARDIUNO TO SEND BACK A RESPONSE");

    Wire.begin();

    while (!bme.begin())
    {
        Serial.println("Could not find BME280 sensor!");
        delay(1000);
    }

    switch (bme.chipModel())
    {
    case BME280::ChipModel_BME280:
        Serial.println("Found BME280 sensor! Success.");
        break;
    case BME280::ChipModel_BMP280:
        Serial.println("Found BMP280 sensor! No Humidity available.");
        break;
    default:
        Serial.println("Found UNKNOWN sensor! Error!");
    }

    // Initiate the radio object
    radio.begin();

    // Set the transmit power to lowest available to prevent power supply related issues
    radio.setPALevel(RF24_PA_HIGH);

    // Set the speed of the transmission to the quickest available
    radio.setDataRate(RF24_250KBPS);

    //  Enable ack on write
    radio.enableDynamicAck();
    radio.setAutoAck(true);

    // Use a channel unlikely to be used by Wifi, Microwave ovens etc
    radio.setChannel(77);

    // Set PayloadSize to a fixed width
    radio.disableDynamicPayloads();
    radio.setPayloadSize(PAYLOAD_SIZE);

    // Open a writing and reading pipe on each radio, with opposite addresses
    radio.openWritingPipe(addresses);

    // We only need to write data
    radio.stopListening();

    DDRD |= 1 << 2;
}

// -----------------------------------------------------------------------------
// LOOP     LOOP     LOOP     LOOP     LOOP     LOOP     LOOP     LOOP     LOOP
// -----------------------------------------------------------------------------
void loop()
{
    // This is what transmits data
    float *temp = (float *)(payload + TEMP_OFFSET);
    float *hum = (float *)(payload + HUMIDITY_OFFSET);
    float *pres = (float *)(payload + PRESSURE_OFFSET);
    uint16_t *light = (uint16_t *)(payload + LIGHT_OFFSET);

    BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
    BME280::PresUnit presUnit(BME280::PresUnit_hPa);

    bme.read(*pres, *temp, *hum, tempUnit, presUnit);
    
    *light = analogRead(A3);

    if (!radio.write(payload, PAYLOAD_SIZE))
    {
        Serial.println("No acknowledgement of transmission - receiving radio device connected?");
    }
    else
    {
        PORTD |= 1 << 2;
        _delay_ms(125);
        PORTD &= ~(1 << 2);
        _delay_ms(125);
    }
}
