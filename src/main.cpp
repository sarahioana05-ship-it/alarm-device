#include <Arduino.h>
#include <WiFi.h>
#include <time.h>
#include <vector>


const char* ssid = "DIGI-33AS";
const char* password = "ehhCFYFhTt";
int alarmState=0; //alarm state - cancelled 0 ; set 1 ; ringing 2
int alarmHour = 0; //hour of the alarm
int alarmMinute = 0; // min of the alarm
int setMode = 0; // mode for switching between hours and mins when setting alarm
int lastMinute=-1; //last minute for refreshing the real time print
int timeNow; // real time
int i;
int potValue; // potentiometer
int lastalarmHour=-1; // for printing alarm time setting
int lastalarmMinute=-1;
unsigned long alarmStartTime; // the start of an alarm
unsigned long buttonPressTime; //total time while button was presseed
bool buttonPressed = false; //check if button is still pressed or not
bool alarmset = false;

struct Alarm // the alarm structure that has the following characteristics
{
    int time; // the alarm time
    bool enabled=true; // if the alarm is enabled or not
};

Alarm newAlarm;
std::vector<Alarm> alarms; // vector of alarms

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

void loop() {
    //gettin and showing real time on screen
    struct tm timeinfo;
    getLocalTime(&timeinfo);
    timeNow=timeinfo.tm_hour*60+timeinfo.tm_min;
    if (timeinfo.tm_min != lastMinute)
    {
        Serial.print("Current time: ");
        Serial.print(timeinfo.tm_hour);
        Serial.print(":");
        Serial.println(timeinfo.tm_min);
        lastMinute=timeinfo.tm_min;
    }

    potValue = analogRead(1);
    if (alarmset == true)
    {
      if(setMode==0)
      {
        alarmHour = (potValue * 24L) / 4096;

        if (alarmHour > 23)
            alarmHour = 23;
      }
      else
      {
        alarmMinute = (potValue * 12L) / 4096 * 5;

        if (alarmMinute > 55)
            alarmMinute = 55;
      }

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

        if (alarmset == false)
        {
          alarmset=true;
          setMode=0;
          Serial.println("Alarm setting mode");
          Serial.println("Adjust hours with potentiometer");
        }
        else
        {
          if(press >=800)
          {
            if(setMode==0) setMode=1;
            else setMode=0;
          }
        
          else
          {
              Serial.print("alarm is set for ");
              Serial.print(alarmHour);
              Serial.print(":");
              Serial.println(alarmMinute);
              newAlarm.time=alarmHour*60+alarmMinute;
              alarms.push_back({newAlarm});

              alarmset=false;

              digitalWrite(18,HIGH);
              delay(100);
              digitalWrite(18,LOW);
              delay(200);
          }
        }
        buttonPressed = false;
    }

    if(digitalRead(02) == LOW && alarmset == true)
    {
        Serial.println("cancel");
        alarmset=false;
        alarmHour=0;
        alarmMinute=0;

        digitalWrite(18,HIGH);
        delay(100);
        digitalWrite(18,LOW);
        delay(200);
    }

    // sounding alarm
    for(i=0;i<alarms.size();i++)
    {
        if (timeNow == alarms[i].time && alarms[i].enabled==true)
            {
                Serial.println("alarm! alarm! alarm!");

                digitalWrite(18, HIGH);
                tone(17, 500);

                alarmStartTime=millis();
                alarms[i].enabled=false;
                alarmState=2;
            }
        
        if (alarmState == 2)
        {
          if (digitalRead(02)==LOW)
          {
            Serial.println("alarm stoped");
            digitalWrite(18, LOW);
            noTone(17);
            alarmState=1;

            delay(200);
          }
          else if (millis()-alarmStartTime >= 5000)
          {
            Serial.println("alarm finished");
            digitalWrite(18, LOW);
            noTone(17);
            alarmState=1;
          }
        }
    } 
}
