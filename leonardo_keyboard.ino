// Arduino Pro Micro (Leonardo)
// Keyboard

#include <Arduino.h>
#include <RotaryEncoder.h>
#include "HID-Project.h"
#include <EEPROM.h>

#define PIN_IN1 A2
#define PIN_IN2 A3

#define s1Led A1
#define s5Led A0

#define encoderButton 14
#define s1Button 4
#define s2Button 5
#define s3Button 6
#define s4Button 7
#define s5Button 8
#define s6Button 9
#define s7Button 10
#define s8Button 16

#define INTERWAL_1sek  1000
unsigned long time_1 = 0;
int twoMinutes = 0;

String str = "Cis";  // Start string from RS

RotaryEncoder *encoder = nullptr;

// This interrupt routine will be called on any change of one of the input signals
void checkPosition()
{
  encoder->tick(); // just call tick() to check the state.
}

// To toggle led pins
void inline digitalToggle(int pin) {
  digitalWrite(pin, !digitalRead(pin));
}

void setup()
{
  // open the serial port:
  Serial.begin(9600);
  Serial.setTimeout(20);

  // setup the rotary encoder functionality
  encoder = new RotaryEncoder(PIN_IN1, PIN_IN2, RotaryEncoder::LatchMode::TWO03);

  // register interrupt routine
  attachInterrupt(digitalPinToInterrupt(PIN_IN1), checkPosition, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PIN_IN2), checkPosition, CHANGE);

  pinMode(encoderButton, INPUT_PULLUP);
  pinMode(s1Button, INPUT_PULLUP);
  pinMode(s2Button, INPUT_PULLUP);
  pinMode(s3Button, INPUT_PULLUP);
  pinMode(s4Button, INPUT_PULLUP);
  pinMode(s5Button, INPUT_PULLUP);
  pinMode(s6Button, INPUT_PULLUP);
  pinMode(s7Button, INPUT_PULLUP);
  pinMode(s8Button, INPUT_PULLUP);

  pinMode(s1Led, OUTPUT);
  pinMode(s5Led, OUTPUT);

  digitalWrite(s1Led, 1);
  digitalWrite(s5Led, 1);

  //Read settings from eeprom
  str = read_String(0);
  //If eeprom was never programmed 
  if(str.length() < 1) str = "Ardiuno!";

  // Sends a clean report to the host. This is important on any Arduino type.
  Mouse.begin();
  Keyboard.begin();

  time_1 = millis();
}

void loop()
{
  static int pos = 0;

  // ************ Execute every 1 second ************ 
  if ((millis() - time_1) >= INTERWAL_1sek) {
    time_1 = millis();
    twoMinutes++;  
    if (twoMinutes > 119 ) {
      twoMinutes = 0;
     if (!digitalRead(s1Led)) {
          Keyboard.write(KEY_SCROLL_LOCK);
          Keyboard.releaseAll();
     }
    }
  }

  // ************ Encoder routines ************ 
  encoder->tick(); // just call tick() to check the state.

  int newPos = encoder->getPosition();
  if (pos != newPos) {
    pos = newPos;
    if ((int)(encoder->getDirection())==1) {
    // 1 up, 255 down - the slowest is enough
    Mouse.move(0, 0, 1);
    }else{
    // 1 up, 255 down - the slowest is enough
    Mouse.move(0, 0, 255);
    }
  }
    
  // ************ Keys routines ************
  // Write single keys, do not use a number here! 
  if (!digitalRead(encoderButton)) {
    Keyboard.press(KEY_LEFT_ALT);
    Keyboard.releaseAll();
    delay(300);
  }

  if (!digitalRead(s1Button)) {
    digitalToggle(s1Led);
    twoMinutes = 0;
    Keyboard.write(KEY_SCROLL_LOCK);
    Keyboard.releaseAll();
    delay(300);
  }

    if (!digitalRead(s2Button)) {
    Keyboard.press(KEY_LEFT_ALT);
    Keyboard.write(KEY_PRINTSCREEN);
    Keyboard.releaseAll();
    delay(300);
  }

  if (!digitalRead(s3Button)) {
    Keyboard.press(KEY_LEFT_CTRL);
    Keyboard.write(KEY_S);
    Keyboard.releaseAll();
    delay(300);
  }

  if (!digitalRead(s4Button)) {
    Keyboard.write(KEY_F12);
    Keyboard.releaseAll();
    delay(300);
  }

   if (!digitalRead(s5Button)) {
    digitalToggle(s5Led);
    delay(300);
  }

    if (!digitalRead(s6Button)) {
    Keyboard.press(KEY_LEFT_ALT);
    Keyboard.write(KEY_TAB);
    Keyboard.releaseAll();
    delay(300);
  }

  if (!digitalRead(s7Button)) { 
    Keyboard.print(str);
    if (!digitalRead(s5Led)) {
      Keyboard.write(KEY_ENTER);
      Keyboard.write(KEY_DOWN_ARROW);
      Keyboard.write(KEY_ENTER);
    } 
    Keyboard.releaseAll();
    delay(300);
  }

  if (!digitalRead(s8Button)) {
    Keyboard.press(KEY_LEFT_CTRL);
    Keyboard.press(KEY_LEFT_SHIFT);
    Keyboard.write(KEY_F);
    Keyboard.releaseAll();
    delay(300);
  } 

  // ************ Port RS routines ************ 
  if (Serial.available() > 0){
    str = Serial.readString();
    str.trim();
    if(!digitalRead(s5Led)) {
      writeString(0, str);     //save recived data to eeprom if led nr 5 is On - start on addres 0
    }
  }
} 

// // ************ Save to EEPROM routine ************ 
void writeString(char addres, String dataString)
{
  int stringSize = dataString.length();
  if(stringSize > 100) stringSize = 100; 
  int i;
  for(i = 0; i < stringSize; i++)
  {
    EEPROM.write( addres + i, dataString[i]);
  }
  EEPROM.write(addres + stringSize, '\0');   //Add termination null character at the end of dataString
}

// ************ Read from EEPROM routine ************ 
String read_String(char addres)
{
  int i;
  char dataString[101]; //Max 100 Bytes
  int charIndex = 0;
  unsigned char k;
  // k = EEPROM.read(addres);
  while(k != '\0' && charIndex < 101)   //Read until null character, max 100 characters
  {    
    k = EEPROM.read(addres + charIndex);
    dataString[charIndex] = k;
    charIndex++;
  }
  dataString[charIndex]='\0';
  return String(dataString);
}

