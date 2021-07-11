#include <Arduino.h>
#ifdef ESP8266 
       #include <ESP8266WiFi.h>
#endif 
#ifdef ESP32   
       #include <WiFi.h>
#endif

#include "Wire.h"

#define ADDRESS 0x2
#define OPEN_BUTTON D5
#define CLOSE_BUTTON D6
#define LED D7

#include "SinricPro.h"
#include "SinricProGarageDoor.h"

bool myDoorState = false;
unsigned long lastBtnPress_Open = 0;
unsigned long lastBtnPress_Close = 0;

#define WIFI_SSID "Dispositivos"
#define WIFI_PASS "1938@picodojabre!"
#define APP_KEY           "64b1a680-498e-41a4-ac57-685985b34377"      // Should look like "de0bxxxx-1x3x-4x3x-ax2x-5dabxxxxxxxx"
#define APP_SECRET        "ec81b852-8021-4aef-b592-b0e4e4a92b95-a29fa9d8-1b89-432d-826b-5d04f3768d47"   // Should look like "5f36xxxx-x3x7-4x3x-xexe-e86724a9xxxx-4c4axxxx-3x3x-x5xe-x9x3-333d65xxxxxx"
#define GARAGEDOOR_ID     "6069be4748ccc14a46748a79"    // Should look like "5dc1564130xxxxxxxxxxxxxx"
#define BAUD_RATE         9600                     // Change baudrate to your need

int x=0;

bool onDoorState(const String& deviceId, bool &doorState) {
  Serial.printf("\r\nGaragedoor is %s now.\r\n", doorState?"closed":"open");
  if (doorState) {
    sendOpenCloseDoor("C");
    digitalWrite(LED, LOW);
    } 
  else {
    sendOpenCloseDoor("O");
    digitalWrite(LED, HIGH);
    }
  return true;
}


void handleButtonPress() {
  unsigned long actualMillis = millis(); // get actual millis() and keep it in variable actualMillis
  if (digitalRead(OPEN_BUTTON) == LOW && actualMillis - lastBtnPress_Open > 1000)  { // is button pressed (inverted logic! button pressed = LOW) and debounced?
    //if (myDoorState) {     // flip myPowerState: if it was true, set it to false, vice versa
      myDoorState = false;
    //} else {
    //  myDoorState = true;
    //}
    sendOpenCloseDoor("O");
    digitalWrite(LED, myDoorState?LOW:HIGH); // if myPowerState indicates device turned on: turn on led (builtin led uses inverted logic: LOW = LED ON / HIGH = LED OFF)

    // get Switch device back
    SinricProGarageDoor& myGarageDoor = SinricPro[GARAGEDOOR_ID];
  
    // send powerstate event
    myGarageDoor.sendDoorStateEvent(myDoorState); // send the new doorState to SinricPro server
    Serial.printf("\r\nDevice %s now is  %s (manually via flashbutton)\r\n", myGarageDoor.getDeviceId().toString().c_str(), myDoorState?"fechado":"aberto");

    lastBtnPress_Open = actualMillis;  // update last button press variable
  }

  if (digitalRead(CLOSE_BUTTON) == LOW && actualMillis - lastBtnPress_Close > 1000)  { // is button pressed (inverted logic! button pressed = LOW) and debounced?
    //if (myDoorState) {     // flip myPowerState: if it was true, set it to false, vice versa
    //  myDoorState = false;
    //} else {
      myDoorState = true;
    //}
    sendOpenCloseDoor("C");
    digitalWrite(LED, myDoorState?LOW:HIGH); // if myPowerState indicates device turned on: turn on led (builtin led uses inverted logic: LOW = LED ON / HIGH = LED OFF)
    // get Switch device back
    SinricProGarageDoor& myGarageDoor = SinricPro[GARAGEDOOR_ID];
  
    // send powerstate event
    myGarageDoor.sendDoorStateEvent(myDoorState); // send the new doorState to SinricPro server
    Serial.printf("\r\nDevice %s now is  %s (manually via flashbutton)\r\n", myGarageDoor.getDeviceId().toString().c_str(), myDoorState?"fechado":"aberto");

    lastBtnPress_Close = actualMillis;  // update last button press variable
  } 
}


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

void setupSinricPro() {
  SinricProGarageDoor &myGarageDoor = SinricPro[GARAGEDOOR_ID];
  myGarageDoor.onDoorState(onDoorState);

  // setup SinricPro
  SinricPro.onConnected([](){ Serial.printf("Connected to SinricPro\r\n"); }); 
  SinricPro.onDisconnected([](){ Serial.printf("Disconnected from SinricPro\r\n"); });
  SinricPro.begin(APP_KEY, APP_SECRET);
}

void setup() {
  Serial.begin(BAUD_RATE); Serial.printf("\r\n\r\n");
  setupWiFi();
  setupSinricPro();
  pinMode(LED, OUTPUT);
  pinMode(OPEN_BUTTON, INPUT);
  pinMode(CLOSE_BUTTON, INPUT);
  Wire.begin();
}


void sendOpenCloseDoor(char  *action) { // 'O'to open and 'C' to close
  Serial.print("Request: [");
  Serial.print(action);
  Wire.beginTransmission(ADDRESS);
  Wire.write(action);
  Wire.endTransmission();
  delay(200);//Wait for Slave to calculate response.
  Wire.requestFrom(ADDRESS, 1); // 
  Serial.print("] Return: [");
  while (Wire.available())
  {
    delay(1);
    char c = Wire.read();
    Serial.print(c);
  }
  Serial.print("]");  
  }

void loop() {
  SinricPro.handle();
  handleButtonPress();

//  delay(200);
}
