#include <Arduino.h>
#include <WiFi.h>
#include <time.h>
#include <vector>


const char* ssid = "MEK_2.4G"; //"DIGI-33AS";
const char* password = "2pc8X64w"; //"ehhCFYFhTt";

int alarmHour = 0; //hour of the alarm
int alarmMinute = 0; // min of the alarm

int setMode = 0; // mode for switching between hours and mins when setting alarm

int lastMinute=-1; //last minute for refreshing the real time print
int timeNow; // real time

int i;

int rawValue; // potentiometer

int lastalarmHour=-1; // for printing alarm time setting
int lastalarmMinute=-1;

unsigned long alarmStartTime; // the start of an alarm

unsigned long buttonPressTime; //total time while button was presseed
bool buttonPressed = false; //check if button is still pressed or not

bool alarmSet = false;
bool alarmAlreadyExists = false;
bool alarmRinging=false; //true when alarm rings
bool alarmDuplicate=false;

int menuState=0; // 0- main menu; 1- set alarms; 2- check alarms; 3- alarm details; 4- setting alarm time
int menuSelection=1; // pot choice for menu
int lastmenuSelection=-1;
bool menuSelect = false;

int alarmSelection=0; //pot choice for alarms
int lastCheckAlarm=-1;

struct Alarm // the alarm structure that has the following characteristics
{
    int time; // the alarm time
    bool enabled=true; // if the alarm is enabled or not
};

Alarm newAlarm;
std::vector<Alarm> alarms; // vector of alarms

int getPotValue(int currentValue, int numberOfValues)
{
    rawValue = analogRead(01);
    int range = 4096/numberOfValues;
    int lowerLimit = currentValue * range;
    int upperLimit = (currentValue + 1) * range;

    if (currentValue > 0 && rawValue < lowerLimit - 100)
            currentValue--;
    if (currentValue < numberOfValues - 1 && rawValue > upperLimit + 100)
            currentValue++;

    return currentValue;
}

int getPotPosition(int numberOfValues)
{
    rawValue = analogRead(1);
    int value = (rawValue * numberOfValues) / 4096;

    if (value >= numberOfValues)
        value = numberOfValues - 1;

    return value;
}

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

    configTzTime("EET-2EEST,M3.5.0/3,M10.5.0/4", "pool.ntp.org");

}

void loop() 
{
    //gettin and showing real time on screen
    struct tm timeinfo;
    getLocalTime(&timeinfo);
    timeNow=timeinfo.tm_hour*60+timeinfo.tm_min;
    if (timeinfo.tm_min != lastMinute)
    {
        Serial.print("Current time: ");
        if (timeinfo.tm_hour < 10) Serial.print("0");
        Serial.print(timeinfo.tm_hour);
        Serial.print(":");
        if (timeinfo.tm_min < 10) Serial.print("0");
        Serial.println(timeinfo.tm_min);
        lastMinute=timeinfo.tm_min;

        if (menuState==0)
          Serial.println("Main Menu");
        if (menuState==1)
          Serial.println("Set Alarms Menu");
        if (menuState==2)
          Serial.println("Check Alarms Menu");
    }

    //menu state 0 -> 1/2 set alarms/check alarms
    if (menuState==0 && digitalRead(03)==LOW && menuSelect==false)
    {
      menuSelect = true;
      Serial.println("Select: SET ALARMS || CHECK ALARMS");
      Serial.println("Press Right B to select or Left B to cancel");
      digitalWrite(18,HIGH);
      delay(100);
      digitalWrite(18,LOW);
      delay(200);
    }

    //right button enter the menu selection
    if (menuState==0 && menuSelect==true)
    {
      menuSelection=getPotValue(menuSelection,2);

      if (lastmenuSelection != menuSelection)
      {
        if(menuSelection==0)
          Serial.println("> SET ALARMS");
        else  
          Serial.println("> CHECK ALARMS");

        lastmenuSelection = menuSelection;
      }
      if (digitalRead(03)==LOW)
      {
        menuSelect = false;
        menuState = menuSelection + 1;
        Serial.print("Entered menu: ");
        if (menuState == 1)
          Serial.println("SET ALARMS");
        else if (menuState == 2)
          Serial.println("CHECK ALARMS");
        
        digitalWrite(18,HIGH);
        delay(100);
        digitalWrite(18,LOW);
        delay(200);
      }
    }

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
              alarmDuplicate=false;

              newAlarm.time=alarmHour*60+alarmMinute;
              for(i=0;i<alarms.size();i++)
              {
                if (newAlarm.time==alarms[i].time)
                {
                  Serial.println("Alarm already exists!");
                  Serial.println("Choose another time!");
                  alarmDuplicate=true;
                }
              }

              if (alarmDuplicate==false)
              {
                Serial.print("alarm is set for ");
                if (alarmHour < 10) Serial.print("0");
                Serial.print(alarmHour);
                Serial.print(":");
                if (alarmMinute < 10) Serial.print("0");
                Serial.println(alarmMinute);

                int insertPosition = alarms.size();
                for(i=0; i<alarms.size(); i++)
                {
                    if(newAlarm.time < alarms[i].time)
                    {
                        insertPosition = i;
                        break;
                    }
                }
                alarms.insert(alarms.begin() + insertPosition, newAlarm);

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

    //ringing alarm
    for(i=0;i<alarms.size();i++)
    {
        if (timeNow == alarms[i].time && alarms[i].enabled==true)
            {
                Serial.println("alarm! alarm! alarm!");

                digitalWrite(18, HIGH);
                tone(17, 500);

                alarmStartTime=millis();
                alarms[i].enabled=false;
                alarmRinging=true;
            }
        
        if (alarmRinging == true)
        {
          if (digitalRead(02)==LOW)
          {
            Serial.println("alarm stoped");
            digitalWrite(18, LOW);
            noTone(17);
            alarmRinging=false;

            delay(200);
          }
          else if (millis()-alarmStartTime >= 5000)
          {
            Serial.println("alarm finished");
            digitalWrite(18, LOW);
            noTone(17);
            alarmRinging=false;
          }
        }
    }

    //menu 2 - check alarms
    if (menuState==2)
    {
      alarmSelection=getPotPosition(alarms.size());
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

  // add cancel/delete option in checking alarms menu
  // add ring alarm daily/every x day of the week/just once
  // add scheduele on UI for the current day
  // make scheduele for the current day show only the tasks for the next 3 hours
  // make tasks have different characteristics - urgent, important, daily, pills, optional
  // make important/urgent task for any other day constantly be shown on screen
  // make phone connection to send tasks with characteristic date and time
  // add scheduele menu
  // add delete option in menu
  // add characteristic change in menu