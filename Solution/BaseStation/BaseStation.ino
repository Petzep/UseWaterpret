/*
 Name:		BaseStation.ino
 Created:	5/19/2016 4:07:46 PM
 Author:	Use Waterpret
*/

#include <SPI.h>

#define ENABLE_DEBUG

//menu
#include <MenuSystem.h>
#include "CustomNumericMenuItem.h"

// forward declarations
const String format_float(const float value);
const String format_int(const float value);
const String format_color(const float value);
void display_menu(Menu* p_menu);
void on_testMessage_selected(MenuItem* p_menu_item);
void on_otherMessage_selected(MenuItem* p_menu_item);
void on_status_selected(MenuItem* p_menu_item);
void on_back_item_selected(MenuItem* p_menu_item);
void on_help_selected(MenuItem* p_menu_item);
void on_about_selected(MenuItem* p_menu_item);

// Menu variables
MenuSystem ms;
Menu mm("Main Menu", &display_menu);
Menu muMessages(">Messages", &display_menu);
	MenuItem muMessages_test("-Send test message", &on_testMessage_selected);
	MenuItem muMessages_otherTest("-Send another test message", &on_otherMessage_selected);
	BackMenuItem muMessages_back(" -Back-", &on_back_item_selected, &ms);
Menu muRobot(">Robot", &display_menu);
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

