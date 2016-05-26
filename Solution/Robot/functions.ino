////////////////////////////////////////
//////ARM AND GRABBER FUNCTIONS/////////
////////////////////////////////////////
void armNeutral()
{
	if (FEEDBACK)
		Serial.println("Arm to neutral position");
	servoArm.write(armNeutralPosition);								//neutral position
	armPos = armNeutralPosition;
	delay(100);
}

void grabberNeutral()
{
	if (FEEDBACK)
		Serial.println("Grabber to neutral position");
	servoGrab.write(grabberNeutralPosition);						//neutral position
	grabPos = grabberNeutralPosition;
	delay(200);
}

void grabberGrab()
{
	if (FEEDBACK)
		Serial.println("Grabbing");
	servoGrab.write(180);
	delay(1000);
	servoGrab.write(grabberGrabPosition);							//Grab (and move up arm?)
	delay(1000);
}

void grabberTurnPos(int pos)
{
	if (FEEDBACK)
	{
		Serial.print("Grabber to position ");
		Serial.println(pos);
	}
	servoGrab.write(pos);											//servo position
	delay(100);
}

void armTurnPos(int pos)
{
	if (FEEDBACK)
	{
		Serial.print("Arm to position ");
		Serial.println(pos);
	}
	servoArm.write(pos);											//servo position
	delay(100);
}

void resetServos()
{
	if (FEEDBACK)
		Serial.println("Resetting servo");
	delay(80);
	servoGrab.write(grabPos + 1);
	servoArm.write(armPos + 1);
	delay(20);
	servoGrab.write(grabPos);
	servoArm.write(armPos);
}

void leBlink(int ledPin, int n)
{
	for (int i = 0; i < n; i++)
	{
		digitalWrite(ledPin, HIGH);		// turn the LED on (HIGH is the voltage level)
		delay(100);						// wait for a second
		digitalWrite(ledPin, LOW);		// turn the LED off by making the voltage LOW
		delay(100);						// wait for a second
	}
}

void sentAndWait(int delay)
{
	Serial.println("Sending to nrf24_server");
	// Send a message to nrf24_server
	uint8_t data[] = "Hello World!";
	uint8_t check[] = "And hello back to you";
	nrf24.send(data, sizeof(data));

	nrf24.waitPacketSent();
	// Now wait for a reply
	uint8_t buf[RH_NRF24_MAX_MESSAGE_LEN];
	uint8_t len = sizeof(buf);

	if (nrf24.waitAvailableTimeout(delay))
	{
		// Should be a reply message for us now   
		if (nrf24.recv(buf, &len))
		{
			Serial.print("got reply: ");
			Serial.println((char*)buf);
			motorDirection *= -1;
		}
		else
		{
			Serial.println("recv failed");
		}
	}
	resetServos();
}