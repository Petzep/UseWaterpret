void r_Grab()
{
	grabberGrab();
	armTurnPos(0);
	delay(1000);
	grabberTurnPos(180);
	delay(1000);
	grabberNeutral();
	armNeutral();
}

void RPMtester(AccelStepper driver)
{
	Serial.println("Testing RPM:");
	Serial.print(" -Max speed: ");
	Serial.println(driver.maxSpeed());
	for (int i = 100; i < 500; i += 50)
	{
		Serial.print(" -Acceleration: ");
		Serial.println(i);
		driver.setAcceleration(i);
		driver.move(500 * stepsPerRevolution);
		while (driver.distanceToGo())
			driver.run();

		Serial.println("Starting next iteration in 1 second");
		delay(1000);
	}
	Serial.println("Nope....Test is over. Resuming in 3 seconds");
	delay(3000);
}