
/*
* serial_nav.ino - Example code using the menu system library
*
* This example shows the menu system being controlled over the serial port.
*
* Copyright (c) 2015, 2016 arduino-menusystem
* Licensed under the MIT license (see LICENSE)
*/

// Menu callback function

// writes the (int) value of a float into a char buffer.
const String format_int(const float value)
{
	return String((int)value);
}

// writes the value of a float into a char buffer.
const String format_float(const float value)
{
	return String(value);
}

const String format_type(const float value)
{
	String buffer;

	switch ((int)value)
	{
	case 0:
		buffer += "Go to plant";
		tempCommand.command = 't';
		break;
	case 1:
		buffer += "Grab plant";
		tempCommand.command = 'g';
		break;
	case 2:
		buffer += "Clean plant";
		tempCommand.command = 'c';
		break;
	case 3:
		buffer += "Return home";
		tempCommand.command = 'r';
		break;
	case 4:
		buffer += "Move";
		tempCommand.command = 'm';
		break;
	case 5:
		buffer += "Delay";
		tempCommand.command = 'd';
		break;
	default:
		buffer += "undef";
	}

	return buffer;
}

const String format_value(const float value)
{
	tempCommand.value = (int)value;

	switch (tempCommand.command) {
	case 't':
	case 'c':
		return "plant " + String(tempCommand.value);
	case 'm':
		return String(tempCommand.value * 100) + " steps";
	case 'd':
		return String(tempCommand.value * 100) + " ms";
	case 'g':
	case 'r':
		return "N/A";
	default:
		return "undef";
	}
}

void on_add_selected(MenuItem* p_menu_item)
{
	if (!addCommand(commandos, tempCommand.command, tempCommand.value)) {
		Serial.println();
		Serial.println(F("ERROR:"));
		Serial.println(F("Command book full!"));
		delay(1000);
	}
}

void on_del_selected(MenuItem* p_menu_item)
{
	if (!deleteCommand(commandos)) {
		Serial.println();
		Serial.println(F("ERROR:"));
		Serial.println(F("No commands to delete!"));
		delay(1000);
	}
}

void on_send_selected(MenuItem* p_menu_item)
{
	if (!sendCommand(commandos)) {
		Serial.println(F("ERROR:"));
		Serial.println(F("Could not send commands"));
	}

	delay(1000);
}

void on_demo_selected(MenuItem* p_menu_item) {
	int c = countCommand(commandos);
	for (int n = 0; n < c; n += 1) {
		deleteCommand(commandos);
	}
	addCommand(commandos, 't', 1);
	addCommand(commandos, 'g', 0);
	addCommand(commandos, 'r', 0);
	addCommand(commandos, 'c', 2);
	addCommand(commandos, 'g', 0);
	addCommand(commandos, 'r', 0);
	addCommand(commandos, 't', 3);
	addCommand(commandos, 'g', 0);
	addCommand(commandos, 'r', 0);
}

void on_testMessage_selected(MenuItem* p_menu_item)
{
	uint8_t* data = (uint8_t*) "Test bericht";
	nrf24SendMessage(data, 13);
	Serial.println(F("Message sent"));
	delay(1000);
}

void on_help_selected(MenuItem* p_menu_item)
{
	display_help();
	delay(3000);
}

void on_about_selected(MenuItem* p_menu_item)
{
	Serial.println(F("About bla bla bla"));
	delay(1000);
}

void on_otherMessage_selected(MenuItem* p_menu_item)
{
	Serial.println(F("Other test message. (not sent #placeholder)"));
	delay(1000);
}


void on_status_selected(MenuItem* p_menu_item)
{
	Serial.println(F("Returning status of the robot:<placeholder>"));
	Serial.println(F("------------------------------"));
	Serial.print(F("Temperature: "));
	Serial.println("80");
	Serial.print(F("Max speed (RPM): "));
	Serial.println("300");
	Serial.print(F("Acceleration: "));
	Serial.println("80");
	Serial.print(F("Motor1 "));
	Serial.println("running");
	Serial.print(F("Motor2: "));
	Serial.println("not running");
	Serial.print(F("Motor3: "));
	Serial.println("not running");
	Serial.print(F("ServoArm (degrees):"));
	Serial.println("167");
	Serial.print(F("MotorGrab: "));
	Serial.println("086");
	delay(3000);
}

void on_back_item_selected(MenuItem* p_menu_item)
{
	Serial.println(F("Back item Selected"));
	delay(1000);
}

void display_menu(Menu* p_menu)
{
	Serial.write(27);       // ESC command
	Serial.print("[2J");    // clear screen command
	Serial.write(27);
	Serial.print("[H");     // cursor to home command
	Serial.print(F("--[ "));
	Serial.print(p_menu->get_name());
	Serial.println(F(" ]--"));

	String buffer;
	MenuComponent const* cp_menu_sel = p_menu->get_current_component();
	for (int i = 0; i < p_menu->get_num_menu_components(); ++i)
	{
		MenuComponent const* cp_m_comp = p_menu->get_menu_component(i);
		if (cp_menu_sel == cp_m_comp)
			Serial.print("> ");
		else
			Serial.print("  ");
		Serial.print(cp_m_comp->get_composite_name(buffer));

		Serial.println("");
	}

	if (p_menu == &muCommand) {
		Serial.println();
		printCommand(commandos);
	}
}


void display_help() {
	Serial.println(F("***************"));
	Serial.println(F("w,^:      go to previus item (up)"));
	Serial.println(F("s,v:      go to next item (down)"));
	Serial.println(F("a,esc:    go back (right)"));
	Serial.println(F("d,enter:  select \"selected\" item"));
	Serial.println(F("h,?:      print this help"));
	Serial.println(F("***************"));
}

void serial_handler()
{
	char inChar, inChar2;

	if ((inChar = Serial.read()) > 0)
	{
		switch (inChar)
		{
		case 27: // control characters
			delay(25);
			if ((inChar2 = Serial.read()) > 0) {
				switch (inChar2) {
				case 91: // escaped control characters
					delay(25);
					switch (Serial.read()) {
					case 65: // up
					case 90: // shift+tab
					case 68: // left
						ms.prev();
						ms.display();
						break;
					case 66: // down
					case 67: // right
						ms.next();
						ms.display();
						break;
					}
					break;
				case 49: // function keys, just display help.
					ms.display();
					display_help();
					while (Serial.read() > 0);
					break;
				}
			}
			else { // escape
				ms.back();
				ms.display();
			}
			break;

		case 'w': // w
			// Previous item
			ms.prev();
			ms.display();
			break;
		case 's': // s
		case 9: // tab
			// Next item
			ms.next();
			ms.display();
			break;
		case 127: // backspace
		case 'a': // a
		case 3: // Ctrl+C
			// Back presed
			ms.back();
			ms.display();
			break;
		case 0x0D: // enter
		case ' ': // space
		case 'd': // d
			// Select presed
			ms.select();
			ms.display();
			break;
		case '?': // ?
		case 0x0F: // F1
		case 'h': // h
			// Display help
			ms.display();
			display_help();
			break;
		default:
			ms.display();
			display_help();
			Serial.print("Unknown character: ");
			Serial.println((int)inChar);
			// read remaining characters
			while (Serial.read() > 0);
			break;
		}
	}
}