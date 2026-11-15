#ifndef ALARMS_H
#define ALARMS_H

#include <vector>

struct Alarm
{
    int time;
    bool enabled = true;
};

extern std::vector<Alarm> alarms;

void checkAlarms();

bool addAlarm(int hour, int minute);

#endif
