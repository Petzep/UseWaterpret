int countCommand(CommandBook *book)
{
	int c = 0;
	while (book[c].command != NULL && c < BOOKSIZE)
		c++;
	return c;
}

void addCommand(CommandBook *book, char command, int value)
{
	int c = countCommand(book);
	book[c].command = command;
	book[c].value = value;
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
			armStepper.move(book[i].value);
			while (armStepper.distanceToGo())
				armStepper.run();
			armStepper.disableOutputs();
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