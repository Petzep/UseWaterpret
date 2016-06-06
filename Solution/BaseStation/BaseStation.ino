/*
 Name:		BaseStation.ino
 Created:	5/19/2016 4:07:46 PM
 Author:	Use Waterpret
*/

#include <SPI.h>

#define ENABLE_DEBUG
#define BOOKSIZE 10
#define BASESTATION

#ifdef ENABLE_DEBUG
// disable Serial output
#define debugln(a) (Serial.println(a))
#define debug(a) (Serial.print(a))
#else
#define debugln(a)
#define debug(a)
#endif

struct CommandBook
{
	char command;
	int value;
}tempCommand;

//menu
#include <MenuSystem.h>
#include "CustomNumericMenuItem.h"

// forward declarations
const String format_float(const float value);
const String format_int(const float value);
const String format_color(const float value);
const String format_type(const float value);
const String format_value(const float value);
void display_menu(Menu* p_menu);
void on_testMessage_selected(MenuItem* p_menu_item);
void on_otherMessage_selected(MenuItem* p_menu_item);
void on_status_selected(MenuItem* p_menu_item);
void on_back_item_selected(MenuItem* p_menu_item);
void on_help_selected(MenuItem* p_menu_item);
void on_about_selected(MenuItem* p_menu_item);
void on_add_selected(MenuItem* p_menu_item);
void on_print_selected(MenuItem* p_menu_item);
void on_sent_selected(MenuItem* p_menu_item);
void addCommand(CommandBook *book, char command, int value);
int countCommand(CommandBook *book);
void runCommand(CommandBook *book);
void printCommand(CommandBook *book);
CommandBook commandos[BOOKSIZE] = {};				// array with commands for the Arduino

// Menu variables
MenuSystem ms;
Menu mm("Main Menu", &display_menu);
Menu muMessages(">Messages", &display_menu);
	MenuItem muMessages_test("-Send test message", &on_testMessage_selected);
	MenuItem muMessages_otherTest("-Send another test message", &on_otherMessage_selected);
	BackMenuItem muMessages_back(" -Back-", &on_back_item_selected, &ms);
Menu muRobot(">Robot", &display_menu);
	Menu muCommand(">Command Builder", &display_menu);
		NumericMenuItem muCommand_type("-Command type", 0, 0, 5, 1, format_type);
		NumericMenuItem muCommand_value("value", -1500, 0, 1500, 100, format_value);
		MenuItem muCommand_add("-Add command to CommandBook", &on_add_selected);
		MenuItem muCommand_print("-print CommandBook", &on_print_selected);
		MenuItem muCommand_sent("-sent CommandBook", &on_sent_selected);
	MenuItem muRobot_status("-Status", &on_status_selected);
	NumericMenuItem muRobot_kleur("-Kleurselector", 0, 0, 2, 1, format_color);
	CustomNumericMenuItem muRobot_speed(12, "Max Speed (slider)", 300, 200, 400, 20, format_int);
	NumericMenuItem muRobot_temp("Max Temp", 100.0, 60.0, 80.0, 0.2, format_float);
	NumericMenuItem muRobot_accel("Acceleration", 350, 300, 400, 25, format_int);
	BackMenuItem muRobot_back(" -Back-", &on_back_item_selected, &ms);
MenuItem mm_help("-help", &on_help_selected);
MenuItem mm_about("-About", &on_about_selected);

void setup()
{
	Serial.begin(9600);
	nrf24Initialize();

	mm.add_menu(&muMessages);
		muMessages.add_item(&muMessages_test);
		muMessages.add_item(&muMessages_otherTest);
		muMessages.add_item(&muMessages_back);
	mm.add_menu(&muRobot);
		muRobot.add_menu(&muCommand);
			muCommand.add_item(&muCommand_type);
			muCommand.add_item(&muCommand_value);
			muCommand.add_item(&muCommand_add);
			muCommand.add_item(&muCommand_print);
			muCommand.add_item(&muCommand_sent);
		muRobot.add_item(&muRobot_status);
		muRobot.add_item(&muRobot_kleur);
		muRobot.add_item(&muRobot_accel);
		muRobot.add_item(&muRobot_speed);
		muRobot.add_item(&muRobot_back);
	mm.add_item(&mm_help);
	mm.add_item(&mm_about);
	ms.set_root_menu(&mm);

	display_help();
	ms.display();
}

void loop() {
	serial_handler();




}

