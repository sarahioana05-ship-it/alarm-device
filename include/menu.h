#ifndef MENU_H
#define MENU_H

extern int menuState;

void handleMainMenu();
void handleBackToMainMenu(bool alarmSet);
void handleSetAlarmMenu();

extern bool alarmSet;

void handleCheckAlarmsMenu();

#endif