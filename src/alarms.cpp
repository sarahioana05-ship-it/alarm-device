
#include <Arduino.h>
#include "alarms.h"
#include "clock.h"

std::vector<Alarm> alarms;

bool alarmRinging = false;
unsigned long alarmStartTime = 0;


bool addAlarm(int hour, int minute)
{
    Alarm newAlarm;
    newAlarm.time = hour * 60 + minute;

    for (int i = 0; i < alarms.size(); i++)
    {
        if (newAlarm.time == alarms[i].time)
        {
            Serial.println("Alarm already exists!");
            Serial.println("Choose another time!");
            return false;
        }
    }

    int insertPosition = alarms.size();

    for (int i = 0; i < alarms.size(); i++)
    {
        if (newAlarm.time < alarms[i].time)
        {
            insertPosition = i;
            break;
        }
    }

    alarms.insert(
        alarms.begin() + insertPosition,
        newAlarm
    );

    Serial.print("Alarm is set for ");

    if (hour < 10)
        Serial.print("0");

    Serial.print(hour);
    Serial.print(":");

    if (minute < 10)
        Serial.print("0");

    Serial.println(minute);

    return true;
}


void checkAlarms()
{
    for (int i = 0; i < alarms.size(); i++)
    {
        if (timeNow == alarms[i].time &&
            alarms[i].enabled == true)
        {
            Serial.println("alarm! alarm! alarm!");

            digitalWrite(18, HIGH);
            tone(17, 500);

            alarmStartTime = millis();
            alarms[i].enabled = false;
            alarmRinging = true;
        }

        if (alarmRinging == true)
        {
            if (digitalRead(02) == LOW)
            {
                Serial.println("alarm stoped");

                digitalWrite(18, LOW);
                noTone(17);

                alarmRinging = false;
                delay(200);
            }
            else if (millis() - alarmStartTime >= 5000)
            {
                Serial.println("alarm finished");

                digitalWrite(18, LOW);
                noTone(17);

                alarmRinging = false;
            }
        }
    }
}
