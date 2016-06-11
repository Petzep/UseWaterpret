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
		case 'r':
		{
			r_MovePlant(0);
			break;
		}
		case 'W':
		{
			shaftStepper.moveTo(900);
			shaftStepper.runToPosition();
			shaftStepper.disableOutputs();
			break;
		}
		case 'S':
		{
			shaftStepper.moveTo(0);
			shaftStepper.runToPosition();
			shaftStepper.disableOutputs();
			break;
		}
		case 'D':
		{
			armStepper.moveTo(750);
			armStepper.runToPosition();
			armStepper.disableOutputs();
			break;
		}
		case 'A':
		{
			armStepper.moveTo(0);
			armStepper.runToPosition();
			armStepper.disableOutputs();
			break;
		}
		case 'G':
		{
			armTurnPos(book[i].value);
			break;
		}
		default:		// "undef"
			break;
		book[i].command = NULL;
		book[i].value = NULL;

		}
		Serial.println("Done");
	}
}
#endif // BASESTATION

void printCommand(CommandBook *book)
{
	int c = countCommand(book);
	Serial.println("Commandos in book:");
	for (int i = 0; i < c; i++)
	{
		Serial.print('-');
		Serial.print(book[i].command);
		Serial.print('\t');
		Serial.println(book[i].value);
	}
	Serial.println("-END OF COMMANDS");
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
