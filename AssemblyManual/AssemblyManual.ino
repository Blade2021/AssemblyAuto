/*  VERSION 1.0.0
    Last succcessful run: 
    Last Upload SHA Token: 
*/
#include <Keypad.h>
#include <Adafruit_LiquidCrystal.h>
#include <Wire.h>
#include <EEPROM.h>

#define MPSMEMLOC 110
#define DEBUGMEMLOC 112
#define VERSIONMEM 770
#define VECTORMEMLOC 100
#define POSDEFAULT 15
#define DATASPEED 19200

//Panel Buttons
const byte toggleButton = A0; // toggle Button
// Panel LEDs
const byte errorLed = 13;
//Sensor & Solenoids
const byte mainSensor = A1;
const byte crimpSolenoid = 4;
const byte stopperSolenoid = 3;

//LCD Variables
byte sysPosition = 0; // Position of sysArray
const int lcdClearTime = 7000;
byte pos = POSDEFAULT; //LCD position for key input
byte jindx = 0; //Key Input Position (Array)
char arraya[] = {0, 1, 2, 3, 0}; //Key input array
const byte sysLength = 5; // System timer array length

//Time Controls
unsigned long preLCDClear = 0; // LCD Clear Timer
unsigned long preTimer1 = 0;
//System Time Variables
int sysArray[sysLength] = {1000, 400, 100, 300, 0};
/*AL-0 - Crimp Wait
  AL-1 - Crimp Time
  AL-2 - Sensor Ignore Delay
  AL-3 - Jam Timer [MPS]
*/

//LiquidCrystal
Adafruit_LiquidCrystal lcd(0);

//Keypad
const byte ROWS = 4; // # of rows for keypad
const byte COLS = 4; // # of columns for keypad
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {12, 11, 10, 9}; //row pins
byte colPins[COLS] = {8, 7, 6, 5}; //column pins
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

//System Variables
boolean active = LOW; // System active variable
byte toggleLogic = 0;
byte mpsEnable = 0; // Machine Protection Enabler
const int passcode = 7777; //System override passcode
byte crimpCycle = 0;

//PC Control
const byte numChars = 32; // Array character limit
char receivedChars[numChars]; // Recieved bytes from serial input
unsigned long preSerialCheck; // Previous sensor check variable
int senWait = 100; // Sensor data wait time
String apple = ""; // Incoming serial data string
byte initial = 1; //Initial contact toggle
byte debug = 0;
byte runCheck = 1;

void setup()
{
  //LEDs
  pinMode(errorLed, OUTPUT);
  //Buttons
  pinMode(toggleButton, INPUT_PULLUP);
  //Sensor & Solenoids
  pinMode(mainSensor, INPUT_PULLUP);
  pinMode(crimpSolenoid, OUTPUT);
  pinMode(stopperSolenoid, OUTPUT);
  // END OF PINMODE

  Serial.begin(DATASPEED);
  Serial.println("Starting...");
  Serial.println("Program Version 1.0.0");
  lcd.begin(20, 4);
  lcd.setCursor(0, 0);
  lcd.print("Run Time: ");
  debug = EEPROM.read(DEBUGMEMLOC);
  mpsEnable = EEPROM.read(MPSMEMLOC);
  memoryLoad();

  Serial.println(F("*** System Variables ***"));
  Serial.print("LCD Clear Time: ");
  Serial.println(lcdClearTime);
  Serial.print("LCD Default POS: ");
  Serial.println(POSDEFAULT);
  Serial.print("Override Passcode: ");
  Serial.println(passcode);
  Serial.println();
  Serial.println("<Controller Ready>");
  lcd.setCursor(0, 1);
  lcd.print("                    "); 
}

void loop()
{
  unsigned long currentTime = millis();
  toggleLogic = digitalRead(toggleButton); //Check the status of the Toggle Button
  if(sysPosition >= sysLength){
    //Included one for null
    sysPosition = 0;
  }
  // If Toggle Button is pressed or "Active"
  if((toggleLogic == 0) || (runCheck <= 0)){
    digitalWrite(errorLed, HIGH);
    inactive(sysPosition);
  } else {
  // If Toggle Button is unpressed or "Inactive"
    digitalWrite(errorLed, LOW);
    if((mainSensor == LOW) && (crimpCycle == 0)){
      digitalWrite(stopperSolenoid, HIGH);
      preTimer1 = currentTime;
      crimpCycle = 1;
    }
    if((crimpCycle == 1) && (currentTime - preTimer1 >= sysArray[0])){
      digitalWrite(crimpSolenoid, HIGH);
      preTimer1 = currentTime;
      crimpCycle = 2;
    }
    if((crimpCycle == 2) && (currentTime - preTimer1 >= sysArray[1])){
      digitalWrite(crimpSolenoid, LOW);
      digitalWrite(stopperSolenoid, LOW);
      preTimer1 = currentTime;
      crimpCycle = 0;
    }
  }
} //End of LOOP Void

void inactive(byte sysPos)
{
  digitalWrite(errorLed, HIGH);
  digitalWrite(crimpSolenoid, LOW);
  digitalWrite(stopperSolenoid, LOW);
  switch (sysPos)
  {
    case 0:
      lcd.setCursor(0, 1);
      lcd.print("Crimp Wait Delay");
      changetime(sysPos);
      break;
    case 1:
      lcd.setCursor(0, 1);
      lcd.print("Crimp Time");
      changetime(sysPos);
      break;
    case 2:
      lcd.setCursor(0, 1);
      lcd.print("Sensor Ignore Delay");
      changetime(sysPos);
      break;
    case 3:
      lcd.setCursor(0, 1);
      lcd.print("Crimp Jam [ MPS ]");
      changetime(sysPos);
      break;
  } //END OF MAIN SWITCH
} // End of Inactive void

