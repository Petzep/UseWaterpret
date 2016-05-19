#include <Stepper.h>
#include <SPI.h>
#include <RH_NRF24.h>

// Singleton instance of the radio driver
RH_NRF24 nrf24;

const int stepsPerRevolution = 200;  // change this to fit the number of steps per revolution
// for your motor


// initialize the stepper library on pins 8 through 11:
Stepper myStepper(stepsPerRevolution, 3, 4, 5, 6);

int stepCount = 0;  // number of steps the motor has taken
int motorSpeed = 0;
void leBlink(int ledPin, int n);
int Y = 1;
unsigned long previousMillis = 0;        // will store last time LED was updatedvccv

void setup()
{
   Serial.begin(9600);                        // open the serial port at 9600 bps:
  if (!nrf24.init())
    Serial.println("init failed");
  // Defaults after init are 2.402 GHz (channel 2), 2Mbps, 0dBm
  if (!nrf24.setChannel(1))
    Serial.println("setChannel failed");
  if (!nrf24.setRF(RH_NRF24::DataRate2Mbps, RH_NRF24::TransmitPower0dBm))
    Serial.println("setRF failed");
  // initialize digital pin 13 as an output.
}

void loop()
{
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= 1000) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;
    Serial.println("Sending to nrf24_server");
    // Send a message to nrf24_server
    uint8_t data[] = "Hello World!";
    uint8_t check[] = "And hello back to you";
    nrf24.send(data, sizeof(data));
    
    nrf24.waitPacketSent();
    // Now wait for a reply
    uint8_t buf[RH_NRF24_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
  
    if (nrf24.waitAvailableTimeout(500))
    { 
      // Should be a reply message for us now   
      if (nrf24.recv(buf, &len))
      {
        Serial.print("got reply: ");
        Serial.println((char*)buf);
        Y = Y * -1;
      }
      else
      {
        Serial.println("recv failed");
      }
    }
    else
    {
      Serial.println("No reply, is nrf24_server running?");
    }
  }
  
int  c = Serial.read();
  switch(c)
  {
    case '-':
    {
      motorSpeed -= 10;
      if (motorSpeed < 0)
        motorSpeed = 0;
      Serial.print("Motorspeed: ");
      Serial.println(motorSpeed);
      break;
    }
    case '+':
    {
      motorSpeed += 10;
      Serial.print("Motorspeed: ");
      Serial.println(motorSpeed);
      break;
    }
    case '0':
    {
      motorSpeed = 0;
      Serial.print("Motorspeed: ");
      Serial.println(motorSpeed);
      break;
    }
  }
  // set the motor speed:
  if (motorSpeed > 0) {
    myStepper.setSpeed(motorSpeed);
    // step 1/100 of a revolution:
   myStepper.step(Y*stepsPerRevolution / 100);
  }
}

void leBlink(int ledPin, int n)
{
  for(int i = 0; i < n; i++)
  {
  digitalWrite(ledPin, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(100);              // wait for a second
  digitalWrite(ledPin, LOW);    // turn the LED off by making the voltage LOW
  delay(100);              // wait for a second
  }
}

