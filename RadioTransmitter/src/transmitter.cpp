
/*
* Getting Started example sketch for nRF24L01+ radios
* This is a very basic example of how to send data from one node to another
* Updated: Dec 2014 by TMRh20
*/

#include <SPI.h>
#include "RF24.h"

/* Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7 & 8 */
RF24 radio(9, 8);
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

    Serial.println(radio.getPALevel());
    Serial.println(radio.getARC());
    
    radio.disableDynamicPayloads();
    radio.setPayloadSize(PAYLOAD_SIZE);

  // Open a writing and reading pipe on each radio, with opposite addresses
  radio.openWritingPipe(addresses[0]);
  // radio.openReadingPipe(1,addresses[1]);
  
  
}

void loop()
{
  int * myNum = (int*)payload;
  *myNum = *myNum + 1;
  /****************** Pong Back Role ***************************/
  //data.timestamp = micros(); // Take the time, and send it.  This will block until complete
  if (!radio.write(payload, PAYLOAD_SIZE))
  {
    Serial.println(F("failed"));
  }
  else
  {
    Serial.print("Sent radio gugu! at: ");
    Serial.print(data.timestamp);
    Serial.print("\n");
  }
  _delay_ms(100);
} // Loop
