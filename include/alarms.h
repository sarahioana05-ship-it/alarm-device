#ifndef ALARMS_H
#define ALARMS_H
#include <time.h>
#include <vector>

enum RepeatMode
{
    ONCE,
    MONDAY,
    TUESDAY,
    WEDNESDAY,
    THURSDAY,
    FRIDAY,
    SATURDAY,
    SUNDAY
};

struct Alarm
{
    int time;
    bool enabled = true;
    RepeatMode repeat = ONCE;
    time_t lastTriggered = 0;
};

extern std::vector<Alarm> alarms;

void checkAlarms();

bool addAlarm(int hour, int minute, RepeatMode repeat = ONCE);

extern bool alarmRinging;

#endif