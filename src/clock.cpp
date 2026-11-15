#include <Arduino.h>
#include <WiFi.h>
#include <time.h>
#include "clock.h"

int timeNow = 0;
int lastMinute = -1;

void setupClock()
{
    configTzTime(
        "EET-2EEST,M3.5.0/3,M10.5.0/4",
        "pool.ntp.org"
    );
}

bool updateClock()
{
    struct tm timeinfo;

    if (!getLocalTime(&timeinfo))
        return false;

    timeNow = timeinfo.tm_hour * 60 + timeinfo.tm_min;

    if (timeinfo.tm_min != lastMinute)
    {
        Serial.print("Current time: ");

        if (timeinfo.tm_hour < 10)
            Serial.print("0");

        Serial.print(timeinfo.tm_hour);
        Serial.print(":");

        if (timeinfo.tm_min < 10)
            Serial.print("0");

        Serial.println(timeinfo.tm_min);

        lastMinute = timeinfo.tm_min;

        return true;
    }

    return false;
}