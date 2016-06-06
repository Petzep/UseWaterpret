////////////////////////////////////////
//////ARM AND GRABBER FUNCTIONS/////////
////////////////////////////////////////
void armNeutral()
{
	debugln("Arm to neutral position");

	servoArm.write(armNeutralPosition);								//neutral position
	armPos = armNeutralPosition;
	delay(100);
}

void grabberNeutral()
{
	debugln("Grabber to neutral position");

	servoGrab.write(grabberNeutralPosition);						//neutral position
	grabPos = grabberNeutralPosition;
	delay(200);
}

void grabberGrab()
{
	debugln("Grabbing");

	servoGrab.write(180);
	delay(1000);
	servoGrab.write(grabberGrabPosition);							//Grab (and move up arm?)
	delay(1000);
}

void grabberTurnPos(int pos)
{
	debug("Grabber to position ");
	debugln(pos);

	servoGrab.write(pos);											//servo position
	delay(100);
}

void armTurnPos(int pos)
{
	debug("Arm to position ");
	debugln(pos);

	servoArm.write(pos);											//servo position
	delay(100);
}

void resetServos()
{
	debugln("Resetting servo");

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

/*not used
void setTemperatureAlarm(int temp)
{
	sensors.setLowAlarmTemp(motor1Temp, -10);
	sensors.setHighAlarmTemp(motor1Temp, temp);
}

void checkAlarm()
{
	if (sensors.hasAlarm())
	{
		Serial.println("ALARM ON MOTOR 1");
	}
}*/