#include <TVout.h>
#include <font6x8.h>

TVout TV;
String inputString = "";         // A string to hold incoming data

void setup() {
  TV.begin(NTSC, 120, 96); 
  TV.select_font(font6x8);
  Serial.begin(115200);  
  TV.println("NanoOS v0.2");
  TV.draw_line(0, 10, 120, 10, WHITE);
  TV.set_cursor(0, 12);
}

void loop() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    
    if (inChar == '\n' || inChar == '\r') {
      TV.println(inputString.c_str());
      inputString = "";
    } else {
      inputString += inChar;
    }
    
    delayMicroseconds(10);
  }
}