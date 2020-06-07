#include "Arduino.h"
#include <SPI.h>
#include <RF24.h>

// This is just the way the RF24 library works:
// Hardware configuration: Set up nRF24L01 radio on SPI bus (pins 10, 11, 12, 13) plus pins 7 & 8
// TODO MOVE D9 --> D7
RF24 radio(9, 8);

byte addresses[][6] = {"10000","20000"};

const unsigned int PAYLOAD_SIZE = 32;
char payload[PAYLOAD_SIZE];

// -----------------------------------------------------------------------------
// SETUP   SETUP   SETUP   SETUP   SETUP   SETUP   SETUP   SETUP   SETUP
// -----------------------------------------------------------------------------
void setup()
{
    Serial.begin(57600);
    Serial.println("THIS IS THE TRANSMITTER CODE - YOU NEED THE OTHER ARDIUNO TO SEND BACK A RESPONSE");

    // Initiate the radio object
    radio.begin();

    // Set the transmit power to lowest available to prevent power supply related issues
    radio.setPALevel(RF24_PA_MAX);

    // Set the speed of the transmission to the quickest available
    radio.setDataRate(RF24_250KBPS);

    radio.enableDynamicAck();
    radio.enableDynamicPayloads();
    radio.setAutoAck(true);

    // Use a channel unlikely to be used by Wifi, Microwave ovens etc
    radio.setChannel(77);

    radio.setPayloadSize(PAYLOAD_SIZE);
    // Open a writing and reading pipe on each radio, with opposite addresses
    radio.openWritingPipe(addresses[1]);
    // radio.openReadingPipe(1, addresses[0]);

    radio.stopListening();
}

// -----------------------------------------------------------------------------
// LOOP     LOOP     LOOP     LOOP     LOOP     LOOP     LOOP     LOOP     LOOP
// -----------------------------------------------------------------------------
void loop()
{    
 // This is what transmits data
    int *data = (int *)(payload + 4);
    (*data)++;

    if (!radio.write(payload, PAYLOAD_SIZE))
    {
        Serial.println("No acknowledgement of transmission - receiving radio device connected?");
    }
}
