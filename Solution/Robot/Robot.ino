/*
Name:		Robot.ino
Created:	5/19/2016 4:07:57 PM
Author:	Use Waterpret
*/

#define Servo ServoTimer2
//#define ENABLE_DEBUG
//#define REMOTE
#define BOOKSIZE 20

struct CommandBook
{
	char command;
	int value;
};

#ifdef ENABLE_DEBUG
// disable Serial output
#define debugln(a) (Serial.println(a))
#define debug(a) (Serial.print(a))
#else
#define debugln(a)
#define debug(a)
#endif

#include <Arduino.h>								// Fixes some define problems
#include <ServoTimer2.h>							// Include servo library
#include <AccelStepper.h>							// AccelStepper library for precise and fluid steppercontrol
#include <SPI.h>									// The SPI-communication liberary for the antenna
#include <OneWire\OneWire.h>						// The OneWire-protocol used by te temp sensor
#include <DallasTemperatureControl\DallasTemperature.h>	// DallasTemperature library for controlling the DS18S20 temperature monitor


OneWire oneWire(A0);								// Setup a oneWire instance to communicate with any OneWire devices 
DallasTemperature sensors(&oneWire);				// Pass the oneWire reference to Dallas Temperature.							
AccelStepper shaftStepper(AccelStepper::FULL4WIRE, 13, 12, 11, 9, TRUE);// initialize the stepper library on pins 13,12,11,9 and disable the output;
AccelStepper kopStepper(AccelStepper::FULL4WIRE, 2, 4, 6, 7, FALSE);	// initialize the stepper library on pins 2,4,6,7 and disable the output;
AccelStepper armStepper(AccelStepper::FULL4WIRE, A2, A3, A4, A5, FALSE);// initialize the stepper library on pins A2,A3,A4,A5 and disable the output;
CommandBook commandos[BOOKSIZE] = {};				// array with commands for the Arduino

DeviceAddress motorShaftTemp;						// arrays to hold device addresses of the TempSensors
Servo servoArm;										// Arm servo signal
Servo servoGrab;									// Grabbbing Servo
const int delayRest = 100;							// Standard delay for momentum to stablelize
const int armNeutralPosition = 90;					// Neutral position of the grabber
const int grabberNeutralPosition = 60;				// Ground position of the grabber
const int grabberGrabPosition = 0;					// Position for closed grabbers
const int armOffset = 0;							// turn ofset for the head in degrees
const int stepsPerRevolution = 200;					// change this to fit the number of steps per revolution for your motor
const int acceleration = 350;						// Acceleration
const float pi = 3.141592654;						// this one is a piece of cake

int kopSpeed = 0;									// Speed of the kopmotor in RPM
char motorDirection = 1;							// direction of the motor
unsigned long previousMillis = 0;					// will store last time LED was updatedvccv
float rpm2steps = stepsPerRevolution / 60.0f;

uint8_t* buf = new uint8_t[32];
uint8_t len = 32;

void r_RPMtester(AccelStepper driver);				// Small acceleration test.
void r_DirectionTest();								// Small direction test.
bool addCommand(CommandBook *book, char command, int value);
int countCommand(CommandBook *book);
void runCommand(CommandBook *book);
void printCommand(CommandBook *book);
bool deleteCommand(CommandBook *book);
bool sendCommand(CommandBook *book);
/////////////////////////////////
///USER DETERMINED VARIABLES/////
/////////////////////////////////
int armPos = armNeutralPosition;
int grabPos = grabberNeutralPosition;
int remoteStep = 5;

