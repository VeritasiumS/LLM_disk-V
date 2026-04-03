// Processing Node - I2C Input Reader
#include <Wire.h>
#define ARDUINO_1_ADDR 0x08
#define ARDUINO_2_ADDR 0x09
#define ARDUINO_3_ADDR 0x0A

#define PACKET_LENGTH 6

// Timing constants
const unsigned long readInterval = 100;
unsigned long lastReadTime = 0;

// Global buffer for received data
uint8_t receivedData[PACKET_LENGTH];

void setup() {
  Wire.begin();  // Initialize I2C as master (like lab)
  lcd.init();
  lcd.backlight();
}
void loop() {
  if ((millis() - lastReadTime) > readInterval) {
    //read from selection node
    readFromInput(ARDUINO_1_ADDR, "Node1");
    //read from audio node
    readFromInput(ARDUINO_2_ADDR, "Node2");
    
    //read from light/color node
    readFromInput(ARDUINO_3_ADDR, "Node3");
    lastReadTime = millis();  // Update timer (lab style)
  }
}
// helper to read data 
void readFromInput(int inputAddr, const char* nodeName) {
  Wire.requestFrom(inputAddr, PACKET_LENGTH);
  if (Wire.available() == PACKET_LENGTH){
    // Read each byte
    for (int i = 0; i < PACKET_LENGTH; i++) {
      receivedData[i] = Wire.read();
    }    
  }
}
