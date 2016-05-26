/*
 Name:		Robot.ino
 Created:	5/19/2016 4:07:57 PM
 Author:	Use Waterpret
*/

#define Servo ServoTimer2

#define NoteAb	(NoteGs/2)
#define NoteA	110
#define NoteAs	116
#define NoteBb	NoteAs
#define NoteB	124
#define NoteC	131
#define NoteCs	139
#define NoteDb	NoteCs
#define NoteD	147
#define NoteDs	155
#define NoteEb	NoteDs
#define NoteE	165
#define NoteF	174
#define NoteFs	186
#define NoteGb	NoteFs
#define NoteG	196
#define NoteGs	208

#include <ServoTimer2.h>
#include <AccelStepper.h>

AccelStepper myStepper(AccelStepper::FULL4WIRE, 2, 4, 6, 7, false);	// initialize the stepper library on pins 2,4,6,7 and disable the output;

unsigned long tempo = 160;
int direction = 1;

void setup()
{
	Serial.begin(9600);
	myStepper.setAcceleration(9999.f);
	myStepper.setMaxSpeed(1000);

}

void playNote(int tone, int octave, int sixteenths) {
	direction *= -1;
	myStepper.setSpeed(direction * tone * (1 << (octave-1)));
	unsigned long time = millis() + sixteenths * (15000L / tempo);
	while (millis() < time) {
		myStepper.runSpeed();
	}
	myStepper.disableOutputs();
}

void pause(int sixteenths) {
	unsigned long time = millis() + sixteenths * (15000L / tempo);
	while (millis() < time) {
		// actively do nothing :D
	}
}

void playZelda() {
	tempo = 160; // bpm
	playNote(NoteG, 2, 2);
	playNote(NoteFs, 2, 2);
	playNote(NoteDs, 2, 2);
	playNote(NoteA, 2, 2);

	playNote(NoteGs, 1, 2);
	playNote(NoteE, 2, 2);
	playNote(NoteGs, 2, 2);
	playNote(NoteC, 3, 4);

	pause(14);
}

void playHappyBirthday() {
	tempo = 110; // bpm
	playNote(NoteC, 2, 3);
	playNote(NoteC, 2, 1);

	playNote(NoteD, 2, 4);
	playNote(NoteC, 2, 4);
	playNote(NoteF, 2, 4);

	playNote(NoteE, 2, 5);
	pause(3);
	playNote(NoteC, 2, 3);
	playNote(NoteC, 2, 1);

	playNote(NoteD, 2, 4);
	playNote(NoteC, 2, 4);
	playNote(NoteG, 2, 4);

	playNote(NoteF, 2, 5);
	pause(3);
	playNote(NoteC, 2, 3);
	playNote(NoteC, 2, 1);

	playNote(NoteC, 3, 4);
	playNote(NoteA, 3, 4);
	playNote(NoteF, 2, 3);
	playNote(NoteF, 2, 1);

	playNote(NoteE, 2, 4);
	playNote(NoteD, 2, 4);
	playNote(NoteAs, 3, 3);
	playNote(NoteAs, 3, 1);

	playNote(NoteA, 3, 4);
	playNote(NoteF, 2, 4);
	playNote(NoteG, 2, 4);

	playNote(NoteF, 2, 6);

	pause(18);
}

///////////////////////////////
//////////MAIN LOOP////////////
///////////////////////////////
void loop()
{
	playZelda();
	playHappyBirthday();
}
