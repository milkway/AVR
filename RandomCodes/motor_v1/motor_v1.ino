int led = PA0; // Pin 0
int in1 = PA1;
int in2 = PA2;
int enable = 8;

int button_1 = 9;
int button_2 = 10;

int pressed_1 = false;
int pressed_2 = false;

void setup() {

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

}

void loop(){

  int pwmOutput = 0;

  // Read button - Debounce
  if (digitalRead(button_1) == LOW) {
    pressed_1 = !pressed_1;
  }
  while (digitalRead(button_1) == LOW);
  delay(20);

    // Read button - Debounce
  if (digitalRead(button_2) == LOW) {
    pressed_2 = !pressed_2;
  }
  while (digitalRead(button_2) == LOW);
  delay(20);


  if (pressed_1 == true & pressed_2 == true){
      analogWrite(enable, 255);
      delay(1000/16);
      digitalWrite(in1, HIGH);
      digitalWrite(in2, HIGH);
      digitalWrite(led, HIGH);  
      pressed_1 = false;
      pressed_2 = false;
    }
    else if (pressed_1 == true & pressed_2 == false){
    analogWrite(enable, pwmOutput);
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    digitalWrite(led, LOW);  
      } 
      else if (pressed_1 == false & pressed_2 == true){
      analogWrite(enable, pwmOutput);
      digitalWrite(in1, LOW);
      digitalWrite(in2, HIGH);
      digitalWrite(led, LOW);
      }
      else {
      analogWrite(enable, 255);
      digitalWrite(in1, HIGH);
      digitalWrite(in2, HIGH);
      delay(1000/16);
      digitalWrite(led, HIGH);  
      }
}
