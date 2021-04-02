#include <Wire.h>
#include <string.h> 
#define ADDRESS 0x2

char response[11] ="0123456789";


int led = PIN_PA0;
int in1 = PIN_PA7;
int in2 = PIN_PA3;
int enable = PIN_PB2;
int button_1 = PIN_PB1;
int button_2 = PIN_PB0;
int pwmOutput = PIN_PB2;

void setup() { 
  Wire.begin(ADDRESS);
  Wire.onReceive(ReceiveEvent);
  Wire.onRequest(RequestEvent);
  Serial.begin(9600);
  Serial.println("Looks good..."); 
  pinMode(button_1, INPUT_PULLUP);
  pinMode(button_2, INPUT_PULLUP);
  pinMode(led, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(enable, OUTPUT);
  
  // Start off
  analogWrite(enable, 255);
  // led on
  digitalWrite(led, HIGH);
  Serial.println("Setup complete.");
}

void loop(){
  while (digitalRead(button_1) == LOW){
      analogWrite(enable, pwmOutput);
      digitalWrite(in1, HIGH);
      digitalWrite(in2, LOW);
      digitalWrite(led, LOW);  
      delay(20);  
      Serial.println("Going forward ----->");
  }
  
  while (digitalRead(button_2) == LOW){
      analogWrite(enable, pwmOutput);
      digitalWrite(in1, LOW);
      digitalWrite(in2, HIGH);
      digitalWrite(led, LOW);  
      delay(20);  
      Serial.println("<----- Going reverse");
  }  

 
  analogWrite(enable, 255);
  digitalWrite(in1, HIGH);
  digitalWrite(in2, HIGH);
  delay(20);
  digitalWrite(led, HIGH);  
  //Serial.print("PWM: "); Serial.print(pwmOutput); Serial.print("\n"); 
 
}


void ReceiveEvent(int count)
{
  Serial.print("Received: [");
  int d = Wire.read();
  while (Wire.available())
  {
   Wire.read();
  }
    
  Serial.print(d);
  Serial.print("]  -> s: [");
  //calc response.
  char s[11] = "0987654321";
  int flag = 0;
  if ((d % 2) == 0) {
   flag = 1; 
   strncpy(s, "9999999999",sizeof(s)-1);
  } 
  else {
   flag = 2;
   strncpy(s, "1111111111",sizeof(s)-1);
  }
  Serial.print(s);
  Serial.print("]  -> Sending: [");    
  strncpy(response, s, sizeof(response)-1);
  Serial.print(response);
  Serial.print("] -> Flag: ");    
  Serial.print(flag);
  Serial.println("]");    
}

void RequestEvent()
{
  Serial.print("Sending:[");
  Serial.print(response);
  Serial.println("]");
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
