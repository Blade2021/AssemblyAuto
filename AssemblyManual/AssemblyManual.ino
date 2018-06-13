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
#define SENMODELOC 140
#define POSDEFAULT 15
#define DATASPEED 19200
#define SOLARRAYSIZE 2
#define LCD_CLEARLOC_A 130
#define LCD_CLEARLOC_B 131

//Panel Buttons
const byte toggleButton = A2; // toggle Button
// Panel LEDs
const byte errorLed = 13;
//Sensor & Solenoids
const byte mainSensor = A0;
const byte slaveSensor = A1;
const byte solenoidArray[SOLARRAYSIZE + 1] = {11, 12};
/*
const byte crimpSolenoid = 12;
const byte stopperSolenoid = 11;
*/

//LCD Variables
byte sysPosition = 0; // Position of sysArray
//const int lcdClearTime = 3000;
const int lcdClearTime = ((EEPROM.read(LCD_CLEARLOC_A) * 100) + (EEPROM.read(LCD_CLEARLOC_B) * 100));
byte pos = POSDEFAULT;            //LCD position for key input
byte jindx = 0;                   //Key Input Position (Array)
char arraya[5] = {0, 1, 2, 3, 0}; //Key input array
const byte sysLength = 5;         // System timer array length

//Time Controls
unsigned long preLCDClear = 0; // LCD Clear Timer
unsigned long preTimer1 = 0;
//System Time Variables
int sysArray[sysLength] = {1000, 400, 100, 300, 0};
/*AL-0 - Crimp Wait
  AL-1 - Crimp Time
  AL-2 - Sensor Ignore Delay [ MPS ]
  AL-3 - Jam Timer [MPS]
  AL-4 - Sensor Mode
*/
byte stateArray[SOLARRAYSIZE + 1] = {0};

//LiquidCrystal
Adafruit_LiquidCrystal lcd(0);

//Keypad
const byte ROWS = 4; // # of rows for keypad
const byte COLS = 4; // # of columns for keypad
char keys[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};
byte rowPins[ROWS] = {9, 8, 7, 6}; //row pins
byte colPins[COLS] = {5, 4, 3, 2}; //column pins
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

//System Variables
boolean active = LOW; // System active variable
byte toggleLogic = 0;
byte mpsEnable = 0;        // Machine Protection Enabler
const int passcode = 7777; //System override passcode
byte crimpCycle = 0;
byte runCheck = 1;
byte mSensorLogic = 1;
byte slaveSensorLogic = 1;
byte senMode = 0;
int systemCount = 0;

//PC Control
const byte numChars = 14;     // Array character limit
char receivedChars[numChars]; // Recieved bytes from serial input
String apple = "";            // Incoming serial data string
byte debug = 0;
boolean newData = false;
//byte runCheck = 1;
byte mode = 1;

void setup()
{
  //LEDs
  pinMode(errorLed, OUTPUT);
  //Buttons
  pinMode(toggleButton, INPUT_PULLUP);
  //Sensor & Solenoids
  pinMode(mainSensor, INPUT_PULLUP);
  pinMode(slaveSensor, INPUT_PULLUP);
  pinMode(solenoidArray[0], OUTPUT);
  pinMode(solenoidArray[1], OUTPUT);
  // END OF PINMODE

  Serial.begin(DATASPEED);
  Serial.println("Starting...");
  Serial.println(F("Program Version 1.0.0"));
  lcd.begin(20, 4);
  lcd.setCursor(0, 0);
  lcd.print("Run Time: ");
  debug = EEPROM.read(DEBUGMEMLOC);
  mpsEnable = EEPROM.read(MPSMEMLOC);
  senMode = EEPROM.read(SENMODELOC);
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
}

