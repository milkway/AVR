/*
 * Example for SinricPro Motionsensor device:
 * - Setup motionsensor device
 * - Support onPowerState to turn on / turn off motion sensor
 * - Checks a motion sensor connected to MOTIONSENSOR_PIN and send event if state changed
 *
 * If you encounter any issues:
 * - check the readme.md at https://github.com/sinricpro/esp8266-esp32-sdk/blob/master/README.md
 * - ensure all dependent libraries are installed
 *   - see https://github.com/sinricpro/esp8266-esp32-sdk/blob/master/README.md#arduinoide
 *   - see https://github.com/sinricpro/esp8266-esp32-sdk/blob/master/README.md#dependencies
 * - open serial monitor and check whats happening
 * - check full user documentation at https://sinricpro.github.io/esp8266-esp32-sdk
 * - visit https://github.com/sinricpro/esp8266-esp32-sdk/issues and check for existing issues or open a new one
 */

// Uncomment the following line to enable serial debug output
//#define ENABLE_DEBUG

#ifdef ENABLE_DEBUG
       #define DEBUG_ESP_PORT Serial
       #define NODEBUG_WEBSOCKETS
       #define NDEBUG
#endif

#include <Arduino.h>
#ifdef ESP8266
       #include <ESP8266WiFi.h>
#endif
#ifdef ESP32
       #include <WiFi.h>
#endif

//////////////////////////
//#include "Wire.h"
#include <NTPClient.h>
#include <WiFiUdp.h>

#define COVER D5
#define ALARM D6
#define LIGHT D3
#define RELAY_PIN D3 // Pin where the relay is connected (D5 = GPIO 14 on ESP8266)

#define WIFI_SSID "Dispositivos"
#define WIFI_PASS "1938@picodojabre!"
#define BAUD_RATE  9600                     // Change baudrate to your need

#define TIMER 30 // Seconds

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
/////////////////////////

#include "SinricPro.h"
#include "SinricProMotionsensor.h"
#include "SinricProSwitch.h"


#define WIFI_SSID         "Dispositivos"
#define WIFI_PASS         "1938@picodojabre!"
#define APP_KEY           "64b1a680-498e-41a4-ac57-685985b34377"      // Should look like "de0bxxxx-1x3x-4x3x-ax2x-5dabxxxxxxxx"
#define APP_SECRET        "ec81b852-8021-4aef-b592-b0e4e4a92b95-a29fa9d8-1b89-432d-826b-5d04f3768d47"   // Should look like "5f36xxxx-x3x7-4x3x-xexe-e86724a9xxxx-4c4axxxx-3x3x-x5xe-x9x3-333d65xxxxxx"
#define MOTIONSENSOR_ID   "60b3afbf8cf8a303b9397bda"    // Should look like "5dc1564130xxxxxxxxxxxxxx"
#define SWITCH_ID         "60b410a18cf8a303b9397ebd"    // Should look like "5dc1564130xxxxxxxxxxxxxx"
#define BAUD_RATE         9600                // Change baudrate to your need

#define MOTIONSENSOR_PIN  D6                   // PIN where motionsensor is connected to
                                               // LOW  = motion is not detected
                                               // HIGH = motion is detected


bool myPowerState = true;                     // assume device is turned on
bool lastMotionState = false;
bool myLightState = false;
bool lastLightState = false;
unsigned long lastChange = 0;
unsigned long lastLightTime = 0;

/**
 * @brief LIGHT STATE
 *
 * If motion sensor state has changed, send event to SinricPro Server
 * state from digitalRead():
 *      HIGH = motion detected
 *      LOW  = motion not detected
 */
void handleLightState() {

}


/**
 * @brief Checks motionsensor connected to MOTIONSENSOR_PIN
 *
 * If motion sensor state has changed, send event to SinricPro Server
 * state from digitalRead():
 *      HIGH = motion detected
 *      LOW  = motion not detected
 */