void setup()										// Built in initialization block
{
	Serial.begin(9600);								// open the serial port at 9600 bps:
	Serial.println(F("Booting Ariel:"));
	Serial.println(F(" -Starting Ariel"));
	Serial.println(F(" -Serial port is open @9600."));

	nrf24Initialize(false);								// Radio initialisation function

	Serial.print(F(" -Attaching Arm Servo:\t"));
	servoArm.attach(5);								// Attach Arm signal to the pin
	if (!servoArm.attached())
		Serial.println(F("servoArm attach failed"));
	else
		Serial.println(F("OK"));
	servoGrab.attach(3);							// Attach Grab signal to the pin
	Serial.print(F(" -Attaching Grab Servo:\t"));
	if (!servoGrab.attached())
		Serial.println(F("servoGrab attach failed"));
	else
		Serial.println(F("OK"));

	sensors.begin();								// Initialise the temperaturesensor bus
	Serial.print(F("Gettnig motorShaftTemp address\t"));
	if (!sensors.getAddress(motorShaftTemp, 0))			// Check if the temperaturesensors are connected.
		Serial.println(F("Unable to find address for motorShaftTemp"));
	else
		Serial.println(F("OK"));

	Serial.print(F(" -Connecting motorShaftTemp:\t"));
	if (!sensors.isConnected(motorShaftTemp))
		Serial.println(F("motorShaftTemp is not connected"));
	else
		Serial.println(F("OK"));


	Serial.println(F("Attaching armStepper:\t"));
	armStepper.setMaxSpeed(220 * rpm2steps);
	Serial.print(F(" -Max speed:\t"));
	Serial.println(armStepper.maxSpeed());
	armStepper.setAcceleration(acceleration);
	Serial.print(F(" -Acceleration:\t"));
	Serial.println(acceleration);
	Serial.print(F(" -Position:\t"));
	Serial.println(armStepper.currentPosition());
	Serial.println(F("Attaching kopStepper:\t"));
	kopStepper.setMaxSpeed(180 * rpm2steps);
	Serial.print(F(" -Max speed:\t"));
	Serial.println(kopStepper.maxSpeed());
	kopStepper.setAcceleration(acceleration);
	Serial.print(F(" -Acceleration:\t"));
	Serial.println(acceleration);
	Serial.print(F(" -Position:\t"));
	Serial.println(kopStepper.currentPosition());
	Serial.println(F("Attaching shaftStepper:\t"));
	shaftStepper.setMaxSpeed(220 * rpm2steps);
	Serial.print(F(" -Max speed:\t"));
	Serial.println(shaftStepper.maxSpeed());
	shaftStepper.setAcceleration(acceleration);
	Serial.print(F(" -Acceleration:\t"));
	Serial.println(acceleration);
	Serial.print(F(" -Position:\t"));
	Serial.println(shaftStepper.currentPosition());

	Serial.println(F("- SPI diabled"));
	SPI.end();

	Serial.println(F("Ariel has started"));
}

