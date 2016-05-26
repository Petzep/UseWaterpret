/*
 Name:		Robot.ino
 Created:	5/19/2016 4:07:57 PM
 Author:	Use Waterpret
*/

#define Servo ServoTimer2
bool DEBUG = true;
bool FEEDBACK = true;

#include <ServoTimer2.h>							// Include servo library
#include <AccelStepper.h>
#include <SPI.h>
#include <RH_NRF24.h>
#include <OneWire\OneWire.h>
#include <DallasTemperatureControl\DallasTemperature.h>

void RPMtester(AccelStepper driver);				// Small RPM that tests the Acceleration.
OneWire oneWire(A0);								// Setup a oneWire instance to communicate with any OneWire devices 
DallasTemperature sensors(&oneWire);				// Pass the oneWire reference to Dallas Temperature.
RH_NRF24 nrf24;										// Singleton instance of the radio driver.
AccelStepper myStepper(AccelStepper::FULL4WIRE, 2, 4, 6, 7, FALSE);	// initialize the stepper library on pins 2,4,6,7 and disable the output;
Servo servoArm;										// Arm servo signal
Servo servoGrab;									// Grabbbing Servo
const int delayRest = 100;							// Standard delay for momentum to stablelize
const int armNeutralPosition = 90;					// Neutral position of the grabber
const int grabberNeutralPosition = 60;				// Ground position of the grabber
const int grabberGrabPosition = 0;					// Position for closed grabbers
const int armOffset = 0;							// turn ofset for the head in degrees
const int stepsPerRevolution = 200;					// change this to fit the number of steps per revolution for your motor
const float pi = 3.141592654;						// this one is a piece of cake
			
int stepCount = 0;									// number of steps the motor has taken
int motorSpeed = 0;									// Speed of the motor in RPM
int motorDirection = 1;								// direction of the motor
unsigned long previousMillis = 0;					// will store last time LED was updatedvccv
float rpm2steps = stepsPerRevolution / 60.0f;

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
	Serial.println("Booting Ariel: Commencing setup");
	Serial.println(" -Starting Ariel\nSerial port is open @9600.");

	Serial.print(" -Initiating radio:\t");
	if (!nrf24.init())								// Defaults after init are 2.402 GHz (channel 2), 2Mbps, 0dBm
		Serial.println("init failed");
	else
		Serial.println("OK");

	Serial.print(" -Setting radio channel:\t");
	if (!nrf24.setChannel(1))
		Serial.println("setChannel failed");
	else
		Serial.println("Channel 1");
	Serial.print(" -Setting radio settings:\t");
	if (!nrf24.setRF(RH_NRF24::DataRate2Mbps, RH_NRF24::TransmitPower0dBm))
		Serial.println("setRF failed");
	else
		Serial.println("2Mbps, 0dBm");

	Serial.print(" -Attaching Arm Servo:\t");
	servoArm.attach(3);								// Attach Arm signal to the pin
	if (!servoArm.attached())
		Serial.println("servoArm attach failed");
	else
		Serial.println("OK");
	servoGrab.attach(5);							// Attach Grab signal to the pin
	Serial.print(" -Attaching Arm Servo:\t");
	if (!servoGrab.attached())
		Serial.println("servoGrab attach failed");
	else
		Serial.println("OK");


	sensors.begin();								// Initialise the temperaturesensor bus

	myStepper.setMaxSpeed(300 * rpm2steps);
	Serial.println(" -Max speed:\t");
	Serial.println(myStepper.maxSpeed());

	Serial.println("Ariel has started");
}


///////////////////////////////
//////////MAIN LOOP////////////
///////////////////////////////
void loop()
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
		Serial.println("turning one round @60RPM: ");
		myStepper.stop();
		myStepper.setSpeed(60 * rpm2steps);
		myStepper.move(stepsPerRevolution);
		myStepper.runToPosition();
		break;
	}
	case 'g':
	{
		r_Grab();
		break;
	}
	case 'r':
	{
		RPMtester(myStepper);
		break;
	}
	case '*':
	{
		sensors.requestTemperatures(); // Send the command to get temperatures
		Serial.print("Temperature for Device 1 is: ");
		Serial.println(sensors.getTempCByIndex(0));
		break;
	}
	case '/':
	{
		Serial.println("Swithcing motor direction");
		motorDirection *= -1;
		break;
	}
	}
	// set the motor speed in RPM:
	if (motorSpeed > 0) {
		myStepper.setSpeed(motorDirection * motorSpeed * rpm2steps);
		myStepper.runSpeed();			//Run motor at set speed.		
	}
	else
		myStepper.disableOutputs();
}


