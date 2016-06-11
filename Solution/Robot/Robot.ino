/*
Name:		Robot.ino
Created:	5/19/2016 4:07:57 PM
Author:	Use Waterpret
*/

#define Servo ServoTimer2
//#define ENABLE_DEBUG
#define BOOKSIZE 10
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

void RPMtester(AccelStepper driver);				// Small RPM that tests the Acceleration.
bool addCommand(CommandBook *book, char command, int value);
int countCommand(CommandBook *book);
void runCommand(CommandBook *book);
void printCommand(CommandBook *book);
bool deleteCommand(CommandBook *book);
bool sendCommand(CommandBook *book);
OneWire oneWire(A0);								// Setup a oneWire instance to communicate with any OneWire devices 
DallasTemperature sensors(&oneWire);				// Pass the oneWire reference to Dallas Temperature.							
AccelStepper myStepper(AccelStepper::FULL4WIRE, A2, A3, A4, A5, FALSE);	// initialize the stepper library on pins 2,4,6,7 and disable the output;
AccelStepper kopStepper(AccelStepper::FULL4WIRE, 2, 4, 6, 7, FALSE);	// initialize the stepper library on pins 2,4,6,7 and disable the output;

DeviceAddress motor1Temp;							// arrays to hold device addresses of the TempSensors
CommandBook commandos[BOOKSIZE] = {};				// array with commands for the Arduino
Servo servoArm;										// Arm servo signal
Servo servoGrab;									// Grabbbing Servo979990
const int delayRest = 100;							// Standard delay for momentum to stablelize
const int armNeutralPosition = 90;					// Neutral position of the grabber
const int grabberNeutralPosition = 60;				// Ground position of the grabber
const int grabberGrabPosition = 0;					// Position for closed grabbers
const int armOffset = 0;							// turn ofset for the head in degrees
const int stepsPerRevolution = 200;					// change this to fit the number of steps per revolution for your motor
const int acceleration = 350;						// Acceleration
const float pi = 3.141592654;						// this one is a piece of cake

int stepCount = 0;									// number of steps the motor has taken
int motorSpeed = 0;									// Speed of the motor in RPM
int kopSpeed = 0;									// Speed of the kopmotor in RPM
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
	Serial.println(F("Booting Ariel:"));
	Serial.println(F(" -Starting Ariel"));
	Serial.println(F(" -Serial port is open @9600."));

	nrf24Initialize(false);								// Radio initialisation function

	Serial.print(F(" -Attaching Arm Servo:\t"));
	servoArm.attach(3);								// Attach Arm signal to the pin
	if (!servoArm.attached())
		Serial.println(F("servoArm attach failed"));
	else
		Serial.println(F("OK"));
	servoGrab.attach(5);							// Attach Grab signal to the pin
	Serial.print(F(" -Attaching Arm Servo:\t"));
	if (!servoGrab.attached())
		Serial.println(F("servoGrab attach failed"));
	else
		Serial.println(F("OK"));

	sensors.begin();								// Initialise the temperaturesensor bus
	if (!sensors.getAddress(motor1Temp, 0))			// Check if the temperaturesensors are connected.
		Serial.println(F("Unable to find address for motor1Temp"));

	Serial.print(F(" -Connecting motor1Temp:\t"));
	if (!sensors.isConnected(motor1Temp))
		Serial.println(F("motor1Temp is not connected"));
	else
		Serial.println(F("OK"));


	Serial.println(F("Attaching myStepper:\t"));
	myStepper.setMaxSpeed(220 * rpm2steps);
	Serial.print(F(" -Max speed:\t"));
	Serial.println(myStepper.maxSpeed());
	myStepper.setAcceleration(350);
	Serial.print(F(" -Acceleration:\t"));
	Serial.println(acceleration);
	Serial.println(F("Attaching kopStepper:\t"));
	kopStepper.setMaxSpeed(220 * rpm2steps);
	Serial.print(F(" -Max speed:\t"));
	Serial.println(kopStepper.maxSpeed());
	kopStepper.setAcceleration(350);
	Serial.print(F(" -Acceleration:\t"));
	Serial.println(acceleration);

	Serial.println(F("Ariel has started"));
}

uint8_t* buf = new uint8_t[32];
uint8_t len = 32;

