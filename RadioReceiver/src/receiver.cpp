#include "Arduino.h"
#include <stdint.h>
#include <SPI.h>
#include <RF24.h>
#include "EnvironmentCalculations.h"

// This is just the way the RF24 library works:
// Hardware configuration: Set up nRF24L01 radio on SPI bus (pins 10, 11, 12, 13) plus pins 7 & 8
RF24 radio(7, 8);

byte addresses[6] = "10000";

#define SERIAL_BAUD 57600

// Data packet
const int TEMP_OFFSET = 0;
const int HUMIDITY_OFFSET = 4;
const int PRESSURE_OFFSET = 8;
const int LIGHT_OFFSET = 12;
const unsigned int PAYLOAD_SIZE = 3 * sizeof(float) + sizeof(uint16_t);
char payload[PAYLOAD_SIZE];

// -----------------------------------------------------------------------------
// SETUP   SETUP   SETUP   SETUP   SETUP   SETUP   SETUP   SETUP   SETUP
// -----------------------------------------------------------------------------
void setup()
{
  Serial.begin(SERIAL_BAUD);
  Serial.println("THIS IS THE RECEIVER CODE - YOU NEED THE OTHER ARDUINO TO TRANSMIT");

  // Initiate the radio object
  radio.begin();

  // Set the transmit power to lowest available to prevent power supply related issues
  radio.setPALevel(RF24_PA_MAX);

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
  // radio.openWritingPipe(addresses[0]);
  radio.openReadingPipe(1, addresses);

  // Start the radio listening for data
  radio.startListening();
}

// -----------------------------------------------------------------------------
// We are LISTENING on this device only (although we do transmit a response)
// -----------------------------------------------------------------------------
void loop()
{

  // This is what we receive from the other device (the transmitter)
  if (radio.available())
  {
    radio.read(payload, PAYLOAD_SIZE);

    // int * data = (int *)(payload+4);
    float *pres = (float *)(payload + PRESSURE_OFFSET);
    float *temp = (float *)(payload + TEMP_OFFSET);
    float *hum = (float *)(payload + HUMIDITY_OFFSET);
    uint16_t *light = (uint16_t *)(payload + LIGHT_OFFSET);

    const int DOT = 2;// +2 for dot and \0
    const int PRECISION = 2; 
    const int PRESSURE_WIDTH = 4;
    const int TEMPERATURE_WIDTH = 3;
    const int HUMIDITY_WIDTH = 4;
    const int LIGHT_WIDTH = 3;

    char str_pres[PRESSURE_WIDTH +  PRECISION + DOT];
    char str_temp[TEMPERATURE_WIDTH +  PRECISION + DOT];
    char str_hum[HUMIDITY_WIDTH +  PRECISION + DOT];

    // char *dtostrf(double val, signed char width, unsigned char prec, char *s)
    dtostrf(*pres, PRESSURE_WIDTH, PRECISION, str_pres);
    dtostrf(*temp, TEMPERATURE_WIDTH, PRECISION, str_temp);
    dtostrf(*hum, HUMIDITY_WIDTH, PRECISION, str_hum);
    char buff[128];
    sprintf(buff, "{\"pressure\":\"%s\",\"temperature\":\"%s\",\"humidity\":\"%s\",\"light\":\"%d\"}\0", str_pres, str_temp, str_hum, int(*light));
    //Serial.println(*hum);
    //Serial.println(str_hum);
    //Serial.println(*temp);
    //Serial.println(str_temp);
    
    // send only json
    Serial.println(buff);

    // Serial.print("Presure: ");
    // Serial.print(*pres);
    // Serial.print("\n");

    // Serial.print("Temperature: ");
    // Serial.print(*temp);
    // Serial.print("\n");

    // Serial.print("Humidity: ");
    // Serial.print(*hum);
    // Serial.print("\n");

    // // Assumed environmental values:
    // float referencePressure = 1018.6; // hPa local QFF (official meteor-station reading)
    // float outdoorTemp = 4.7;          // °C  measured local outdoor temp.
    // float barometerAltitude = 1650.3; // meters ... map readings + barometer position
    // EnvironmentCalculations::PresUnit presUnit(EnvironmentCalculations::PresUnit_hPa);
    // EnvironmentCalculations::AltitudeUnit envAltUnit = EnvironmentCalculations::AltitudeUnit_Meters;
    // EnvironmentCalculations::TempUnit envTempUnit = EnvironmentCalculations::TempUnit_Celsius;

    // /// To get correct local altitude/height (QNE) the reference Pressure
    // ///    should be taken from meteorologic messages (QNH or QFF)
    // float altitude = EnvironmentCalculations::Altitude(*pres, envAltUnit, referencePressure, outdoorTemp, envTempUnit);

    // float dewPoint = EnvironmentCalculations::DewPoint(*temp, *hum, envTempUnit);

    // /// To get correct seaLevel pressure (QNH, QFF)
    // ///    the altitude value should be independent on measured pressure.
    // /// It is necessary to use fixed altitude point e.g. the altitude of barometer read in a map
    // float seaLevel = EnvironmentCalculations::EquivalentSeaLevelPressure(barometerAltitude, *temp, *pres, envAltUnit, envTempUnit);

    // float absHum = EnvironmentCalculations::AbsoluteHumidity(*temp, *hum, envTempUnit);

    // Serial.print("\t\tAltitude: ");
    // Serial.print(altitude);
    // Serial.print((envAltUnit == EnvironmentCalculations::AltitudeUnit_Meters ? "m" : "ft"));
    // Serial.print("\t\tDew point: ");
    // Serial.print(dewPoint);
    // Serial.print("°" + String(envTempUnit == EnvironmentCalculations::TempUnit_Celsius ? "C" : "F"));
    // Serial.print("\t\tEquivalent Sea Level Pressure: ");
    // Serial.print(seaLevel);
    // Serial.print(String(presUnit == EnvironmentCalculations::PresUnit_hPa ? "hPa" : "Pa")); // expected hPa and Pa only

    // Serial.print("\t\tHeat Index: ");
    // float heatIndex = EnvironmentCalculations::HeatIndex(*temp, *hum, envTempUnit);
    // Serial.print(heatIndex);
    // Serial.print("°" + String(envTempUnit == EnvironmentCalculations::TempUnit_Celsius ? "C" : "F"));

    // Serial.print("\t\tAbsolute Humidity: ");
    // Serial.println(absHum);
  }
}
