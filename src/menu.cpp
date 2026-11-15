#include <Arduino.h>
#include "menu.h"
#include "potentiometer.h"

int menuSelection = 1;
int lastmenuSelection = -1;
bool menuSelect = false;

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