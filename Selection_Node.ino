// Selection Node - IR Remote Input with LCD Display
// Sends composed message to Processing Node (RISC-V) over hardware UART
// as newline-delimited tagged text:  "SEL:<message>\n"
//
// Wiring (RISC-V link):
//   Uno D1 (TX) ---> 5V<->3.3V logic level shifter (HV side -> LV side)
//                     ---> RISC-V GPIO UART RX
//   Uno GND  <-----> RISC-V GND   (mandatory common ground)
//   Uno D0 (RX) is left unconnected; this node does not receive.
//
// D0/D1 are shared with the USB-Serial bridge on Uno R3. Disconnect
// the D1 wire from the level shifter before uploading a new sketch, and
// do not open the Arduino Serial Monitor while the RISC-V is also reading
// the line

#include <LiquidCrystal.h>
#include <IRremote.h>

#define IR_RECEIVE_PIN 2 // IR receiver signal pin
// LCD pins: RS, E, D4, D5, D6, D7
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

// Serial link config
#define LINK_BAUD 9600

// Message Buffer
char messageBuffer[17]; // 16 chars max + null terminator
uint8_t msgIndex = 0; // current cursor position in buffer

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

// Setup
void setup() {
  // Hardware UART -> RISC-V Processing Node.
  // Keep this clean: only "SEL:<message>\n" lines should go out.
  Serial.begin(LINK_BAUD);

  // LCD init
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(">"); // typing prompt on row 0

  // IR init
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);

  // Clear buffer
  memset(messageBuffer, 0, sizeof(messageBuffer));
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
  // Push one framed line to the RISC-V over UART:  "SEL:<message>\n"
  // Input is restricted to digits 0-9 by decodeIRToChar(), so the payload
  // never contains '\n' or other control bytes that would break framing.
  Serial.print("SEL:");
  Serial.print(messageBuffer);
  Serial.print('\n');

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
