#include <Arduino.h>
#include "menu.h"
#include "potentiometer.h"
#include "alarms.h"

int menuSelection = 1;
int lastmenuSelection = -1;
bool menuSelect = false;

int alarmHour = 0;
int alarmMinute = 0;

int setMode = 0;

int lastalarmHour = -1;
int lastalarmMinute = -1;

unsigned long buttonPressTime;
bool buttonPressed = false;

bool alarmSet = false;

int alarmSelection=0;
int lastCheckAlarm=-1;

void handleSetAlarmMenu()
{
    if (menuState != 1)
        return;

    if (alarmSet == true)
    {
        if (setMode == 0)
            alarmHour = getPotValue(alarmHour, 24);
        else
            alarmMinute = getPotValue(alarmMinute / 5, 12) * 5;

        if (alarmHour != lastalarmHour ||
            alarmMinute != lastalarmMinute)
        {
            Serial.print("Alarm time: ");

            if (alarmHour < 10)
                Serial.print("0");

            Serial.print(alarmHour);
            Serial.print(":");

            if (alarmMinute < 10)
                Serial.print("0");

            Serial.println(alarmMinute);

            lastalarmHour = alarmHour;
            lastalarmMinute = alarmMinute;
        }
    }

    if (digitalRead(3) == LOW && buttonPressed == false)
    {
        buttonPressTime = millis();
        buttonPressed = true;
    }

    if (digitalRead(3) == HIGH && buttonPressed == true)
    {
        unsigned long press = millis() - buttonPressTime;

        if (alarmSet == false)
        {
            alarmSet = true;
            setMode = 0;

            alarmHour = getPotPosition(24);
            lastalarmHour = -1;
            lastalarmMinute = -1;

            Serial.println("Alarm setting mode");
            Serial.println("Adjust hours with the pot");

            digitalWrite(18, HIGH);
            delay(100);
            digitalWrite(18, LOW);
            delay(200);
        }
        else
        {
            if (press >= 800)
            {
                if (setMode == 0)
                {
                    Serial.println("Adjust minutes with the pot");

                    alarmMinute = getPotPosition(12) * 5;
                    lastalarmMinute = -1;
                    setMode = 1;
                }
                else
                {
                    Serial.println("Adjust hours with the pot");

                    alarmHour = getPotPosition(24);
                    lastalarmHour = -1;
                    setMode = 0;
                }
            }
            else
            {
                if (addAlarm(alarmHour, alarmMinute))
                {
                    alarmSet = false;
                }

                digitalWrite(18, HIGH);
                delay(100);
                digitalWrite(18, LOW);
                delay(200);
            }
        }

        buttonPressed = false;
    }

    if (digitalRead(2) == LOW && alarmRinging == true)
    {
        Serial.println("alarm stoped");

        digitalWrite(18, LOW);
        noTone(17);

        alarmRinging = false;
        delay(200);
    }

    if (digitalRead(2) == LOW && alarmSet == true)
    {
        Serial.println("cancel");
        alarmSet = false;

        digitalWrite(18, HIGH);
        delay(100);
        digitalWrite(18, LOW);
        delay(200);
    }
}

void handleBackToMainMenu(bool alarmSet)
{
    if (((menuState == 1 && alarmSet == false) ||
         menuState == 2) &&
        digitalRead(2) == LOW)
    {
        Serial.println("Back to Main menu");
        menuState = 0;

        digitalWrite(18, HIGH);
        delay(100);
        digitalWrite(18, LOW);
        delay(200);
    }
}

void handleMainMenu()
{
    if (menuState == 0 && digitalRead(3) == LOW && menuSelect == false)
    {
        menuSelect = true;

        Serial.println("Select: SET ALARMS || CHECK ALARMS");
        Serial.println("Press Right B to select or Left B to cancel");

        digitalWrite(18, HIGH);
        delay(100);
        digitalWrite(18, LOW);
        delay(200);
    }

    if (menuState == 0 && menuSelect == true)
    {
        menuSelection = getPotValue(menuSelection, 2);

        if (lastmenuSelection != menuSelection)
        {
            if (menuSelection == 0)
                Serial.println("> SET ALARMS");
            else
                Serial.println("> CHECK ALARMS");

            lastmenuSelection = menuSelection;
        }

        if (digitalRead(3) == LOW)
        {
            menuSelect = false;
            menuState = menuSelection + 1;

            Serial.print("Entered menu: ");

            if (menuState == 1)
                Serial.println("SET ALARMS");
            else if (menuState == 2)
                Serial.println("CHECK ALARMS");

            digitalWrite(18, HIGH);
            delay(100);
            digitalWrite(18, LOW);
            delay(200);
        }
    }
}

void handleCheckAlarmsMenu()
{
    if (menuState == 2)
    {
        if (alarms.size() == 0)
        {
            Serial.println("No existing alarms!");
            Serial.println("Back to main menu");
            menuState = 0;
        }
        else
        {
            alarmSelection = getPotValue(alarmSelection, alarms.size());

            if (lastCheckAlarm != alarmSelection)
            {
                if (alarms[alarmSelection].time / 60 < 10)
                    Serial.print("0");

                Serial.print(alarms[alarmSelection].time / 60);
                Serial.print(":");

                if (alarms[alarmSelection].time % 60 < 10)
                    Serial.print("0");

                Serial.println(alarms[alarmSelection].time % 60);

                if (alarms[alarmSelection].enabled == true)
                    Serial.println("enabled");
                else
                    Serial.println("disabled");

                lastCheckAlarm = alarmSelection;
            }
        }
    }
}