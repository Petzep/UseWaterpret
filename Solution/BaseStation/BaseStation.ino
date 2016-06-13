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
void on_status_selected(MenuItem* p_menu_item);
void on_back_item_selected(MenuItem* p_menu_item);
void on_help_selected(MenuItem* p_menu_item);
void on_about_selected(MenuItem* p_menu_item);
void on_add_selected(MenuItem* p_menu_item);
void on_del_selected(MenuItem* p_menu_item);
void on_demo_selected(MenuItem* p_menu_item);
void on_send_selected(MenuItem* p_menu_item);
bool addCommand(CommandBook *book, char command, int value);
int countCommand(CommandBook *book);
void runCommand(CommandBook *book);
void printCommand(CommandBook *book);
bool deleteCommand(CommandBook *book);
bool sendCommand(CommandBook *book);
CommandBook commandos[BOOKSIZE] = {};				// array with commands for the Arduino

// Menu variables
MenuSystem ms;
Menu mm("Main Menu", &display_menu);
	Menu muCommand("Command Builder..", &display_menu);
		NumericMenuItem muCommand_type("Command type", 0, 0, 11, 1, format_type);
		NumericMenuItem muCommand_value("Value", 0, -15, 15, 1, format_value);
		MenuItem muCommand_add("Add command to CommandBook", &on_add_selected);
		MenuItem muCommand_delete("Delete command from CommandBook", &on_del_selected);
		MenuItem muCommand_demo("Set demo sequence", &on_demo_selected);
		MenuItem muCommand_send("Send CommandBook to Robot", &on_send_selected);
	MenuItem muRobot_status("Status", &on_status_selected);
	BackMenuItem mu_back("Back..", &on_back_item_selected, &ms);
	MenuItem mm_help("Help", &on_help_selected);
	MenuItem mm_about("About", &on_about_selected);

void setup()
{
	Serial.begin(9600);
	nrf24Initialize(true);
	delay(3000);

	mm.add_menu(&muCommand);
		muCommand.add_item(&muCommand_type);
		muCommand.add_item(&muCommand_value);
		muCommand.add_item(&muCommand_add);
		muCommand.add_item(&muCommand_delete);
		muCommand.add_item(&muCommand_demo);
		muCommand.add_item(&muCommand_send);
		muCommand.add_item(&mu_back);
	mm.add_item(&muRobot_status);
	mm.add_item(&mm_help);
	mm.add_item(&mm_about);
	ms.set_root_menu(&mm);

	display_help();
	ms.display();
}

void loop() {
	serial_handler();




}

