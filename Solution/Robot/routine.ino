void RPMtester(Stepper &driver)
{
	int testArray[] = {5,60, 90, 120, 180, 300 };	
	for (int i = 0; i < sizeof(testArray); i++)
	{
		driver.setSpeed(testArray[i]);
		unsigned long currentMillis = millis();
		unsigned long previousMillis = currentMillis;
		while (currentMillis - previousMillis <= (60000 / testArray[i]))
		{
			currentMillis = millis();
			driver.step(stepsPerRevolution / 100);
		}
		delay(100);
		driver.setSpeed(testArray[i]);
		driver.step(-stepsPerRevolution);
		previousMillis = currentMillis;
		delay(100);
	}
}
