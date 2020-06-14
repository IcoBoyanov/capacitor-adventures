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

  // Open a writing and reading pipe on each radio, with same addresses
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

    float *pres = (float *)(payload + PRESSURE_OFFSET);
    float *temp = (float *)(payload + TEMP_OFFSET);
    float *hum = (float *)(payload + HUMIDITY_OFFSET);
    uint16_t *light = (uint16_t *)(payload + LIGHT_OFFSET);

    const int DOT = 2;// +2 for dot and \0
    const int PRECISION = 2; 
    const int PRESSURE_WIDTH = 4;
    const int TEMPERATURE_WIDTH = 3;
    const int HUMIDITY_WIDTH = 4;

    char str_pres[PRESSURE_WIDTH +  PRECISION + DOT];
    char str_temp[TEMPERATURE_WIDTH +  PRECISION + DOT];
    char str_hum[HUMIDITY_WIDTH +  PRECISION + DOT];

    // char *dtostrf(double val, signed char width, unsigned char prec, char *s)
    dtostrf(*pres, PRESSURE_WIDTH, PRECISION, str_pres);
    dtostrf(*temp, TEMPERATURE_WIDTH, PRECISION, str_temp);
    dtostrf(*hum, HUMIDITY_WIDTH, PRECISION, str_hum);
    char buff[128];
    
    sprintf(buff, "{\"pressure\":\"%s\",\"temperature\":\"%s\",\"humidity\":\"%s\",\"light\":\"%d\"}\0", str_pres, str_temp, str_hum, int(*light));
    // send only json
    Serial.println(buff);
  }
}