void changetime(int sysPos)
{
  lcd.setCursor(5, 2);
  lcd.print(sysArray[sysPos]);
  lcd.print("      ");
  lcd.setCursor(pos, 2);
  char key = keypad.getKey();
  if (key)
  {
    if ((key == 'A') || (key == 'a'))
    {
      sysPosition++;
    }
    lcd.print(key);
    pos++;
    lcd.setCursor(pos, 2);

    arraya[jindx++] = key;
    arraya[jindx];
    if (pos > 20)
    {
      pos = POSDEFAULT;
    }
    if (key == '*')
    {
      int value = atoi(arraya);
      Serial.print("SYSTEM | Keypad Input: ");
      Serial.println(value);
      if (value > 5100)
      {
        value = 5100;
        Serial.println("WARNING: MAX VALUE HIT");
        lcd.setCursor(0, 3);
        lcd.print("ERROR: MAX VALUE HIT");
        preLCDClear = millis();
      }
      eepromWrite(sysPos, value);
      sysArray[sysPos] = value;
      pos = POSDEFAULT;
      lcd.setCursor(pos, 2);
      lcd.print("      ");
      jindx = 0;
      return;
    }
    if (key == '#')
    {
      pos = POSDEFAULT;
      lcd.setCursor(pos, 2);
      lcd.print("     ");
      jindx = 0;
      return;
    }
  } //End of If(Key)
}
//End of ChangeTime function

void eepromWrite(byte arrayLoc, int value) {
  //int memAddress = (((vector * MEMVECTORMULTIPLE) + arrayLoc) * 2);
  int memAddress = (arrayLoc * 2);
  if (memCheck(memAddress, 12) == true) {
    if ((value >= 5100) || (value <= 0)) {
      Serial.println("EEPROM Function Aborted [REF:3692]");
      return;
    }
    int tempValue = value / 10;
    if ((value <= 2550)) {
      EEPROM.update(memAddress, tempValue);
      Serial.print("Updating EEPROM Address ( ");
      Serial.print(memAddress);
      Serial.print(" ) with value [ ");
      Serial.print(tempValue);
      Serial.println(" ]");
      memAddress++;
      if (memCheck(memAddress, 13) == false) {
        return;
      }
      EEPROM.update(memAddress, 0);
      Serial.print("Updating EEPROM Address ( ");
      Serial.print(memAddress);
      Serial.print(" ) with value [ ");
      Serial.print(tempValue);
      Serial.println(" ]");
    }
    if (value > 2550) {
      tempValue = tempValue - 255;
      EEPROM.update(memAddress, tempValue);
      Serial.print("Updating EEPROM Address ( ");
      Serial.print(memAddress);
      Serial.print(" ) with value [ ");
      Serial.print(tempValue);
      Serial.println(" ]");
      memAddress++;
      if (memCheck(memAddress, 14) == false) {
        return;
      }
      EEPROM.update(memAddress, 255);
      Serial.print("Updating EEPROM Address ( ");
      Serial.print(memAddress);
      Serial.print(" ) with value [ ");
      Serial.print(tempValue);
      Serial.println(" ]");
    }
  }
}

void memoryLoad() {
  //Load EEPROM Memory
  for (byte k = 0; k < sysLength; k++)
  {
    int memAddress = (k * 2);
    int memBlockOne = EEPROM.read(memAddress);
    Serial.print("X23 Address[ ");
    Serial.print(memAddress);
    Serial.print(" ] Value: ");
    Serial.println(memBlockOne);
    memBlockOne = memBlockOne * 10;
    if ((memBlockOne > 2550) || (memBlockOne < 0))
    {
      Serial.print("ERROR | Corrupted memory LOC:");
      Serial.print(memAddress);
      Serial.print(" Result:");
      Serial.println(memBlockOne);
      lcd.setCursor(0, 3);
      lcd.print("MEMCORE:");
      lcd.print(memAddress);
      break;
    }
    memAddress++;
    int memBlockTwo = EEPROM.read(memAddress);
    Serial.print("X24: Address[ ");
    Serial.print(memAddress);
    Serial.print(" ] Value: ");
    Serial.println(memBlockTwo);
    memBlockTwo = memBlockTwo * 10;
    if ((memBlockTwo > 2550) || (memBlockTwo < 0))
    {
      Serial.print("ERROR | Corrupted memory LOC:");
      Serial.print(memAddress);
      Serial.print(" Result:");
      Serial.println(memBlockTwo);
      lcd.setCursor(0, 3);
      lcd.print("MEMCORE:");
      lcd.print(memAddress);
      break;
    }
    sysArray[k] = memBlockOne + memBlockTwo;
    Serial.print("EEPROM[");
    Serial.print(k);
    Serial.print("]: ");
    Serial.println(sysArray[k]);
    delay(10);
  }
}

boolean memCheck(unsigned int address, byte refID) {
  if (address > EEPROM.length()) {
    Serial.print("Memory limit reached. ID( ");
    Serial.print(refID);
    Serial.println(" ) [REF: 4320]");
    return false;
  } else {
    return true;
  }
}
