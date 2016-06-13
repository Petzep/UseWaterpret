int countCommand(CommandBook *book)
{
	int c = 0;
	while (book[c].command != NULL && c < BOOKSIZE)
		c++;
	return c;
}

bool addCommand(CommandBook *book, char command, int value)
{
	int c = countCommand(book);
	if (c >= BOOKSIZE) {
		return false;
	}
	book[c].command = command;
	book[c].value = value;
	return true;
}

#ifndef BASESTATION
void runCommand(CommandBook *book)
{
	int c = countCommand(book);
	for (int i = 0; i < c; i++)
	{
		switch (book[i].command)
		{
		case 't':
		{
			r_MovePlant(book[i].value);
			break;
		}
		case 'c':
		{
			r_Cut();
			break;
		}
		case 'g':
		{
			r_Grab();
			break;
		}
		case 'r':
		{
			r_MovePlant(0);
			break;
		}
		case 'W':
		{
			shaftStepper.enableOutputs();
			shaftStepper.moveTo(900);
			shaftStepper.runToPosition();
			shaftStepper.disableOutputs();
			break;
		}
		case 'S':
		{
			shaftStepper.enableOutputs();
			shaftStepper.moveTo(0);
			shaftStepper.runToPosition();
			//shaftStepper.disableOutputs(); //Keeping the shaft on it's place ONE TIME ONLY FOR DEMO (MABYE 2-3, but check temperature)
			break;
		}
		case 'D':
		{
			armStepper.enableOutputs();
			armStepper.moveTo(750);
			armStepper.runToPosition();
			armStepper.disableOutputs();
			break;
		}
		case 'A':
		{
			armStepper.enableOutputs();
			armStepper.moveTo(0);
			armStepper.runToPosition();
			armStepper.disableOutputs();
			break;
		}
		case 'u':
		{
			armTurnPos(book[i].value * 10);
			break;
		}
		case 'i':
		{
			grabberTurnPos(book[i].value *10 );
			break;
		}
		case 'd':
		{
			delay(book[i].value*500);
			break;
		}
		case 'm':
		{
			kopStepper.move(book[i].value * 100);
			break;
		}
		default:		// "undef"
			break;
		book[i].command = NULL;
		book[i].value = NULL;

		}
		Serial.print('-');
		Serial.print(book[i].command);
		Serial.print('\t');
		Serial.print(book[i].value);
		Serial.println("\tDone");
	}
	Serial.println("All commands done.");
}
#endif // BASESTATION

void printCommand(CommandBook *book)
{
	int c = countCommand(book);
	Serial.println(F("Commandos in book:"));
	for (int i = 0; i < c; i++)
	{
		Serial.print('-');
		Serial.print(book[i].command);
		Serial.print('\t');
		Serial.println(book[i].value);
	}
	Serial.println(F("-END OF COMMANDS"));
}

bool deleteCommand(CommandBook *book)
{
	int c = countCommand(book);
	if (c == 0) {
		return false;
	}
	book[c - 1].command = NULL;
	book[c - 1].value = NULL;
	return true;
}

bool sendCommand(CommandBook *book) {
	int c = countCommand(book);
	uint8_t* buf = new uint8_t[BOOKSIZE * 3 + 1];
	buf[0] = c;
	for (int n = 0; n < c; n += 1) {
		buf[(n * 3) + 1] = (uint8_t) book[n].command;
		buf[(n * 3) + 2] = (uint8_t) (book[n].value >> 8);
		buf[(n * 3) + 3] = (uint8_t)(book[n].value & 0xFF);
	}

	Serial.println(F("Sending message..."));
	bool result = nrf24SendMessage(buf, c * 3 + 1);
	delete buf;
	return result;
}
