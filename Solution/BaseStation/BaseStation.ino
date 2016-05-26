/*
 Name:		BaseStation.ino
 Created:	5/19/2016 4:07:46 PM
 Author:	Use Waterpret
*/

#include <SPI.h>

#define DEBUG true

// RH_NRF24 nrf24(8, 7); // use this to be electrically compatible with Mirf
// RH_NRF24 nrf24(8, 10);// For Leonardo, need explicit SS pin
// RH_NRF24 nrf24(8, 7); // For RFM73 on Anarduino Mini

void setup()
{
	Serial.begin(9600);
	while (!Serial)
		; // wait for serial port to connect. Needed for Leonardo only
	nrf24Initialize();

	uint8_t* data = (uint8_t*) "Test bericht";
	nrf24SendMessage(data, 13);
}

void loop() {

}

