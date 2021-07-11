#include <Arduino.h>
#ifdef ESP8266 
#include <ESP8266WiFi.h>
#endif 
#ifdef ESP32   
       #include <WiFi.h>
#endif

#include "Wire.h"
#include <NTPClient.h>
#include <WiFiUdp.h>

#define COVER D5
#define ALARM D6
#define LIGHT D3

#define WIFI_SSID "Dispositivos"
#define WIFI_PASS "1938@picodojabre!"
#define BAUD_RATE  9600                     // Change baudrate to your need

#define TIMER 30 // Seconds
#define START_DELAY 10 // Seconds
#define LOOP_DELAY 500 // Miliseconds
int timer = TIMER;
int state = 0;

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);


void setupWiFi() {
  Serial.printf("\r\n[Wifi]: Connecting");
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.printf(".");
    delay(250);
  }
  IPAddress localIP = WiFi.localIP();
  Serial.printf("\r\nconnected!\r\n[WiFi]: IP-Address is %d.%d.%d.%d\r\n", localIP[0], localIP[1], localIP[2], localIP[3]);
}


void setup() {
  Serial.begin(BAUD_RATE); Serial.printf("\r\n\r\n");
  setupWiFi();
  pinMode(COVER, INPUT_PULLUP);
  pinMode(ALARM, INPUT_PULLUP);
  pinMode(LIGHT, OUTPUT);
  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(LIGHT, HIGH);
  Wire.begin();

  // Initialize a NTPClient to get time
  timeClient.begin();
  // Set offset time in seconds to adjust for your timezone, for example:
  // GMT +1 = 3600
  // GMT +8 = 28800
  // GMT -1 = -3600
  // GMT 0 = 0
  timeClient.setTimeOffset(-3*3600);
  delay(START_DELAY*1000); 
}

void loop() {
  timeClient.update();
  int currentHour = timeClient.getHours();
  Serial.printf("\rCOVER PIN: %d ALARM PIN: %d TIMER: %d secs HOURS: %d STATE: %d TIME: ", digitalRead(COVER), digitalRead(ALARM), timer, currentHour, state);
  Serial.print(timeClient.getFormattedTime()); 
  if (((digitalRead(COVER) == HIGH)||(digitalRead(ALARM) == HIGH))) { // &&((currentHour >= 18)||(currentHour < 5))
    digitalWrite(LIGHT, LOW);
    timer = TIMER;  
    state = 1;
  }

  if (state == 1) {
    timer--; 
    }
    
  if (timer == 0) {
    timer = TIMER;
    state = 0;
    digitalWrite(LIGHT, HIGH);
  }

 delay(LOOP_DELAY);
  //digitalWrite(BUILTIN_LED, LOW);
}
