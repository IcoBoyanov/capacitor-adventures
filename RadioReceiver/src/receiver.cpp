
/*
* Getting Started example sketch for nRF24L01+ radios
* This is a very basic example of how to send data from one node to another
* Updated: Dec 2014 by TMRh20
*/

#include <SPI.h>
#include "RF24.h"

/* Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7 & 8 */
RF24 radio(7, 8);
/**********************************************************/

byte addresses[][6] = {"trans", "rciev"};

struct Data
{
    char str[13]= "radio, gugu!";
    unsigned long timestamp;
};
Data data;

const int PAYLOAD_SIZE = 4;
char payload[PAYLOAD_SIZE];

void setup()
{
  Serial.begin(115200);
  radio.begin();
  radio.setAddressWidth(5);
  // Set the PA Level low to prevent power supply related issues since this is a
  // getting_started sketch, and the likelihood of close proximity of the devices. RF24_PA_MAX is default.
  radio.setPALevel(RF24_PA_HIGH);
  radio.setAutoAck(false);
  radio.setDataRate(rf24_datarate_e::RF24_250KBPS);
  
  radio.disableDynamicPayloads();
  radio.setPayloadSize(PAYLOAD_SIZE);
  Serial.println(radio.getPALevel());
  Serial.println(radio.getARC());
    // Open a writing and reading pipe on each radio, with opposite addresses
    // radio.openWritingPipe(addresses[1]);
    radio.openReadingPipe(1, addresses[0]);

    // Start the radio listening for data
    radio.startListening();
}

void loop()
{
    if (radio.available())
    {
      //Data input; // Grab the response, compare, and send to debugging spew
      radio.read(payload, PAYLOAD_SIZE);
      int * num = (int*)payload;
      //Serial.print("Got ");
      Serial.println(*num);
    }

} // Loop
