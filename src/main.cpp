#include <Arduino.h>
#include <WiFi.h>
#include <time.h>
#include <vector>
#include <potentiometer.h>
#include <clock.h>
#include "alarms.h"
#include "menu.h"


const char* ssid = "MEK_2.4G"; //"DIGI-33AS";
const char* password = "2pc8X64w"; //"ehhCFYFhTt";

int menuState = 0;

void setup() {
    Serial.begin(115200);
    delay(1000);

    pinMode(18, OUTPUT); // led
    pinMode(17,OUTPUT); // buzzer
    pinMode(03,INPUT_PULLUP); //button right
    pinMode(02,INPUT_PULLUP); // button left

    Serial.println("Connecting to Wi-Fi...");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println();
    Serial.println("Wi-Fi connected!");
    Serial.print("ESP32 IP address: ");
    Serial.println(WiFi.localIP());

    setupClock();
}

void loop() 
{
    if (updateClock())
    {
      if (menuState==0)
        Serial.println("Main Menu");
      if (menuState==1)
        Serial.println("Set Alarms Menu");
      if (menuState==2)
        Serial.println("Check Alarms Menu");
    }

    handleMainMenu();
    handleBackToMainMenu(alarmSet);

    handleSetAlarmMenu();
    checkAlarms();

    handleCheckAlarmsMenu();
  }

  // add cancel/delete option in checking alarms menu // i added this on the site
  // add ring alarm daily/every x day of the week/just once
  // add scheduele on UI for the current day
  // make scheduele for the current day show only the tasks for the next 3 hours
  // make tasks have different characteristics - urgent, important, daily, pills, optional
  // make important/urgent task for any other day constantly be shown on screen
  // make phone connection to send tasks with characteristic date and time
  // add scheduele menu
  // add delete option in menu
  // add characteristic change in menu