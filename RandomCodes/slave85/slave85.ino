// Code for the ATtiny85
// screen /dev/ttyUSB1 9600
// ctrl+a and K or ctrl+a and :quit
// cu -l /dev/ttyUSB1 -s 9600
// ~. 
// avrdude -c usbasp -p t85
// avrdude -c usbisp -p t85
// avrdude -c usbtiny -p t85

#include <Wire.h>

char response[11] ="0123456789";

void setup()
{
  //Serial.begin(115200);
  Wire.begin(2);
  Wire.onReceive(ReceiveEvent);
  Wire.onRequest(RequestEvent);
  pinMode(4, OUTPUT);
  digitalWrite(4, LOW);
}

void loop()
{
  delay(1);
}

void ReceiveEvent(int count)
{
  //Serial.print("Received[");
  int d = Wire.read();
  while (Wire.available())
  {
   Wire.read();
  }

    
  //Serial.print(c);
  //Serial.println("]");
  //calc response.
  String ts = "0987654321";
  if ((d % 2) == 0) {
   ts = "1234567890";
   digitalWrite(4, LOW);
  } 
  else {
   ts = getTime();
   digitalWrite(4, HIGH);
  }
    
  strncpy(response,ts.c_str(),sizeof(response)-1);
}

void RequestEvent()
{
  /*Serial.print("Sending:[");
  Serial.print(response);
  Serial.println("]");*/
  Wire.write(response);
}

String getTime(){
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;
  String ts = "U:";
  if (hr<10) ts+="0";
  ts += hr;
  ts +=":";
  if ((min%60)<10) ts+="0";
  ts += min % 60;
  ts +=":";
  if ((sec%60)<10) ts+="0";
  ts += sec % 60;
  return(ts);
}
