int photoPin = A0;

void setup() {
  Serial.begin(9600);
  for (int i = 1; i <= 7; i++) {
    pinMode(i, OUTPUT);
  }
}

unsigned long lastUpdate, updateSpeed = 300;

void loop() {
  int light = analogRead(photoPin);
  
  if ((millis() - lastUpdate) > updateSpeed) {
    
    int lightRaw = analogRead(photoPin);
    int light = map(lightRaw, 1023, 0, 10, 0);

    for (int i = 1; i <= 7; i++) {
      digitalWrite(i, LOW);
    }
    if (light >= 1 && light <= 7) {
      digitalWrite(light, HIGH);
    }

    switch(light){
    case 1:
        Serial.println("dark");
      	break;
    case 2:
        Serial.println("partially dark");
      	break;
    case 3:
        Serial.println("medium");
      	break;
    case 4:
        Serial.println("fully lit");
      	break;
    case 5:
        Serial.println("brightly lit");
      	break;
    }
    
    lastUpdate = millis();
    Serial.println(String(millis()));
  }
}
