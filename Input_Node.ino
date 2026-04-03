// Selection Node - IR Remote Input with LCD Display
// Sends composed message to Processing Node via I2C

#include <Wire.h>
#include <LiquidCrystal.h>
#include <IRremote.h>

#define IR_RECEIVE_PIN 2 // IR receiver signal pin
// LCD pins: RS, E, D4, D5, D6, D7
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

// I2C Config
#define SLAVE_ADDR 0x08 // ARDUINO_1_ADDR on Processing Node
#define PACKET_LENGTH 6 // Processing Node's PACKET_LENGTH

// Message Buffer
char messageBuffer[17]; // 16 chars max + null terminator
uint8_t msgIndex = 0; // current cursor position in buffer

// I2C Transmit Buffer
volatile uint8_t txBuffer[PACKET_LENGTH];
volatile bool newDataReady = false;

// States
enum NodeState {
  STATE_TYPING, // user is composing message
  STATE_SENT  // message sent, showing confirmation on LCD
};
NodeState currentState = STATE_TYPING;

unsigned long sentTimestamp = 0;
const unsigned long SENT_DISPLAY_MS = 1500;  // how long "Sent!" stays on screen

// IR Code Mapping

char decodeIRToChar(uint32_t code) {
  switch (code) {
    // Number buttons -> characters or digits
    case 0x16: return '0';
    case 0x0C: return '1';
    case 0x18: return '2';
    case 0x5E: return '3';
    case 0x08: return '4';
    case 0x1C: return '5';
    case 0x5A: return '6';
    case 0x42: return '7';
    case 0x52: return '8';
    case 0x4A: return '9';
    default: return '\0';  // key is unknown
  }
}

// (random values, to be replaced)
#define IR_CODE_UP     0x18   // Arrow UP - Confirm / send
#define IR_CODE_DOWN   0x52   // Arrow DOWN - Delete last char

// I2C Request Handler
// Called when Processing Node does Wire.requestFrom()
void onI2CRequest() {
  Wire.write((const uint8_t*)txBuffer, PACKET_LENGTH);
    noInterrupts();
    newDataReady = false; // clear flag after sending
    interrupts();
}

// Setup 
void setup() {
  Serial.begin(9600);  // debug

  // LCD init
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(">"); // typing prompt on row 0

  // IR init
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);

  // I2C slave init
  Wire.begin(SLAVE_ADDR);
  Wire.onRequest(onI2CRequest);

  // Clear buffers
  memset(messageBuffer, 0, sizeof(messageBuffer));
  memset((void*)txBuffer, 0, PACKET_LENGTH);
}

// Loop
void loop() {
  switch (currentState) {

    case STATE_TYPING:
      handleTypingState();
      break;

    case STATE_SENT:
      handleSentState();
      break;
  }
}

//  State Handlers

void handleTypingState() {
  if (!IrReceiver.decode()) return;

  uint32_t code = IrReceiver.decodedIRData.command;
  IrReceiver.resume();  // ready for next press

  if (code == IR_CODE_UP) {
    // Arrow UP -> Confirm & Send
    sendMessage();
    return;
  }

  if (code == IR_CODE_DOWN) {
    // Arrow DOWN -> Delete last char
    if (msgIndex > 0) {
      msgIndex--;
      messageBuffer[msgIndex] = '\0';
      updateLCDTyping();
    }
    return;
  }

  // Normal character 
  char c = decodeIRToChar(code);
  if (c != '\0' && msgIndex < 16) {
    messageBuffer[msgIndex] = c;
    msgIndex++;
    messageBuffer[msgIndex] = '\0';
    updateLCDTyping();
  }
}

void handleSentState() {
  // Show "Sent!" briefly, then return to typing
  if (millis() - sentTimestamp >= SENT_DISPLAY_MS) {
    currentState = STATE_TYPING;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(">");
  }
}

//  Helpers 

void updateLCDTyping() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(">");
  lcd.print(messageBuffer);

  // Row 2: show char count
  lcd.setCursor(0, 1);
  lcd.print("Len: ");
  lcd.print(msgIndex);
  lcd.print("/16");
}

void sendMessage() {
  // Pack message into txBuffer for I2C
  noInterrupts();
  for (int i = 0; i < PACKET_LENGTH; i++) {
    if (i < msgIndex) {
      txBuffer[i] = (uint8_t)messageBuffer[i];
    } else {
      txBuffer[i] = 0;
    }
  }
  newDataReady = true;
  interrupts();

  // Show confirmation on LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Sent!");
  lcd.setCursor(0, 1);
  lcd.print(messageBuffer);

  // Reset message buffer for next input
  memset(messageBuffer, 0, sizeof(messageBuffer));
  msgIndex = 0;

  sentTimestamp = millis();
  currentState = STATE_SENT;
}
