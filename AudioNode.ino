#include <Wire.h>

// pin connections
const int micPin  = A0;
const int potPin = A3;
// for storing readings
int micVal  = 0;
int potVal = 0;

void setup() {
  Wire.begin(5); // join risv at address (may change)
  Wire.onRequest(requestEvent(micValue,potValue));
  // for checking serial monitor7
  Serial.begin(9600);
  Serial.println(F("Initialize System"));
  //Init Microphone
  pinMode(micPin, INPUT);
  pinMode(potPin, INPUT);
}

void loop() {
  
  micVal = analogRead(micPin); // get mic value
  Serial.print(F("mic val ")); Serial.println(micVal);
  if (micVal > 600) { // mic is on
    Serial.println("mic detected");
  }
  potVal = analogRead(potPin); // get potentiometer value
}

void requestEvent(int micValue, potValue) {
  Wire.write(micValue);
  Wire.write(potValue);
}