///////////////////////////////
//////////MAIN LOOP////////////
//////////////////////////////
#ifdef REMOTE
void loop() {
	SPI.begin();
	sensors.requestTemperatures(); // Send the command to get temperatures
	Serial.print(F("Temperature for the Steppermotor is: "));
	Serial.println(sensors.getTempC(motorShaftTemp));
	if (nrf24ReceiveMessage(buf, &len))
	{
		Serial.println(F("Message received"));

		int c = countCommand(commandos);
		for (int n = 0; n < c; n += 1)
		{
			commandos[n].command = NULL;
			commandos[n].value = NULL;
		}
		c = buf[0];
		Serial.println("Received " + String(c) + " commands");
		for (int n = 0; n < c; n += 1)
		{
			addCommand(commandos, (char)buf[n * 3 + 1], (((int)buf[n * 3 + 2]) << 8) + (int)buf[n * 3 + 3]);
		}

		//digitalWrite(11, LOW);
		//digitalWrite(8, HIGH);
		//radioSleep();
		//SPI.transfer(0xAA);
		//SPI.setDataMode(SPI_MODE0);
		//delay(2000);
		SPI.endTransaction();
		SPI.end();
		shaftStepper.disableOutputs();
		Serial.println(digitalRead(9));
		Serial.println(digitalRead(11));
		Serial.println(digitalRead(12));
		Serial.println(digitalRead(12));
		shaftStepper.enableOutputs();


		Serial.println(F("Received commands!"));
		Serial.println();
		printCommand(commandos);
		Serial.println(F("Waiting 3 seconds before executing"));
		delay(3000);

		runCommand(commandos);

		delay(1000);
		Serial.println();
		sensors.requestTemperatures(); // Send the command to get temperatures
		Serial.print(F("Temperature for the Steppermotor is: "));
		Serial.println(sensors.getTempC(motorShaftTemp));
		Serial.println("DONE!");
	}
}
#else
void loop()
{
	int  c = Serial.read();
	switch (c)
	{
	case '-':
	{
		remoteStep--;
		if (remoteStep == 0)
			remoteStep = 1;
		Serial.print(F("Stepsize: "));
		Serial.println(remoteStep);
		break;
	}
	case '+':
	{
		remoteStep++;
		Serial.print(F("Stepsize: "));
		Serial.println(remoteStep);
		break;
	}
	case '(':
	{
		kopSpeed -= remoteStep;
		if (kopSpeed < 0)
			kopSpeed = 0;
		Serial.print(F("kopSpeed: "));
		Serial.println(kopSpeed);
		break;
	}
	case ')':
	{
		kopSpeed += remoteStep;
		if (kopSpeed > kopStepper.maxSpeed())
			kopSpeed = kopStepper.maxSpeed();
		Serial.print(F("kopSpeed: "));
		Serial.println(kopSpeed);
		break;
	}
	case '0':
	{
		Serial.println(F("All stop!"));
		SPI.end();
		kopStepper.stop();
		while (kopStepper.distanceToGo())
			kopStepper.run();
		kopSpeed = 0;
		grabberNeutral();
		armNeutral();
		kopStepper.disableOutputs();
		armStepper.disableOutputs();
		shaftStepper.disableOutputs();
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
		debug(F("arm offset: "));
		debug(armOffset);
		armPos += remoteStep;
		armTurnPos(armPos);
		break;
	}
	case '2':
	{
		debug(F("arm offset: "));
		debug(armOffset);
		armPos -= remoteStep;
		armTurnPos(armPos);
		break;
	}
	case '5':
	{
		Serial.println(F("turning one round @60RPM: "));
		kopStepper.setSpeed(60 * rpm2steps);
		kopStepper.move(stepsPerRevolution);
		kopStepper.runSpeedToPosition();
		break;
	}
	case 'c':
	{
		r_Cut();
		break;
	}
	case 'r':
	{
		r_RPMtester(kopStepper);
		break;
	}
	case 'd':
	{
		r_DirectionTest();
		break;
	}
	case '*':
	{
		sensors.requestTemperatures(); // Send the command to get temperatures
		Serial.print(F("Temperature for the Steppermotor is: "));
		Serial.println(sensors.getTempC(motorShaftTemp));
		break;
	}
	case '.':
	{
		Serial.println(F("Locking the motor for 1 second"));
		kopStepper.setSpeed(0);
		kopStepper.move(1);
		kopStepper.move(-1);
		unsigned long previousMillis = millis();
		while (millis() - previousMillis <= 1000)
			kopStepper.runSpeed();
		kopStepper.disableOutputs();
		break;
	}
	case '/':
	{
		Serial.println(F("Swithcing motor direction"));
		motorDirection *= -1;
		break;
	case '[':
	{
		Serial.println(F("Reseting arm motor position to 0"));
		armStepper.setCurrentPosition(armStepper.currentPosition());
		break;
	}
	case ']':
	{
		Serial.print(F("Current arm motor position: "));
		Serial.println(armStepper.currentPosition());
		break;
	}
	case '{':
	{
		Serial.println(F("Reseting shaft motor position to 0"));
		shaftStepper.setCurrentPosition(shaftStepper.currentPosition());
		break;
	}
	case '}':
	{
		Serial.print(F("Current shaft motor position: "));
		Serial.println(shaftStepper.currentPosition());
		break;
	}
	case '9':
	{
		Serial.println(F("Extending shaft"));
		shaftStepper.moveTo(900);
		shaftStepper.runToPosition();
		shaftStepper.disableOutputs();
		break;
	}
	case '7':
	{
		Serial.println(F("Retracting shaft"));
		shaftStepper.moveTo(0);
		shaftStepper.runToPosition();
		shaftStepper.disableOutputs();
		break;
	}
	case '3':
	{
		Serial.println(F("Extending arm "));
		armStepper.moveTo(750);
		armStepper.runToPosition();
		armStepper.disableOutputs();
		break;
	}
	case '1':
	{
		Serial.println(F("Retracting arm "));
		armStepper.moveTo(0);
		armStepper.runToPosition();
		armStepper.disableOutputs();
		break;
	}
	case 't':
	{
		Serial.println("Testing command ");
		addCommand(commandos, 't', 1);
		addCommand(commandos, 'W', 0);
		addCommand(commandos, 'D', 0);
		addCommand(commandos, 'c', 0);
		addCommand(commandos, 'A', 0);
		addCommand(commandos, 'S', 0);
		addCommand(commandos, 't', 2);
		addCommand(commandos, 'W', 0);
		addCommand(commandos, 'D', 0);
		addCommand(commandos, 'i', 0);
		addCommand(commandos, 'A', 0);
		addCommand(commandos, 'S', 0);
		addCommand(commandos, 't', 3);
		addCommand(commandos, 'W', 0);
		addCommand(commandos, 'D', 0);
		addCommand(commandos, 'i', 180);
		addCommand(commandos, 'A', 0);
		addCommand(commandos, 'S', 0);
		addCommand(commandos, 't', 0);
		delay(1000);
		printCommand(commandos);
		delay(1000);
		runCommand(commandos);
		break;
	}
	case '<':
	{
		Serial.println(F("Spooling up kopStepper"));
		kopStepper.move(-2000);
		while (kopStepper.distanceToGo() < -1000)
			kopStepper.run();
		motorDirection = -1;
		kopSpeed = -kopStepper.maxSpeed();
		break;
	}
	case '>':
	{
		Serial.println(F("Spooling up kopStepper"));
		kopStepper.move(2000);
		while (kopStepper.distanceToGo() > 1000)
			kopStepper.run();
		motorDirection = 1;
		kopSpeed = kopStepper.maxSpeed();
		break;
	}
	}
	}
	// set the motor speed in RPM:
	if (kopSpeed) {
		kopStepper.setSpeed(motorDirection * kopSpeed * rpm2steps);
		kopStepper.runSpeed();			//Run motor at set speed.		
	}
	else
		kopStepper.disableOutputs();
}
#endif // REMOTE