void handleMotionsensor() {
  //if (!myPowerState) return;                            // if device switched off...do nothing

  unsigned long actualMillis = millis();
  if (actualMillis - lastChange < 250) return;          // debounce motionsensor state transitions (same as debouncing a pushbutton)

  bool actualMotionState = digitalRead(MOTIONSENSOR_PIN);   // read actual state of motion sensor  
  
  if (actualMotionState != lastMotionState) {         // if state has changed
    lastMotionState = actualMotionState;              // update last known state
    lastChange = actualMillis;                        // update debounce time
    SinricProMotionsensor &myMotionsensor = SinricPro[MOTIONSENSOR_ID]; // get motion sensor device
    if (actualMotionState) {
      myMotionsensor.sendMotionEvent(actualMotionState);
      Serial.printf("Device %s turned %s (via MotionSensor) \r\n", myMotionsensor.getDeviceId().c_str(), "Motion Detected!");       
    if (!myLightState) {
       lastLightState = myLightState;
       myLightState = true;
       lastLightTime = millis();
       SinricProSwitch &mySwitch = SinricPro[SWITCH_ID];   // create new light device
       mySwitch.sendPowerStateEvent(myLightState);   
       Serial.printf("Device %s turned %s (via MotionSensor) \r\n", mySwitch.getDeviceId().c_str(), myLightState?"on":"off");                 
      }
    }
  }
}

/**
 * @brief Callback for setPowerState request
 *
 * @param deviceId      String containing deviceId (useful if this callback used by multiple devices)
 * @param[in] state     bool true=turn on device / false=turn off device
 * @param[out] state    bool true=device turned on / false=device turned off
 * @return true         request handled properly
 * @return false        request can't be handled because some kind of error happened
 */
bool onPowerState(const String &deviceId, bool &state) {
  Serial.printf("Device %s turned %s (via SinricPro) \r\n", deviceId.c_str(), state?"on":"off");
  myPowerState = state;
  return true; // request handled properly
}


bool onPowerStateSwitch(const String &deviceId, bool &state) {  
  Serial.printf("Device %s turned %s (via SinricPro) \r\n", deviceId.c_str(), state?"on":"off");
  myLightState = state;
  lastLightState = myLightState;
  digitalWrite(RELAY_PIN, !state);             // set pin state
  return true;                                // request handled properly
}


// setup function for WiFi connection
void setupWiFi() {
  Serial.printf("\r\n[Wifi]: Connecting");
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.printf(".");
    delay(250);
  }
  IPAddress localIP = WiFi.localIP();
  Serial.printf("connected!\r\n[WiFi]: IP-Address is %d.%d.%d.%d\r\n", localIP[0], localIP[1], localIP[2], localIP[3]);
}

// setup function for SinricPro
void setupSinricPro() {
  // add device to SinricPro
  SinricProMotionsensor& myMotionsensor = SinricPro[MOTIONSENSOR_ID];
  SinricProSwitch& mySwitch = SinricPro[SWITCH_ID];   // create new switch device
  // set callback function to device
  myMotionsensor.onPowerState(onPowerState);
  mySwitch.onPowerState(onPowerStateSwitch);                // apply onPowerState callback
  // setup SinricPro
  SinricPro.onConnected([](){ Serial.printf("Connected to SinricPro\r\n"); });
  SinricPro.onDisconnected([](){ Serial.printf("Disconnected from SinricPro\r\n"); });
  SinricPro.begin(APP_KEY, APP_SECRET);               // start SinricPro
}

// main setup function
void setup() {
  Serial.begin(BAUD_RATE); Serial.printf("\r\n\r\n");

  pinMode(MOTIONSENSOR_PIN, INPUT_PULLUP);
  pinMode(COVER, INPUT_PULLUP);  
  pinMode(LIGHT, OUTPUT);
  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(LIGHT, HIGH);

  setupWiFi();
  setupSinricPro();
  // Initialize a NTPClient to get time
  timeClient.begin();
  // Set offset time in seconds to adjust for your timezone, for example:
  // GMT +1 = 3600
  // GMT +8 = 28800
  // GMT -1 = -3600
  // GMT 0 = 0
  timeClient.setTimeOffset(-3*3600);
}

void loop() {
  handleMotionsensor();
  handleLightState();
  SinricPro.handle();
  timeClient.update();
  int currentHour = timeClient.getHours();
  delay(500);
}
