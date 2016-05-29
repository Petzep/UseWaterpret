/*
 Name:		Robot.ino
 Created:	5/19/2016 4:07:57 PM
 Author:	Use Waterpret
*/

#define Servo ServoTimer2
#define DEBUG true
#define FEEDBACK true

#include <ServoTimer2.h>							// Include servo library
#include <Stepper.h>
#include <SPI.h>
#include <OneWire\OneWire.h>
#include <DallasTemperatureControl\DallasTemperature.h>

void RPMtester(Stepper &driver);
// Setup a oneWire instance to communicate with any OneWire devices 
// (not just Maxim/Dallas temperature ICs)
OneWire oneWire(A0);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

Servo servoArm;										// Arm servo signal
Servo servoGrab;									// Grabbbing 
const int delayRest = 100;							// Standard delay for momentum to stablelize
const int armNeutralPosition = 90;					// Neutral position of the grabber
const int grabberNeutralPosition = 60;				// Ground position of the grabber
const int grabberGrabPosition = 0;					// Position for closed grabbers
const int armOffset = 0;							// turn ofset for the head in degrees
const float pi = 3.141592654;						// this one is a piece of cake
const int stepsPerRevolution = 200;					// change this to fit the number of steps per revolution for your motor
Stepper myStepper(stepsPerRevolution, 2, 4, 6, 7);	// initialize the stepper library on pins 8 through 11:
			
int stepCount = 0;									// number of steps the motor has taken
int motorSpeed = 0;									// Speed of the motor in RPM
int motorDirection = 1;								// direction of the motor
unsigned long previousMillis = 0;					// will store last time LED was updatedvccv

/////////////////////////////////
///USER DETERMINED VARIABLES/////
/////////////////////////////////
int dist_Collision = 40;
int dist_Edge = 10;
int armPos = armNeutralPosition;
int grabPos = grabberNeutralPosition;
int remoteStep = 5;

void setup()										// Built in initialization block
{
	Serial.begin(9600);								// open the serial port at 9600 bps:
	nrf24Initialize();
	
	servoArm.attach(3);								// Attach Arm signal to the pin
	servoGrab.attach(5);							// Attach Grab signal to the pin

	// Start up the library
	sensors.begin();
}

void loop() {

	uint8_t data[256];
	uint8_t len;
	while (!nrf24ReceiveMessage(data, &len)) {}
	Serial.println((char*)data);

	delay(1000);
}


///////////////////////////////
//////////MAIN LOOP////////////
///////////////////////////////
void loop2()
{
	/*unsigned long currentMillis = millis();

	if (currentMillis - previousMillis >= 5000) {
		previousMillis = currentMillis;
		sentAndWait(500);
	}*/

	int  c = Serial.read();
	switch (c)
	{
	case '-':
	{
		remoteStep--;
		if (remoteStep == 0)
			remoteStep = 1;
		Serial.print("Stepsize: ");
		Serial.println(remoteStep);
		break;
	}
	case '+':
	{
		remoteStep++;
		Serial.print("Stepsize: ");
		Serial.println(remoteStep);
		break;
	}
	case '7':
	{
		motorSpeed -= remoteStep;
		if (motorSpeed < 0)
			motorSpeed = 0;
		Serial.print("Motorspeed: ");
		Serial.println(motorSpeed);
		break;
	}
	case '9':
	{
		motorSpeed += remoteStep;
		Serial.print("Motorspeed: ");
		Serial.println(motorSpeed);
		break;
	}
	case '0':
	{
		motorSpeed = 0;
		Serial.print("Motorspeed: ");
		Serial.println(motorSpeed);
		grabberNeutral();
		armNeutral();
		break;
	}
	case '4':
	{
		grabPos += remoteStep;
		grabberTurnPos(grabPos);
		break;
	}
	case '6':
	{
		grabPos -= remoteStep;
		grabberTurnPos(grabPos);
		break;
	}
	case '8':
	{
		Serial.print("arm offset: ");
		Serial.print(armOffset);
		Serial.print("  ");
		armPos += remoteStep;
		armTurnPos(armPos);
		break;
	}
	case '2':
	{
		Serial.print("arm offset: ");
		Serial.print(armOffset);
		Serial.print("  ");
		armPos -= remoteStep;
		armTurnPos(armPos);
		break;
	}
	case '5':
	{
		Serial.println("turning one round: ");
		myStepper.setSpeed(60);
		myStepper.step(stepsPerRevolution);
		break;
	}
	case 'r':
	{
		Serial.println("testing RPM: ");
		myStepper.setSpeed(60);
		myStepper.step(stepsPerRevolution);
		//RPMtester(myStepper);
		break;
	}
	case '*':
	{
		sensors.requestTemperatures(); // Send the command to get temperatures
		Serial.print("Temperature for Device 1 is: ");
		Serial.println(sensors.getTempCByIndex(0));
		break;
	}
	}
	// set the motor speed in RPM:
	if (motorSpeed > 0) {
		myStepper.setSpeed(motorSpeed);
		// step 1/100 of a revolution:
		myStepper.step(motorDirection*stepsPerRevolution / 100);
	}
}


