#include <Arduino.h>
#include <WiFi.h>
#include <time.h>
#include <vector>
#include <potentiometer.h>
#include <clock.h>
#include <alarms.h>
#include <menu.h>
#include <firebase_config.h>
#include <WebServer.h>

const char* ssid = "DIGI-33AS"; //"DIGI-33AS";
const char* password = "ehhCFYFhTt"; //"ehhCFYFhTt";

int menuState = 0;
WebServer server(80);

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

    Serial.print("Firebase project: ");
    Serial.println(FIREBASE_PROJECT_ID);

    server.on("/", HTTP_GET, []() {
        server.send(200, "text/html",
            "<h1>ESP32 LED Control</h1>"
            "<form action='/led/on' method='post'><button>LED On</button></form>"
            "<form action='/led/off' method='post'><button>LED Off</button></form>");
    });

    server.on("/led/on", HTTP_POST, []() {
          digitalWrite(18, HIGH);
          server.sendHeader("Location", "/");
          server.send(303, "text/plain", "");
    });

    server.on("/led/off", HTTP_POST, []() {
        digitalWrite(18, LOW);
        server.sendHeader("Location", "/");
        server.send(303, "text/plain", "");
    });

    server.begin();

}

void loop() 
{
  server.handleClient();
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
    handleAlarmSettingsMenu();
    handleRepeatMenu();
  }

  // add scheduele on UI for the current day
  // make scheduele for the current day show only the tasks for the next 3 hours
  // make tasks have different characteristics - urgent, important, daily, pills, optional
  // make important/urgent task for any other day constantly be shown on screen
  // make phone connection to send tasks and alarms, here will be imported the date, time and other characteristics of these
  // add scheduele menu - scheduele tasks will only be changed from the app