void loop() {
	if (nrf24ReceiveMessage(buf, &len)) {
		Serial.println("Message received");

		int c = countCommand(commandos);
		for (int n = 0; n < c; n += 1) {
			commandos[n].command = NULL;
			commandos[n].value = NULL;
		}
		c = buf[0];
		Serial.println("Received " + String(c) + " commands");
		for (int n = 0; n < c; n += 1) {
			addCommand(commandos, (char)buf[n * 3 + 1], (((int)buf[n * 3 + 2]) << 8) + (int)buf[n * 3 + 3]);
		}
		Serial.println(F("Received commands!"));
		Serial.println();
		printCommand(commandos);

		runCommand(commandos);

		delay(1000);
		Serial.println();
		Serial.println("DONE!");
		Serial.println();
	}
}

///////////////////////////////
//////////MAIN LOOP////////////
///////////////////////////////
void loop2()
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
	case '7':
	{
		motorSpeed -= remoteStep;
		if (motorSpeed < 0)
			motorSpeed = 0;
		Serial.print(F("Motorspeed: "));
		Serial.println(motorSpeed);
		break;
	}
	case '9':
	{
		motorSpeed += remoteStep;
		if (motorSpeed > myStepper.maxSpeed())
			motorSpeed = myStepper.maxSpeed();
		Serial.print(F("Motorspeed: "));
		Serial.println(motorSpeed);
		break;
	}
	case '0':
	{
		kopStepper.stop();
		while (kopStepper.distanceToGo())
			kopStepper.run();
		kopSpeed = 0;
		motorSpeed = 0;
		Serial.print(F("Motorspeed: "));
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
		Serial.print(F("arm offset: "));
		Serial.println(armOffset);
		armPos += remoteStep;
		armTurnPos(armPos);
		break;
	}
	case '2':
	{
		Serial.print(F("arm offset: "));
		Serial.println(armOffset);
		armPos -= remoteStep;
		armTurnPos(armPos);
		break;
	}
	case '5':
	{
		Serial.println(F("turning one round @60RPM: "));
		myStepper.setSpeed(60 * rpm2steps);
		myStepper.move(stepsPerRevolution);
		myStepper.runSpeedToPosition();
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
		Serial.print(F("Temperature for the Steppermotor is: "));
		Serial.println(sensors.getTempC(motor1Temp));
		break;
	}
	case '.':
	{
		Serial.println(F("Locking the motor for 1 second"));
		myStepper.setSpeed(0);
		myStepper.move(1);
		myStepper.move(-1);
		unsigned long previousMillis = millis();
		while (millis() - previousMillis <= 1000)
			myStepper.runSpeed();
		myStepper.disableOutputs();
		break;
	}
	case '/':
	{
		Serial.println(F("Swithcing motor direction"));
		motorDirection *= -1;
		break;
	case '[':
	{
		Serial.println(F("Reseting motor position to 0"));
		myStepper.setCurrentPosition(myStepper.currentPosition());
		break;
	}
	case ']':
	{
		Serial.print(F("Current motor position: "));
		Serial.println(myStepper.currentPosition());
		break;
	}
	case '3':
	{
		Serial.println(F("Extending arm "));
		myStepper.move(-1050);
		while (myStepper.distanceToGo())
			myStepper.run();
		break;
	}
	case '1':
	{
		Serial.println(F("Retracting arm "));
		myStepper.move(1050);
		while (myStepper.distanceToGo())
			myStepper.run();
		break;
	}
	case 't':
	{
		Serial.println("Testing command ");
		addCommand(commandos, 'm', 200);
		addCommand(commandos, 'd', 1000);
		addCommand(commandos, 'm', -400);
		addCommand(commandos, 'd', 500);
		addCommand(commandos, 'm', 100);
		delay(1000);
		printCommand(commandos);
		delay(1000);
		runCommand(commandos);
		break;
	}
	case '<':
	{
		Serial.println("Spooling up kopStepper");
		kopStepper.move(-2000);
		while (kopStepper.distanceToGo() < -1000)
			kopStepper.run();
		kopSpeed = -kopStepper.maxSpeed();
		break;
	}
	case '>':
	{
		Serial.println("Spooling up kopStepper");
		kopStepper.move(2000);
		while (kopStepper.distanceToGo() > 1000)
			kopStepper.run();
		kopSpeed = kopStepper.maxSpeed();
		break;
	}
	}
	}
	// set the motor speed in RPM:
	if (motorSpeed > 0) {
		myStepper.setSpeed(motorDirection * motorSpeed * rpm2steps);
		myStepper.runSpeed();			//Run motor at set speed.		
	}
	else
		myStepper.disableOutputs();
	
	if (kopSpeed != 0) {
		kopStepper.setSpeed(kopSpeed * rpm2steps);
		kopStepper.runSpeed();			//Run kopmotor at set speed.		
	}
	else
		kopStepper.disableOutputs();
}