void loop()
{
  unsigned long currentTime = millis();
  lcdControl();

  if (Serial.available() > 0)
  {
    recvWithEndMarker();
  }
  if (newData == true)
  {
    if (debug >= 1)
    {
      Serial.println(F("DEBUG: newData Function ran."));
    }
    checkData();
  }
  toggleLogic = digitalRead(toggleButton); //Check the status of the Toggle Button
  if (sysPosition >= sysLength)
  {
    //Included one for null
    sysPosition = 0;
  }
  // If Toggle Button is pressed or "Active"
  if (((toggleLogic == LOW) || (runCheck == 0)) && mode != 3)
  {
    digitalWrite(errorLed, HIGH);
    mode = 0;
    inactive(sysPosition);
  }
  if ((toggleLogic == HIGH) && (mode != 2))
  {
    mode = 1;
  }
  if (mode == 1)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Run Time:");
    lcd.setCursor(0, 1);
    lcd.print("sMode:");
    lcd.setCursor(6, 1);
    if (senMode == 1)
    {
      lcd.print("Enabled");
    }
    if (senMode == 0)
    {
      lcd.print("Disabled");
    }
    lcd.setCursor(0,2);
    lcd.print("SC:");
    lcd.print(systemCount);
    mode = 2;
  }
  if (mode == 2)
  {
    // If Toggle Button is unpressed or "Inactive"
    digitalWrite(errorLed, LOW);
    mSensorLogic = digitalRead(mainSensor);
    if ((mSensorLogic == LOW) && (crimpCycle == 0))
    {
      systemCount++;
      if (debug >= 2)
      {
        Serial.print(F("Main Cycle Started | SC:"));
        Serial.println(systemCount);
      }
      lcd.setCursor(0, 2);
      lcd.print("SC:");
      lcd.print(systemCount);
      digitalWrite(solenoidArray[0], HIGH);
      preTimer1 = currentTime;
      crimpCycle = 1;
      if (senMode == 1)
      {
        // Go to block detection sequence
        crimpCycle = 1;
      }
      else
      {
        // Skip block detection
        crimpCycle = 5;
      }
    }
    //Block Detection Enabled
    if ((crimpCycle == 1) && (currentTime - preTimer1 >= sysArray[0]))
    {
      slaveSensorLogic = digitalRead(slaveSensor);
      if (slaveSensorLogic == LOW)
      {
        preTimer1 = currentTime;
        crimpCycle = 2;
        Serial.print("Block Detected.  SC:");
        Serial.println(systemCount);
        lcd.setCursor(0, 3);
        lcd.print("Block detected      ");
        preLCDClear = millis();
      }
      else
      {
        digitalWrite(solenoidArray[1], HIGH);
        preTimer1 = currentTime;
        crimpCycle = 6;
      }
    }
    // Wait for Sensor to be cleared
    if(crimpCycle == 2){
      lcd.setCursor(15,2);
      lcd.print((currentTime - preTimer1) / 1000);
      slaveSensorLogic = digitalRead(slaveSensor);
      if(slaveSensorLogic == HIGH){
        crimpCycle = 3;
        preTimer1 = currentTime;
        Serial.print("Block Cleared | SC:");
        Serial.println(systemCount);
        lcd.setCursor(0,3);
        lcd.print("Block Cleared       ");
        preLCDClear = millis();
      }
    }
    // Block Detection - Extra wait timer
    if ((crimpCycle == 3) && (currentTime - preTimer1 >= sysArray[4])){
      slaveSensorLogic = digitalRead(slaveSensor);
      if(slaveSensorLogic == LOW){
        crimpCycle = 2;
        return;
      }
      digitalWrite(solenoidArray[1], HIGH);
      preTimer1 = currentTime;
      crimpCycle = 6;
      lcd.setCursor(15,2);
      lcd.print("     ");
    }
    // Block Detection Disabled
    if ((crimpCycle == 5) && (currentTime - preTimer1 >= sysArray[0]))
    {
      digitalWrite(solenoidArray[1], HIGH);
      preTimer1 = currentTime;
      crimpCycle = 6;
    }
    // Reset Cylinders
    if ((crimpCycle == 6) && (currentTime - preTimer1 >= sysArray[1]))
    {
      digitalWrite(solenoidArray[0], LOW);
      digitalWrite(solenoidArray[1], LOW);
      preTimer1 = currentTime;
      crimpCycle = 0;
    }
  }
  if (mode == 3)
  {
    lcd.setCursor(0, 1);
    lcd.print("OVERRIDE: ON        ");
    //Get keypad input
    char key;
    key = keypad.getKey();
    if (key)
    {
      if(debug >= 2){
        Serial.print("Debug Key: ");
        Serial.println(key);
      }
      switch (key)
      {
      case 'A':
      case 'B':
      case 'C':
      case 'D':
        return;
      case '#':
      case '*':
      case '0':
        mode = 1;
        return;
      default:
        break;
      }
      int trigger = key - '0';
      //Send keypad input to Override_Trigger function
      Override_Trigger(trigger);
    }
  } // End of Else Statment
}
void inactive(byte sysPos)
{
  lcd.setCursor(0, 2);
  lcd.print("Time:");
  digitalWrite(errorLed, HIGH);
  digitalWrite(solenoidArray[0], LOW);
  digitalWrite(solenoidArray[1], LOW);
  crimpCycle = 0;
  switch (sysPos)
  {
  case 0:
    lcd.setCursor(0, 1);
    lcd.print(F("Crimp Wait Delay    "));
    changetime(sysPos);
    break;
  case 1:
    lcd.setCursor(0, 1);
    lcd.print(F("Crimp Time          "));
    changetime(sysPos);
    break;
  case 2:
    lcd.setCursor(0, 1);
    lcd.print(F("Sensor Ignore Delay "));
    changetime(sysPos);
    break;
  case 3:
    lcd.setCursor(0, 1);
    lcd.print(F("Crimp Jam [ MPS ]   "));
    changetime(sysPos);
    break;
  case 4:
    lcd.setCursor(0, 1);
    lcd.print(F("Block Wait Time  "));
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
      if (sysPosition == sysLength)
      {
        sysPosition = 0;
      }
      return;
    }
    if ((key == 'C') || (key == 'c'))
    {
      lcd.clear();
      lcd.setCursor(0, 1);
      lcd.print("Enter Sensor Mode:  ");

      boolean complete = false;
      while (complete == false)
      {
        char key = keypad.getKey();
        if (key)
        {
          switch (key)
          {
          case '0':
            senMode = 0;
            Serial.println("SenMode = 0");
            EEPROM.update(SENMODELOC, 0);
            lcd.setCursor(0, 3);
            lcd.print("Sensor Mode: 0");
            complete = true;
            break;

          case '1':
            senMode = 1;
            Serial.println("SenMode = 1");
            EEPROM.update(SENMODELOC, 1);
            lcd.setCursor(0, 3);
            lcd.print("Sensor Mode: 1");
            complete = true;
            break;

          case '#':
          case '*':
            Serial.println("Premature Exit");
            complete = true;
            break;

          default:
            Serial.print("Key [ ");
            Serial.print(key);
            Serial.println(" ] not accepted");
            break;
          }
        }
      }
      jindx = 0;
    }
    else
    {
      lcd.print(key);
      pos++;
      lcd.setCursor(pos, 2);

      arraya[jindx++] = key;
      arraya[jindx];
      if (pos > 20)
      {
        pos = POSDEFAULT;
      }
    }
    if (key == '*')
    {
      int value = atoi(arraya);
      Serial.print(F("SYSTEM | Keypad Input: "));
      Serial.println(value);
      if ((value == passcode) && (mode == 0))
      {
        /* VERY IMPORTANT!  Check to see if active is 0
          so that override isn't turned on while machine running.  */
        mode = 3;
        pos = POSDEFAULT;
        lcd.setCursor(pos, 2);
        lcd.print("       ");
        jindx = 0;
        return;
      }
      //Function to control to use two sensors or one!
      /*if ((sysPos == 4))
      {
        if (value >= 1)
        {
          value = 1;
          sysArray[4] = value;
          Serial.println("Sensor Mode Changed: ON");
        }
        else
        {
          value = 0;
          sysArray[4] = value;
          Serial.println("Sensor Mode Changed: OFF");
        }
      }*/
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

void eepromWrite(byte arrayLoc, int value)
{
  //int memAddress = (((vector * MEMVECTORMULTIPLE) + arrayLoc) * 2);
  int memAddress = (arrayLoc * 2);
  if (memCheck(memAddress, 12) == true)
  {
    if ((value >= 5100) || (value <= 0))
    {
      Serial.println("EEPROM Function Aborted [REF:3692]");
      return;
    }
    int tempValue = value / 10;
    if ((value <= 2550))
    {
      EEPROM.update(memAddress, tempValue);
      Serial.print("Updating EEPROM Address ( ");
      Serial.print(memAddress);
      Serial.print(" ) with value [ ");
      Serial.print(tempValue);
      Serial.println(" ]");
      memAddress++;
      if (memCheck(memAddress, 13) == false)
      {
        return;
      }
      EEPROM.update(memAddress, 0);
      Serial.print("Updating EEPROM Address ( ");
      Serial.print(memAddress);
      Serial.println(" ) with value [ 0 ]");
    }
    if (value > 2550)
    {
      tempValue = tempValue - 255;
      EEPROM.update(memAddress, tempValue);
      Serial.print("Updating EEPROM Address ( ");
      Serial.print(memAddress);
      Serial.print(" ) with value [ ");
      Serial.print(tempValue);
      Serial.println(" ]");
      memAddress++;
      if (memCheck(memAddress, 14) == false)
      {
        return;
      }
      EEPROM.update(memAddress, 255);
      Serial.print("Updating EEPROM Address ( ");
      Serial.print(memAddress);
      Serial.println(" ) with value [ 255 ]");
    }
  }
}

void memoryLoad()
{
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

boolean memCheck(unsigned int address, byte refID)
{
  if (address > EEPROM.length())
  {
    Serial.print("Memory limit reached. ID( ");
    Serial.print(refID);
    Serial.println(" ) [REF: 4320]");
    return false;
  }
  else
  {
    return true;
  }
}

void Override_Trigger(int RTrigger)
{
  if (RTrigger - 1 < SOLARRAYSIZE)
  {
    boolean tempstate = LOW;
    String lcdstate = "OFF";
    if (stateArray[RTrigger] == 1)
    {
      tempstate = LOW;
      lcdstate = "OFF";
      stateArray[RTrigger] = 0;
    }
    else
    {
      stateArray[RTrigger] = 1;
      lcdstate = "ON";
      tempstate = HIGH;
    }
    digitalWrite(solenoidArray[RTrigger - 1], tempstate);
    lcd.setCursor(0, 3);
    lcd.print("Relay ");
    lcd.print(RTrigger);
    lcd.print(" SET TO: ");
    lcd.print(lcdstate);
    lcd.print(" ");
    Serial.print(F("SYSTEM OVERRIDE | Relay "));
    Serial.print(RTrigger);
    Serial.print(" | ");
    Serial.println(lcdstate);
    preLCDClear = millis();
  }
}

void lcdControl()
{
  lcd.setCursor(0, 0);
  lcd.print("Run Time:");
  lcd.setCursor(10, 0);
  lcd.print(millis() / 1000);
  if (millis() - preLCDClear >= lcdClearTime)
  {
    preLCDClear = millis();
    lcd.setCursor(0, 3);
    lcd.print("                    ");
  }
}

void recvWithEndMarker()
{
  static byte ndx = 0;
  char endMarker = '\n';
  char rc;

  while (Serial.available() > 0 && newData == false)
  {
    rc = Serial.read();

    if (rc != endMarker)
    {
      receivedChars[ndx] = rc;
      ndx++;
      apple = apple += rc;
      if (ndx >= numChars)
      {
        ndx = numChars - 1;
      }
    }
    else
    {
      receivedChars[ndx] = '\0'; // terminate the string
      ndx = 0;
      newData = true;
    }
  }
}
void checkData()
{
  if (newData == true)
  {
    if (apple.length() >= 5)
    {
      if (apple.substring(0, 6) == "EEPROM")
      {
        eepromUpdate();
      }
      if (apple.substring(0, 5) == "DEBUG")
      {
        char voucher = apple.charAt(6);
        byte endingVoucher = voucher - '0';
        if ((endingVoucher >= 0) && (endingVoucher <= 9))
        {
          debug = endingVoucher;
          Serial.print("Debug updated to: ");
          Serial.println(debug);
          EEPROM.update(DEBUGMEMLOC, debug);
        }
        else
        {
          Serial.println(F("Debug value not accepted"));
        }
      }
      if (apple.substring(0, 8) == "OVERRIDE")
      {
        if ((mode == 0) || (mode == 1))
        {
          mode = 2;
          Serial.println("Override:On");
        }
        else
        {
          mode = 0;
          Serial.println("Override:Off");
        }
      }
    }
    newData = false;
    apple = "";
  }
}
void eepromUpdate()
{
  int address = firstValue();
  if (memCheck(address, 10) == false)
  {
    return;
  }
  else
  {
    int eepromValue = lastValue();
    Serial.print("EEPROM.update(");
    Serial.print(address);
    Serial.print(", ");
    Serial.print(eepromValue);
    Serial.println(")");
    EEPROM.update(address, eepromValue);
    memoryLoad();
  }
}
int firstValue()
{
  char masterArray[numChars];
  byte slaveindx;
  byte value_start = apple.indexOf('.');
  if (debug >= 2)
  {
    Serial.print("V Start: ");
    Serial.println(value_start);
  }
  byte value_end = apple.indexOf('.', value_start + 1);
  if (debug >= 2)
  {
    Serial.print("V End: ");
    Serial.println(value_end);
  }
  for (byte k = value_start + 1; k < value_end; k++)
  {
    masterArray[slaveindx] = receivedChars[k];
    slaveindx++;
  }
  masterArray[slaveindx] = '\0';
  Serial.println(masterArray);
  int value = atoi(masterArray);

  if (debug >= 2)
  {
    Serial.print("fvF firstValue: ");
    Serial.println(value);
  }
  return value;
}

int lastValue()
{
  char masterArray[numChars];
  byte slaveindx = 0;
  byte value_end = apple.lastIndexOf('.');
  if (debug >= 2)
  {
    Serial.print("V End (2): ");
    Serial.println(value_end);
  }
  for (byte k = value_end + 1; k < apple.length(); k++)
  {
    masterArray[slaveindx] = receivedChars[k];
    slaveindx++;
  }
  masterArray[slaveindx] = '\0';
  int lastvalue = atoi(masterArray);
  if (debug >= 2)
  {
    Serial.print("lvF lastValue: ");
    Serial.println(lastvalue);
  }
  return lastvalue;
}
