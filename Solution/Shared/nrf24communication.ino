#include <RH_NRF24.h>

RH_NRF24 nrf24;

#ifndef DEBUG
#define DEBUG true
#endif

bool nrf24Initialize() {
	if (!nrf24.init())
		Serial.println("init failed");
	// Defaults after init are 2.402 GHz (channel 2), 2Mbps, 0dBm
	if (!nrf24.setChannel(1))
		Serial.println("setChannel failed");
	if (!nrf24.setRF(RH_NRF24::DataRate2Mbps, RH_NRF24::TransmitPower0dBm))
		Serial.println("setRF failed");
}

bool compareBuffers(uint8_t *buf1, uint8_t *buf2, uint8_t len) {
	for (uint8_t n = 0; n < len; n += 1) {
		if (buf1[n] != buf2[n]) {
			return false;
		}
	}
	return true;
}

bool nrf24ReceiveMessage(uint8_t* str, uint8_t* len) {
	// receive a message or timeout after 5 seconds
	unsigned long time = millis();

	if (DEBUG) {
		Serial.println("Receiving message..");
	}

	if (!nrf24.waitAvailableTimeout(5000)) {
		return false;
	}

	if (!nrf24.recv(str, len)) {
		return false;
	}

	if (DEBUG) {
		Serial.println("Message received.");
		Serial.print(" Length: ");
		Serial.println((int) len);
		Serial.print(" Message: ");
		Serial.println((char *)str);
	}

	uint8_t* buf;
	uint8_t blen;
	while (true) {
		if (DEBUG) {
			Serial.println("Attempting confirmation..");
		}
		nrf24.send(str, *len);
		delay(100);
		if (millis() > time + 5000L && millis() >= time) {
			Serial.println("5 second timeout");
			return false;
		}
		if (nrf24.waitAvailableTimeout(100)) {
			if (!nrf24.recv(buf, &blen)) {
				continue;
			}
			if (blen != 7) {
				continue;
			}
			if (!compareBuffers(buf, (uint8_t*) "OKSTOP", 7)) {
				continue;
			}
			if (DEBUG) {
				Serial.println("Receiving message..");
			}
			break;
		}
	}

	// since we don't want to wait a full 5 seconds; we're just going to send 20 confirm messages
	for (uint8_t n = 0; n < 20; n += 1) {
		nrf24.send((uint8_t*) "CONFIRMED", 10);
		delay(100);
	}
	return true;
}

bool nrf24SendMessage(uint8_t* str, uint8_t len) {
	bool success = false;
	unsigned long time = millis();

	if (DEBUG) {
		Serial.println("Sending message..");
	}

	while (!success) {
		if (DEBUG) {
			Serial.println("Attemp started..");
		}
		// send the message and wait until we have a message available
		do {
			nrf24.send(str, len);
			if (millis() > time + 5000L && millis() >= time) {
				if (DEBUG) {
					Serial.println("5 second timeout");
				}
				return false;
			}
		} while (!nrf24.waitAvailableTimeout(100));

		uint8_t* buf;
		uint8_t blen;

		if (!nrf24.recv(buf, &blen)) {
			continue;
		}

		if (DEBUG) {
			Serial.println("Message received.");
			Serial.print(" Length: ");
			Serial.println((int)blen);
			Serial.print(" Message: ");
			Serial.println((char *)buf);
		}
		

		// if the received message is not our message, repeat our message
		if (len != blen) {
			continue;
		}
		if (!compareBuffers(buf, str, len)) {
			continue;
		}

		// success! notify the other party that they can stop confirming the message and timeout after a second
		do {
			if (DEBUG) {
				Serial.println("Attempt confirmation.");
			}
			nrf24.send((uint8_t*) "OKSTOP", 7);
			if (millis() > time + 5000L && millis() >= time) {
				Serial.println("5 second timeout");
				break;
			}
		} while (!nrf24.waitAvailableTimeout(100));

		// empty received message buffer if we got one
		if (nrf24.available()) {
			nrf24.recv(buf, &blen);
		}

		success = true;
		if (DEBUG) {
			Serial.println("Message succesfully sent.");
		}
	}

	return true;
}