void r_Cut()
{
	grabberGrab();
	for (int i = 0; i < 20; i++)
	{
		armTurnPos(armPos + 2);
		delay(25);
		armTurnPos(armPos - 2);
		delay(25);
	}
	delay(500);
	grabberNeutral();
	armTurnPos(armPos);
}

void r_Grab()
{
	grabberGrab();
	grabberNeutral();
	armTurnPos(armPos);
}

void r_RPMtester(AccelStepper driver)
{
	Serial.println(F("Testing RPM:"));
	Serial.print(F(" -Max speed: "));
	Serial.println(driver.maxSpeed());
	for (int i = 100; i < 500; i += 50)
	{
		Serial.print(F(" -Acceleration: "));
		Serial.println(i);
		driver.setAcceleration(i);
		driver.move(500 * stepsPerRevolution);
		while (driver.distanceToGo())
			driver.run();

		Serial.println(F("Starting next iteration in 1 second"));
		delay(1000);
	}
	Serial.println(F("Nope....Test is over. Resuming in 3 seconds"));
	delay(3000);
}

void r_DirectionTest()
{
	Serial.println(F("Testing Drirectoion:"));
	Serial.println(F("------------------"));
	Serial.println(F("Arm motor +"));
	armStepper.move(50);
	armStepper.runToPosition();
	armStepper.move(-50);
	armStepper.runToPosition();
	armStepper.disableOutputs();
	Serial.println(F("Shaft motor +"));
	shaftStepper.move(50);
	shaftStepper.runToPosition();
	shaftStepper.move(-50);
	shaftStepper.runToPosition();
	shaftStepper.disableOutputs();
	Serial.println(F("Kop motor +"));
	kopStepper.move(50);
	kopStepper.runToPosition();
	kopStepper.move(-50);
	kopStepper.runToPosition();
	kopStepper.disableOutputs();
}

void r_MovePlant(int plant)
{
	kopStepper.moveTo(200*plant);
	kopStepper.runToPosition();
}