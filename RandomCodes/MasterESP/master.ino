#include <Wire.h>
#define ADDRESS 0x2
int x=0;

void setup()
{
  Serial.begin(9600); // start serial for output
  Wire.begin();//Change to Wire.begin() for non ESP.
  //Wire.setClockStretchLimit(1500);
}

void loop()
{
 
  Wire.beginTransmission(ADDRESS);
  Wire.write(x);
  Wire.endTransmission();
  x = (x+1)%50;
  delay(200);//Wait for Slave to calculate response.

  Wire.requestFrom(ADDRESS,10);
  Serial.print("Request Return:[");
  while (Wire.available())
  {
    delay(1);
    char c = Wire.read();
    Serial.print(c);
  }
  Serial.print("] ");
  Serial.println(x);
  delay(1000);
}

/*/Code for the Arduino Mega<
#include <Wire.h>
//#include "WindSpeed.h"


//WindSpeed wind;

void setup()
{
  Wire.begin(); // join i2c bus (address optional for master)
  Wire.setClockStretchLimit(1500);
  Serial.begin(9600); // start serial for output

  Serial.println("alive");
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
}

void loop()
{
  Serial.println("loop");

 // Serial.println(wind.getSpeed());
 
  Wire.requestFrom(2, 1);
  int r;
  while (Wire.available())
  {
    r = Wire.read();
  }
  Serial.println(r);
  delay(2000);
} */
