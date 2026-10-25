#include <Arduino.h>
#include "potentiometer.h"

int getPotValue(int currentValue, int numberOfValues)
{
    int rawValue = analogRead(1);

    int range = 4096 / numberOfValues;
    int lowerLimit = currentValue * range;
    int upperLimit = (currentValue + 1) * range;

    if (currentValue > 0 && rawValue < lowerLimit - 100)
        currentValue--;

    if (currentValue < numberOfValues - 1 &&
        rawValue > upperLimit + 100)
        currentValue++;

    return currentValue;
}

int getPotPosition(int numberOfValues)
{
    int rawValue = analogRead(1);

    int value = (rawValue * numberOfValues) / 4096;

    if (value >= numberOfValues)
        value = numberOfValues - 1;

    return value;
}