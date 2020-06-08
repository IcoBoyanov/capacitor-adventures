#include "Arduino.h"
#include <SPI.h>
#include <RF24.h>

// This is just the way the RF24 library works:
// Hardware configuration: Set up nRF24L01 radio on SPI bus (pins 10, 11, 12, 13) plus pins 7 & 8
RF24 radio(7, 8);

byte addresses[6] = "10000";


const unsigned int PAYLOAD_SIZE = 32;
char payload[PAYLOAD_SIZE];


// -----------------------------------------------------------------------------
// SETUP   SETUP   SETUP   SETUP   SETUP   SETUP   SETUP   SETUP   SETUP
// -----------------------------------------------------------------------------
void setup() {
  Serial.begin(57600);
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
void loop() {

  // This is what we receive from the other device (the transmitter)
  if ( radio.available()) {
    radio.read(payload, PAYLOAD_SIZE);

    int * data = (int *)(payload+4);

    Serial.println(*data);
  }
}
