#include <Arduino.h>
#include <alarms.h>
#include <clock.h>
#include <time.h>

std::vector<Alarm> alarms;

bool alarmRinging = false;
unsigned long alarmStartTime = 0;


bool addAlarm(int hour, int minute, RepeatMode repeat)
{
    Alarm newAlarm;
    newAlarm.time = hour * 60 + minute;
    newAlarm.repeat = repeat;

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
    struct tm timeinfo;

    if (!getLocalTime(&timeinfo))
        return;

    int currentDay = timeinfo.tm_wday;

    time_t currentTime = time(nullptr);
    time_t currentMinute = currentTime / 60;

    for (int i = 0; i < alarms.size(); i++)
    {
        if (alarms[i].enabled == true)
        {
            bool correctDay = false;

            if (alarms[i].repeat == ONCE)
            {
                correctDay = true;
            }
            else
            {
                int day = currentDay;

                if (day == 0)
                    day = 7;

                if (alarms[i].repeat == day)
                    correctDay = true;
            }

            if (timeNow == alarms[i].time &&
                correctDay &&
                alarms[i].lastTriggered != currentMinute)
            {
                alarms[i].lastTriggered = currentMinute;
                
                Serial.println("alarm! alarm! alarm!");

                digitalWrite(18, HIGH);
                tone(17, 500);

                alarmStartTime = millis();
                alarmRinging = true;

                if (alarms[i].repeat == ONCE)
                {
                    alarms[i].enabled = false;
                }
            }
        }

        if (alarmRinging == true)
        {
            if (digitalRead(2) == LOW)
            {
                Serial.println("alarm stopped");

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