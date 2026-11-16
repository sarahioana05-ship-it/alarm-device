#include <Arduino.h>
#include <menu.h>
#include <potentiometer.h>
#include <alarms.h>

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

int alarmSettingsSelection = 0;
int lastAlarmSettingsSelection = -1;

int repeatSelection = 0;
int lastRepeatSelection = -1;

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

                lastCheckAlarm = alarmSelection;
            }
        }
    }

    if (menuState == 2 && digitalRead(3) == LOW)
    {
        menuState = 3;

        alarmSettingsSelection = getPotPosition(3);
        lastAlarmSettingsSelection = -1;

        Serial.println("Alarm Settings Menu");

        Serial.print("Alarm at: ");

        int hour = alarms[alarmSelection].time / 60;
        int minute = alarms[alarmSelection].time % 60;

        if (hour < 10)
            Serial.print("0");

        Serial.print(hour);
        Serial.print(":");

        if (minute < 10)
            Serial.print("0");

        Serial.println(minute);

        Serial.print("State: ");

        if (alarms[alarmSelection].enabled)
        {
            Serial.println("enabled");

            switch (alarms[alarmSelection].repeat)
            {
                case ONCE:      Serial.print("once"); break;
                case MONDAY:    Serial.print("every monday"); break;
                case TUESDAY:   Serial.print("every tuesday"); break;
                case WEDNESDAY: Serial.print("every wednesday"); break;
                case THURSDAY:  Serial.print("every thursday"); break;
                case FRIDAY:    Serial.print("every friday"); break;
                case SATURDAY:  Serial.print("every saturday"); break;
                case SUNDAY:    Serial.print("every sunday"); break;
            }
        } 
        else
            Serial.println("disabled");

        Serial.println("Change state to:");

        digitalWrite(18, HIGH);
        delay(100);
        digitalWrite(18, LOW);
        delay(200);
    }
}

void handleAlarmSettingsMenu()
{
    if (menuState != 3)
        return;

    alarmSettingsSelection = getPotValue(alarmSettingsSelection, 3);

    if (alarmSettingsSelection != lastAlarmSettingsSelection)
    {
        if (alarmSettingsSelection == 0)
            Serial.println("delete");

        else if (alarmSettingsSelection == 1)
            Serial.println("disable");

        else if (alarmSettingsSelection == 2)
            Serial.println("enable");

        lastAlarmSettingsSelection = alarmSettingsSelection;
    }

    if (digitalRead(3) == LOW)
    {
        if (alarmSettingsSelection == 0)
        {
            alarms.erase(alarms.begin() + alarmSelection);

            Serial.println("Alarm deleted");

            menuState = 2;

            if (alarmSelection >= alarms.size())
            {
                alarmSelection = 0;
            }

            lastCheckAlarm = -1;
        }

        else if (alarmSettingsSelection == 1)
        {
            alarms[alarmSelection].enabled = false;

            Serial.println("Alarm disabled");

            menuState = 2;
            lastCheckAlarm = -1;
        }

        else if (alarmSettingsSelection == 2)
        {
            menuState = 4;

            repeatSelection = getPotPosition(8);
            lastRepeatSelection = -1;

            Serial.println("Repeat Selection Menu");
            Serial.println("Choose when the alarm should ring:");
        }

        digitalWrite(18, HIGH);
        delay(100);
        digitalWrite(18, LOW);
        delay(200);
    }

    if (digitalRead(2) == LOW)
    {
        menuState = 2;

        Serial.println("Back to Check Alarms Menu");

        digitalWrite(18, HIGH);
        delay(100);
        digitalWrite(18, LOW);
        delay(200);
    }
}

void handleRepeatMenu()
{
    if (menuState != 4)
        return;

    repeatSelection = getPotValue(repeatSelection, 8);

    if (repeatSelection != lastRepeatSelection)
    {
        switch (repeatSelection)
        {
            case 0: Serial.println("once"); break;
            case 1: Serial.println("every monday"); break;
            case 2: Serial.println("every tuesday"); break;
            case 3: Serial.println("every wednesday"); break;
            case 4: Serial.println("every thursday"); break;
            case 5: Serial.println("every friday"); break;
            case 6: Serial.println("every saturday"); break;
            case 7: Serial.println("every sunday"); break;
        }

        lastRepeatSelection = repeatSelection;
    }

    if (digitalRead(3) == LOW)
    {
        alarms[alarmSelection].repeat =
            static_cast<RepeatMode>(repeatSelection);

        alarms[alarmSelection].enabled = true;

        Serial.println("Alarm enabled");

        menuState = 2;
        lastCheckAlarm = -1;

        digitalWrite(18, HIGH);
        delay(100);
        digitalWrite(18, LOW);
        delay(200);
    }

    if (digitalRead(2) == LOW)
    {
        menuState = 3;

        lastAlarmSettingsSelection = -1;

        Serial.println("Back to Alarm Settings Menu");

        digitalWrite(18, HIGH);
        delay(100);
        digitalWrite(18, LOW);
        delay(200);
    }
}