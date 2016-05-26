#include <RH_NRF24.h>

RH_NRF24 nrf24;

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
	while (!nrf24.available()) {
		if (time > 5000L && time < millis() - 5000L) {
			Serial.println("5 second timeout");
			return false;
		}
	}
	uint8_t* buf;
	uint8_t blen;
	if (!nrf24.recv(str, len)) {
		return false;
	}

	Serial.print("Message received: ");
	Serial.println((char*) buf);

	while (true) {
		Serial.println("Attempting to confirm");
		nrf24.send(str, *len);
		delay(100);
		if (millis() > time + 5000L && millis() >= time) {
			Serial.println("5 second timeout");
			return false;
		}
		if (nrf24.available()) {
			if (!nrf24.recv(buf, &blen)) {
				continue;
			}
			if (blen != 7) {
				continue;
			}
			if (!compareBuffers(buf, (uint8_t*) "OKSTOP", 7)) {
				continue;
			}
			Serial.println("Confirmed");
			break;
		}
	}

	// since we don't want to wait a full 5 seconds; we're just going to send 50 confirm messages
	for (uint8_t n = 0; n < 50; n += 1) {
		nrf24.send((uint8_t*) "CONFIRMED", 10);
		delay(10);
	}
	return true;
}

bool nrf24SendMessage(uint8_t* str, uint8_t len) {
	bool success = false;
	unsigned long time = millis();
	Serial.println("Sending message..");
	while (!success) {
		Serial.println("Attempt started..");
		// send the message and wait until we have a message available
		do {
			Serial.print(millis());
			Serial.println("message sent");
			nrf24.send(str, len);
			delay(100);
			if (millis() > time + 5000L && millis() >= time) {
				Serial.println("5 second timeout");
				return false;
			}
		} while (!nrf24.available());

		uint8_t* buf;
		uint8_t blen;

		if (!nrf24.recv(buf, &blen)) {
			continue;
		}

		Serial.print("Received response: ");
		Serial.println((char*) buf);

		// if the received message is not our message, repeat our message
		if (len != blen) {
			continue;
		}
		if (!compareBuffers(buf, str, len)) {
			continue;
		}

		time = millis();

		// success! notify the other party that they can stop confirming the message and timeout after a second
		do {
			Serial.println("Attempting to confirm...");
			nrf24.send((uint8_t*) "OKSTOP", 7);
			delay(100);
			if (millis() > time + 1000L && millis() >= time) {
				Serial.println("1 second timeout");
				break;
			}
		} while (!nrf24.available());

		// empty received message buffer if we got one
		if (nrf24.available()) {
			nrf24.recv(buf, &blen);
		}

		success = true;
		Serial.println("Message succesfully sent");
	}

	return true;
}