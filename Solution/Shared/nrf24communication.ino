#include <RHReliableDatagram.h>
#include <RH_NRF24.h>
#include <SPI.h>

#define CLIENT_ADDRESS 1
#define SERVER_ADDRESS 2

// Singleton instance of the radio driver
RH_NRF24 driver;
// RH_NRF24 driver(8, 7);   // For RFM73 on Anarduino Mini

// Class to manage message delivery and receipt, using the driver declared above
RHReliableDatagram manager(driver);

bool isServer;
uint8_t _nrf24_buf[32];
uint8_t _nrf24_blen = 0;

bool nrf24Initialize(bool server) {
	Serial.print(F(" -Initiating radio:\t"));
	if (manager.init()) {
		Serial.println(F("OK"));
		Serial.print(F(" -Setting radio channel:\t"));
		if (!driver.setChannel(1)) {
			return false;
			Serial.println(F("setChannel failed"));
		}
		else {
			Serial.println(F("Channel 1"));
		}
		Serial.print(F(" -Setting radio settings:\t"));
		if (!driver.setRF(RH_NRF24::DataRate2Mbps, RH_NRF24::TransmitPower0dBm)) {
			Serial.println(F("setRF failed"));
			return false;
		}
		else {
			Serial.println(F("2Mbps, 0dBm"));
		}
	}
	else {
		Serial.println(F("FAILED!!!"));
		return false;
	}

	if (server) {
		manager.setThisAddress(SERVER_ADDRESS);
	}
	else {
		manager.setThisAddress(CLIENT_ADDRESS);
	}
	isServer = server;

	return true;
}

bool compareBuffers(uint8_t *buf1, uint8_t *buf2, uint8_t len) {
	for (uint8_t n = 0; n < len; n += 1) {
		if (buf1[n] != buf2[n]) {
			return false;
		}
	}
	return true;
}

void nrf24DebugPrint(char* str) {
	if (true) {
		Serial.println(str);
	}
}

bool nrf24ReceiveMessage(uint8_t* str, uint8_t* len) {
	uint8_t from;
	if (!manager.waitAvailableTimeout(3000)) {
		return false;
	}

	if (!manager.recvfromAck(str, len, &from)) {
		debugln(F("Couldn't receive message"));
		return false;
	}
	uint8_t* _nrf24_buf = (uint8_t *) "OK";
	if (manager.sendtoWait(_nrf24_buf, 3, from)) {
		return true;
	}
	debugln(F("Did not receive acknowledgement on 'OK'"));
	return false;

}

bool nrf24SendMessage(uint8_t* str, uint8_t len) {
	uint8_t address = SERVER_ADDRESS;
	if (isServer) {
		address = CLIENT_ADDRESS;
	}

	if (!manager.sendtoWait(str, len, address)) {
		debugln(F("No acknowledgement received"));
		return false;
	}

	if (!manager.waitAvailableTimeout(1000)) {
		return false;
	}

	uint8_t from;
	if (manager.recvfromAck(_nrf24_buf, &_nrf24_blen, &from)) {
		return true;
	}
	debugln(F("Could not receive 'OK'"));
	return false;
}

bool radioSleep()
{
	driver.setModeIdle();	
	if (driver.sleep())
	{
		return true;
	}
	else
		return false;
}