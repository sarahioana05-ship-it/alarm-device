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

int alarmHour = 0; //hour of the alarm
int alarmMinute = 0; // min of the alarm

int setMode = 0; // mode for switching between hours and mins when setting alarm


int lastalarmHour=-1; // for printing alarm time setting
int lastalarmMinute=-1;

unsigned long alarmStartTime; // the start of an alarm

unsigned long buttonPressTime; //total time while button was presseed
bool buttonPressed = false; //check if button is still pressed or not

bool alarmSet = false;
bool alarmAlreadyExists = false;
bool alarmRinging=false; //true when alarm rings

int menuState = 0;

int alarmSelection=0; //pot choice for alarms
int lastCheckAlarm=-1;


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

    //left button to return to main
    if((menuState==1 && alarmSet==false) && digitalRead(02)==LOW)
    {
      Serial.println("Back to Main menu");
      menuState=0;

      digitalWrite(18,HIGH);
      delay(100);
      digitalWrite(18,LOW);
      delay(200);
    }

    if(menuState==2 && digitalRead(02)==LOW)
    {
      Serial.println("Back to Main menu");
      menuState=0;

      digitalWrite(18,HIGH);
      delay(100);
      digitalWrite(18,LOW);
      delay(200);
    }

    //menu 1 - set alarm menu
    if (menuState==1)
    {

      if (alarmSet == true)
      {
        if(setMode==0)
          alarmHour =getPotValue(alarmHour,24);
        else
          alarmMinute =getPotValue(alarmMinute/5,12) * 5;

        if (alarmHour != lastalarmHour || alarmMinute != lastalarmMinute)
        {
          Serial.print("Alarm time: ");
          if (alarmHour < 10) Serial.print("0");
          Serial.print(alarmHour);
          Serial.print(":");
          if (alarmMinute < 10) Serial.print("0");
          Serial.println(alarmMinute);

          lastalarmHour=alarmHour;
          lastalarmMinute=alarmMinute;
        }
      }

      //right button - alarm setting
      if (digitalRead(03) == LOW && buttonPressed == false)
      {
          buttonPressTime=millis();
          buttonPressed = true;
      }
      if(digitalRead(03) == HIGH && buttonPressed == true)
      {
          unsigned long press = millis()-buttonPressTime;

          if (alarmSet == false)
          {
            alarmSet = true;
            setMode=0;
            alarmHour = getPotPosition(24);
            lastalarmHour = -1;
            lastalarmMinute = -1;
            Serial.println("Alarm setting mode");
            Serial.println("Adjust hours with the pot");
            digitalWrite(18,HIGH);
            delay(100);
            digitalWrite(18,LOW);
            delay(200);
          }
          else
          {
            if(press >=800)
            {
              if(setMode==0) 
              {
                Serial.println("Adjust minutes with the pot");
                alarmMinute=getPotPosition(12)*5;
                lastalarmMinute = -1;
                setMode=1;
              }
              else 
              {
                Serial.println("Adjust hours with the pot");
                alarmHour = getPotPosition(24);
                lastalarmHour = -1;
                setMode=0;
              }
            }
          
            else
            {
              if (addAlarm(alarmHour, alarmMinute))
              {
                  alarmSet = false;
              }

              digitalWrite(18,HIGH);
              delay(100);
              digitalWrite(18,LOW);
              delay(200);
            }
          }
          buttonPressed = false;
      }

      if (digitalRead(02)==LOW && alarmRinging==true)
          {
            Serial.println("alarm stoped");
            digitalWrite(18, LOW);
            noTone(17);
            alarmRinging=false;

            delay(200);
          }

      //left button cancel the alarm setting
      if (digitalRead(02) == LOW && alarmSet == true)
      {
          Serial.println("cancel");
          alarmSet = false;

          digitalWrite(18,HIGH);
          delay(100);
          digitalWrite(18,LOW);
          delay(200);
      }
    }

    // Check if an alarm should ring
    checkAlarms();

    //menu 2 - check alarms
    if (menuState==2)
    {
      if(alarms.size()==0)
      {
        Serial.println("No existing alarms!");
        Serial.println("Back to main menu");
        menuState=0;
      }

      else
      {
        alarmSelection=getPotValue(alarmSelection,alarms.size());
        if (lastCheckAlarm != alarmSelection)
        {
          if (alarms[alarmSelection].time / 60 <10) Serial.print("0");
          Serial.print(alarms[alarmSelection].time / 60);
          Serial.print(":");
          if (alarms[alarmSelection].time % 60 <10) Serial.print("0");
          Serial.println(alarms[alarmSelection].time % 60);

          if (alarms[alarmSelection].enabled == true)
          Serial.println("enabled");
          else
          Serial.println("disabled");
          lastCheckAlarm=alarmSelection;
        }
      }
    }
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