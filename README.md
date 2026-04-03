# Processing_Node (Risk-V)
# Selection_Node (UNO 1)
# Audio_Node (UNO 2)
# Light_Node (UNO 3)

## RISC-V (processing Node):
Installing Linux Debian to risc-V using a microsd card. The Risc-V Board uses an HDMI cord to connect directly to a monitor. Connect the speaker to the breadboard or the built in 3.5mm audio jack. Then I2C communication is established by connecting GPIO pins from all 3 arduinos to the risc -v. Logic level shifters are used to manage the voltage difference. 

## Arduino 1 (Selection Node):
The remote receiver, selection buttons and the 4pin LCD display are connected to the breadboard. The LCD is connected using SDA to A4, SCL to A5 with a 10k resistor. 

## Arduino 2 (Audio Node):
Attach an electret microphone to the breadboard and connect the GND to the Arduino’s GND, OUT to an analog output (A0-A5), and VCC to 5V. Connect potentiometer + pin to 5v, - pin to GND, and data pin to another analog output (A0-A5). 

## Arduino 3 (Light/Color Node):
The LED lights, buttons, and photoresistor, are connected to the breadboard. The photoresistor connects to A0 with a 10k resistor. The LED are wired  with 220k resistors.. The selection buttons are connected to digital pin 2, 3, 4 with 10k resistors each.
