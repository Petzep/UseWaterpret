
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

// writes the value of a float into a char buffer as predefined colors.
const String format_color(const float value)
{
	String buffer;

	switch ((int)value)
	{
	case 0:
		buffer += "Red";
		break;
	case 1:
		buffer += "Green";
		break;
	case 2:
		buffer += "Blue";
		break;
	default:
		buffer += "undef";
	}

	return buffer;
}

const String format_type(const float value)
{
	String buffer;

	switch ((int)value)
	{
	case 0:
		buffer += "Go to plant";
		tempCommand.command = 'x';
		break;
	case 1:
		buffer += "Grab plant";
		tempCommand.command = 'x';
		break;
	case 2:
		buffer += "Clean plant";
		tempCommand.command = 'x';
		break;
	case 3:
		buffer += "Return home";
		tempCommand.command = 'x';
		break;
	case 4:
		buffer += "move";
		tempCommand.command = 'm';
		break;
	case 5:
		buffer += "delay";
		tempCommand.command = 'd';
		break;
	default:
		buffer += "undef";
	}

	return buffer;
}

const String format_value(const float value)
{
	tempCommand.value =(int)value;
	return String((int)value);
}

// In this example all menu items use the same callback.
void on_add_selected(MenuItem* p_menu_item)
{
	addCommand(commandos, tempCommand.command, tempCommand.value);
	Serial.println(F("Command added"));
	delay(1000);
}

void on_print_selected(MenuItem* p_menu_item)
{
	printCommand(commandos);
	delay(1000);
}

void on_sent_selected(MenuItem* p_menu_item)
{
	Serial.println(F("Commands sended"));
	delay(1000);
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
	Serial.print(F("Current menu name: "));
	Serial.println(p_menu->get_name());

	String buffer;
	MenuComponent const* cp_menu_sel = p_menu->get_current_component();
	for (int i = 0; i < p_menu->get_num_menu_components(); ++i)
	{
		MenuComponent const* cp_m_comp = p_menu->get_menu_component(i);
		Serial.print(cp_m_comp->get_composite_name(buffer));

		if (cp_menu_sel == cp_m_comp)
			Serial.print("<<< ");

		Serial.println("");
	}
}


void display_help() {
	Serial.println(F("***************"));
	Serial.println(F("w: go to previus item (up)"));
	Serial.println(F("s: go to next item (down)"));
	Serial.println(F("a: go back (right)"));
	Serial.println(F("d: select \"selected\" item"));
	Serial.println(F("?: print this help"));
	Serial.println(F("h: print this help"));
	Serial.println(F("***************"));
}

void serial_handler()
{
	char inChar;
	if ((inChar = Serial.read()) > 0)
	{
		switch (inChar)
		{
		case 'w': // Previus item
			ms.prev();
			ms.display();
			break;
		case 's': // Next item
			ms.next();
			ms.display();
			break;
		case 'a': // Back presed
			ms.back();
			ms.display();
			break;
		case 'd': // Select presed
			ms.select();
			ms.display();
			break;
		case '?':
		case 'h': // Display help
			ms.display();
			display_help();
			break;
		default:
			break;
		}
	}
}