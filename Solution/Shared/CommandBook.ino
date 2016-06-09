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
		case 'm':
		{
			myStepper.move(book[i].value);
			while (myStepper.distanceToGo())
				myStepper.run();
			myStepper.disableOutputs();
			break;
		}
		case 'd':
		{
			delay(book[i].value);
			break;
		}
		default:
		{
			Serial.println("Invaild command!");
			break;
		}
		}
		book[i].command = NULL;
		book[i].value = NULL;
	}
	Serial.println("Done");
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
	uint8_t* buf = new uint8_t[c * 3];
	for (int n = 0; n < c; n += 1) {
		buf[n * 3 + 0] = (uint8_t)book[c].command;
		buf[n * 3 + 1] = (uint8_t)(book[c].value & 0xFF);
		buf[n * 3 + 2] = (uint8_t)((book[c].value >> 8) & 0xFF);
	}

	Serial.println(F("Sending message..."));
	return nrf24SendMessage(buf, c * 3);
}