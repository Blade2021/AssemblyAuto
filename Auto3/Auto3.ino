/*  VERSION 1.2.9
    Last succcessful run: 1.2.8
    Last Upload SHA Token: 8de98c
*/
#include <Keypad.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>

#define senArraySize 8
#define solArraySize 9
#define memVectorMultiple 11
#define mpsMemLoc 110

//Panel Buttons
const byte manualButton = 6;
const byte nextButton = 42;
const byte saveButton = 46;
const byte upButton = 48;
const byte downButton = 44;
const byte toggleButton = 50;

// Panel LEDs
const byte panelLed1 = 51;
const byte panelLed2 = 49;
const byte panelLed3 = 47;
const byte panelLed4 = 45;
const byte panelLed5 = 43;
const byte errorLed = 13;
//Sensors
const byte sensorArray[senArraySize] = {A0, A1, A2, A3, A4, A5, A6, A7};
/* SENSOR LIST
   A0 - HookRailEmpty
   A1 - HangerRackFull
   A2 - HookCycleStart
   A3 - CrimpCycleStart
   A4 - HookRailFull
   A5 - StripOffOut
   A6 - HeadDown
   A7 - HeadUp
*/
//Solenoids
const byte solenoidArray[solArraySize] = {7, 8, 16, 17, 18, 19, 15, 14, 9};
/*
    8 - [AL-0] Hanger Feed
    7 - [AL-1] Hook Stopper
   16 - [AL-2] Head/Tooling
   17 - [AL-3] Strip Off
   18 - [AL-4] Crimp Stopper
   19 - [AL-5] Crimp
   15 - [AL-6] Vibrator
   14 - [AL-7] MainAir
   9  - [AL-8] Motor Relay
*/
//LCD Variables
byte sysPosition = 0;
const int lcdClearTime = 7000;
byte pos = 15;
byte jindx = 0;
char arraya [] = {0, 1, 2, 3, 0};
const byte sysLength = 9;

//Time Controls
const int buttonWait = 600;
unsigned long preLCDClear = 0;
unsigned long buttonPreviousTime = 0;
unsigned long previousTimer1 = 0;
unsigned long previousTimer2 = 0;
unsigned long previousTimer3 = 0;
unsigned long previousTimer4 = 0;
unsigned long precountTime = 0;
//System Time Variables
int sysArray[sysLength] = {1000, 1000, 1000, 1000, 2300, 2000, 300, 2000, 1200};
/*AL-0 - Feed Cycle Delay
  AL-1 - Feed Open Delay
  AL-2 - Hook Cycle Delay
  AL-3 - Crimp Cycle Delay
  AL-4 - Crimp Time
  AL-5 - RailCheck Delay
  AL-6 - Sensor Ignore Delay
  AL-7 - Feed/Hook [MPS]
  AL-8 - Head LOC [MPS]
*/
//LiquidCrystal
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

//Keypad
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {25, 27, 29, 31}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {33, 35, 37, 39};
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );


//System Variables
boolean active = LOW;
byte partError = 0;
byte mpsEnable = 0;
byte toggleLogic = 0;
byte feedLoop = 0;
byte feedCheck = 0;
byte feedNext = 0;
byte hookNext = 0;
byte hookLoop = 0;
byte hookCheck = 0;
byte crimpLoop = 0;
byte crimpNext = 0;
byte railCheck = 0;  // Was set to LOW
byte railCheckNext = 0;
byte rswitch = 0;
byte sOverride = 1;
byte stateArray[10] = {0}; //Include extra 0 for the NULL END
const int passcode = 7777;
byte runCheck = 1;  //Initalize as 1 until machine error.
byte mfcount;
byte vector;

//LOGIC CONTROLS
byte logicCount = 0; //Counter of material flow
byte bNextLogic = 0; //Button Next Logic
byte bUpLogic = 0; //Button Up Logic
byte bDownLogic = 0; //Button Down Logic
byte saveButtonLogic = 0; //Save Button Logic
byte manualFeed = 0; //Manual Feed Logic
byte secStart = 0; //Second Start

//PC Control
const byte numChars = 32;
char receivedChars[numChars];
boolean newData = false;
String apple = "";
byte initial = 1;
byte orchard[senArraySize+1] = {0};

void setup() {
  for (byte k; k < 8; k++) {
    digitalWrite(solenoidArray[k], LOW);
    delay(10);
  }
  //LEDs
  pinMode(panelLed1, OUTPUT);
  pinMode(panelLed2, OUTPUT);
  pinMode(panelLed3, OUTPUT);
  pinMode(panelLed4, OUTPUT);
  pinMode(panelLed5, OUTPUT);
  pinMode(errorLed, OUTPUT);
  //Buttons
  pinMode(manualButton, INPUT_PULLUP);
  pinMode(nextButton, INPUT_PULLUP);
  pinMode(saveButton, INPUT_PULLUP);
  pinMode(upButton, INPUT_PULLUP);
  pinMode(downButton, INPUT_PULLUP);
  pinMode(toggleButton, INPUT_PULLUP);
  //Solenoids
  pinMode(solenoidArray[0], OUTPUT);
  pinMode(solenoidArray[2], OUTPUT);
  pinMode(solenoidArray[3], OUTPUT);
  pinMode(solenoidArray[4], OUTPUT);
  pinMode(solenoidArray[5], OUTPUT);
  pinMode(solenoidArray[6], OUTPUT);
  pinMode(solenoidArray[7], OUTPUT);
  pinMode(solenoidArray[1], OUTPUT);
  pinMode(solenoidArray[8], OUTPUT);
  //Photo
  pinMode(sensorArray[2], INPUT_PULLUP);
  pinMode(sensorArray[1], INPUT_PULLUP);
  pinMode(sensorArray[3], INPUT_PULLUP);
  //Prox
  pinMode(sensorArray[0], INPUT_PULLUP);
  pinMode(sensorArray[4], INPUT_PULLUP);
  pinMode(sensorArray[7], INPUT_PULLUP);
  pinMode(sensorArray[6], INPUT_PULLUP);
  pinMode(sensorArray[5], INPUT_PULLUP);
  // END OF PINMODE

  Serial.begin(19200);
  Serial.println("Starting...");
  Serial.println("Program Version 1.2.9");
  lcd.begin(20, 4);
  lcd.setCursor(0, 0);
  lcd.print("Run Time: ");
  lcd.setCursor(2, 1);
  lcd.print("*** BOOTING ***");

  mpsEnable = EEPROM.read(mpsMemLoc);
  vector = EEPROM.read(100);
  Serial.print("Vector: ");
  Serial.println(vector);
  delay(10);
  EEPROM_Read();

  switch(vector){
    case 0:
      lcd.setCursor(17,0);
      lcd.print("412");
      break;
    case 1:
      lcd.setCursor(17,0);
      lcd.print("414");
      break;
    case 2:
      lcd.setCursor(17,0);
      lcd.print("500");
      break;
    default:
      lcd.setCursor(17,0);
      lcd.print("   ");
      break;
  }
  
  Serial.println(F("*** System Variables ***"));
  Serial.print("Button Wait Time: ");
  Serial.println(buttonWait);
  Serial.print("LCD Clear Time: ");
  Serial.println(lcdClearTime);
  Serial.print("LCD Default POS: ");
  Serial.println(pos);
  Serial.print("Override Passcode: ");
  Serial.println(passcode);
  Serial.println();
  lcd.setCursor(0, 1);
  lcd.print("                    ");
}

void loop() {
  if (Serial.available() > 0) {
    recvWithEndMarker();
  }
  if (newData == true) {
    checkData();
  }
  //Main Timer to keep track of entire machine!
  unsigned long currentTime = millis();
  //Call LCD Clear function to clear 4th line of LCD
  lcdControl();
  // Check sOverride  If 0 or 1, It is considered "off"
  if (sOverride == 0 || sOverride == 1) {
    //Run initial reset of all LED's and reset Relay status
    if (sOverride == 0 && active != 0) {
      for (byte indx = 0; indx < 9; indx++) {
        stateArray[indx] = 0;
        Serial.print("Relay status INDEX: ");
        Serial.print(indx);
        Serial.println(" reset.");
      }
      lcd.setCursor(0, 1);
      lcd.print("                    ");
      digitalWrite(panelLed1, LOW);
      digitalWrite(panelLed2, LOW);
      digitalWrite(panelLed3, LOW);
      digitalWrite(panelLed4, LOW);
      digitalWrite(panelLed5, LOW);
      //Reset the count after leaving sOverride or inactive mode
      logicCount = 0;
      runCheck = 1;
      feedNext = 0;
      hookNext = 0;
      crimpNext = 0;
      railCheckNext = 0;
      sOverride = 1; //Exit initial reset
    }
    //Listen for Next Button (Goes through different values inside sysArray)
    bNextLogic = digitalRead(nextButton);
    if ((bNextLogic == LOW) && (currentTime - buttonPreviousTime >= buttonWait)) {
      buttonPreviousTime = currentTime;
      sysPosition++;
      //Reset back to value 0 if you get to the end of the array.
      if (sysPosition >= sysLength) {
        sysPosition = 0;
        Serial.print("Time VAR: ");
        Serial.print(sysPosition + 1);
        Serial.print(" selected. | ");
        Serial.println(sysArray[sysPosition]);
      }
      else {
        Serial.print("Time VAR: ");
        Serial.print(sysPosition + 1);
        Serial.print(" selected. | ");
        Serial.println(sysArray[sysPosition]);
      }
    }
    //Raise the value of the selected variable inside the sysArray (ADD Time)
    bUpLogic = digitalRead(upButton);
    if ((bUpLogic == LOW) && (currentTime - buttonPreviousTime >= buttonWait)) {
      sysArray[sysPosition] = sysArray[sysPosition] + 100;
      buttonPreviousTime = currentTime;
      Serial.print("TimeVar ");
      Serial.print(sysPosition + 1);
      Serial.print(" is now: ");
      Serial.println(sysArray[sysPosition]);
    }
    //Lower the value of the selected variable inside the sysArray (Reduce Time)
    bDownLogic = digitalRead(downButton);
    if ((bDownLogic == LOW) && (currentTime - buttonPreviousTime >= buttonWait)) {
      sysArray[sysPosition] = sysArray[sysPosition] - 100;
      buttonPreviousTime = currentTime;
      Serial.print("TimeVar ");
      Serial.print(sysPosition + 1);
      Serial.print(" is now: ");
      Serial.println(sysArray[sysPosition]);
    }
    //Save the new value to the memory for next reset.
    saveButtonLogic = digitalRead(saveButton);
    if ((saveButtonLogic == LOW) && (currentTime - buttonPreviousTime >= buttonWait)) {
      buttonPreviousTime = currentTime;
      if (sOverride == 0 || sOverride == 1) {
        Serial.print("TimeVar ");
        Serial.print(sysPosition + 1);
        Serial.println(" saved.");
        savetrigger(sysPosition);  //go to savetrigger function to save.
      }
    }
    if ((mpsEnable > 0) && (runCheck == 0)) {
      manualFeed = digitalRead(manualButton);
      if ((manualFeed == LOW) && (currentTime - buttonPreviousTime >= buttonWait)) {
        buttonPreviousTime = currentTime + 2000;
        runCheck = 1;
        digitalWrite(solenoidArray[8], LOW);
        Serial.println("RunCheck reset!");
        lcd.setCursor(0, 3);
        lcd.print("RunCheck Reset!");
      }
    }
    /* Trigger active mode on/off
        active Mode: Sets wether the machine should read the sensors or ignore them.
    */
    toggleLogic = digitalRead(toggleButton);
    if ((toggleLogic == HIGH) && (active == 0)) {
      active = 1;
      digitalWrite(panelLed1, LOW);
      digitalWrite(panelLed2, LOW);
      digitalWrite(panelLed3, LOW);
      digitalWrite(panelLed4, LOW);
      digitalWrite(panelLed5, LOW);
      digitalWrite(errorLed, LOW);
    }
    if (toggleLogic == LOW) {
      active = 0;
      digitalWrite(errorLed, HIGH);
    }
    // active Mode start.  Machine will read sensors and run relays.
    if ((active == 1) && (runCheck == 1)) {
      digitalWrite(solenoidArray[7], HIGH);
      digitalWrite(solenoidArray[9], LOW);
      manualFeed = digitalRead(manualButton);
      feedLoop = digitalRead(sensorArray[2]);
      feedCheck = digitalRead(sensorArray[1]);
      /* feedLoop - Check your main cycle sensor
         error - Check to see if the machine went into error Mode
         secStart - Restart machine after feedCheck activates again
         feedCheck - Check Feed station for material.
         manualFeed - Ignore other variables and trigger on button press
      */
      if (((feedLoop == LOW) && (partError == 0)) || ((secStart == 1) && (feedCheck == LOW)) || (manualFeed == LOW)) {
        if (mpsEnable >= 1) {
          if ((feedNext == 0) && (currentTime - previousTimer1 <= sysArray[7]) && (currentTime - previousTimer1 >= sysArray[6]) && (manualFeed == HIGH)) {
            machStop(0);
            runCheck = 0;
            Serial.println(F("Motor stopped due to ERROR[0032]"));
            Serial.print("preTime: ");
            Serial.print(previousTimer1);
            Serial.print(" - ");
            Serial.print("currentTime: ");
            Serial.print(currentTime);
            Serial.print(" > ");
            Serial.print("varTime: ");
            Serial.println(sysArray[7]);
            previousTimer1 = currentTime;
          }
        }
        if (((feedNext == 0) && (mpsEnable <= 0)) || ((currentTime - previousTimer1 >= sysArray[7]) && (mpsEnable >= 1) && (feedNext == 0)) || ((manualFeed == LOW) && (currentTime - buttonPreviousTime >= buttonWait))) {
          if (manualFeed == LOW) {
            buttonPreviousTime = currentTime;
          }
          // FEED ACTIVATED
          Serial.println("Feed Cycle Activated");
          lcd.setCursor(0, 2);
          lcd.print("Feed Reset:");
          //Start counting time for TimeKeepr function
          if (logicCount == 0) {
            precountTime = currentTime;
          }
          //Check Feed station for material.
          feedCheck = digitalRead(sensorArray[1]);
          if ((feedCheck == HIGH) && (secStart != 1)) {
            Serial.println(F("ERROR: Hanger Rack NOT full."));
            lcd.setCursor(0, 3);
            lcd.print("ERROR: Hanger Rack");
            preLCDClear = currentTime;
            partError = 1;
            secStart = 1;
            lcd.setCursor(11, 2);
            lcd.print("ON ");
          }
          else {
            //Add one to logic count
            logicCount++;
            secStart = 0;
            lcd.setCursor(11, 2);
            lcd.print("OFF");
            partError = 0;
            lcd.setCursor(0, 1);
            lcd.print("SC: ");
            lcd.setCursor(4, 1);
            lcd.print(logicCount);
            lcd.print("  ");
            digitalWrite(panelLed1, HIGH);
            digitalWrite(errorLed, LOW);
            feedNext = 1;
            previousTimer1 = currentTime;
          }
        }
      }
      // FEED OPEN
      if ((feedNext == 1) && (currentTime - previousTimer1 >= sysArray[0])) {
        previousTimer1 = currentTime;
        digitalWrite(solenoidArray[0], HIGH);
        Serial.println("Feed Cycle | FEED OPEN");
        feedNext = 2;
      }
      //FEED CLOSE
      if ((feedNext == 2) && (currentTime - previousTimer1 >= sysArray[1])) {
        Serial.println("Feed Cycle | FEED CLOSE");
        previousTimer1 = currentTime;
        digitalWrite(solenoidArray[0], LOW);
        digitalWrite(panelLed1, LOW);
        feedNext = 0;
      }
      // END OF FEED CYCLE
      // Vibrator Cycle
      railCheck = digitalRead(sensorArray[4]);
      if ((railCheck == HIGH) && (railCheckNext == 0)) {
        Serial.println("Rail Check Activated");
        previousTimer2 = currentTime;
        digitalWrite(panelLed5, HIGH);
        digitalWrite(solenoidArray[6], HIGH);
        railCheckNext = 1;
      }
      if (railCheckNext == 1) {
        railCheck = digitalRead(sensorArray[4]);
        if (railCheck == LOW) {
          previousTimer2 = currentTime;
          railCheckNext = 2;
        }
      }
      if (railCheckNext == 2) {
        railCheck = digitalRead(sensorArray[4]);
        if (railCheck == HIGH) {
          railCheckNext = 1;
        }
        if ((railCheck == LOW) && (currentTime - previousTimer2 >= sysArray[5])) {
          digitalWrite(solenoidArray[6], LOW);
          digitalWrite(panelLed5, LOW);
          previousTimer2 = currentTime;
          Serial.println("Rail Check Finished");
          railCheckNext = 0;
        }
      }
      // END OF VIBRATOR CYCLE
      // Crimp Cycle
      crimpLoop = digitalRead(sensorArray[3]);
      if ((crimpLoop == LOW) && (crimpNext == 0)) {
        Serial.println("Crimp Cycle Activated");
        digitalWrite(panelLed4, HIGH);
        digitalWrite(solenoidArray[4], HIGH);
        previousTimer4 = currentTime;
        crimpNext = 1;
      }
      if ((crimpNext == 1) && (currentTime - previousTimer4 >= sysArray[3])) {
        previousTimer4 = currentTime;
        digitalWrite(solenoidArray[5], HIGH);
        Serial.println("Crimp Cycle | Crimp");
        crimpNext = 2;
      }
      if ((crimpNext == 2) && (currentTime - previousTimer4 >= sysArray[4])) {
        Serial.println("Crimp Cycle | Reset");
        previousTimer4 = currentTime;
        digitalWrite(solenoidArray[5], LOW);
        digitalWrite(solenoidArray[4], LOW);
        digitalWrite(panelLed4, LOW);
        crimpNext = 0;
      }
      // Hook Cycle
      hookLoop = digitalRead(sensorArray[2]);
      if ((hookLoop == LOW) && (hookNext == 0)) {
        if ((mpsEnable <= 1) || ((mpsEnable >= 2) && (currentTime - previousTimer3 >= sysArray[7]))) {
          Serial.println("Hook Cycle Activated");
          digitalWrite(panelLed2, HIGH);
          boolean hookCheck;
          hookCheck = digitalRead(sensorArray[0]);
          if (hookCheck == HIGH) {
            Serial.println("ERROR: Hook Check failed");
            lcd.setCursor(0, 3);
            lcd.print("ERROR: Hook Check");
            preLCDClear = currentTime;
            //partError = 1;
            digitalWrite(panelLed2, LOW);
            feedLoop = 0;
            feedNext = 0;
            digitalWrite(errorLed, HIGH);
          }
          if (hookCheck == LOW) {
            previousTimer3 = currentTime;
            digitalWrite(solenoidArray[1], HIGH);
            hookNext = 1;
          }
        }
        if ((mpsEnable >= 2) && (currentTime - previousTimer3 < sysArray[7]) && (currentTime - previousTimer3 >= sysArray[6])) {
          //Check if MPS is enabled.  If so, check value of time sensor triggered.
          runCheck = 0;
          previousTimer3 = currentTime;
          machStop(0);
        }
      }
      //Send Head Down
      if ((hookNext == 1) && (currentTime - previousTimer3 >= sysArray[2])) {
        previousTimer3 = currentTime;
        Serial.println("Hook Cycle | Tool/Head OUT");
        digitalWrite(solenoidArray[2], HIGH);
        hookNext = 2;
      }
      //Send StripOff Out
      if (hookNext == 2) {
        int HeadCheckDown = digitalRead(sensorArray[6]);
        //MPS Disabled
        if (((HeadCheckDown == LOW) && (mpsEnable <= 2)) || ((HeadCheckDown == LOW) && (mpsEnable >= 3) && (currentTime - previousTimer3 < sysArray[8]))) {
          digitalWrite(solenoidArray[3], HIGH);
          hookNext = 3;
          Serial.println("Hook Cycle | Strip Off OUT");
        }
        //MPS Setting 5 - Shut down on timer
        if ((mpsEnable >= 5) && (currentTime - previousTimer3 >= sysArray[8])) {
          machStop(1);
          Serial.println("Motor stopped due to ERROR[0036]");
          runCheck = 0;
          hookNext = 0;
        }
        if ((HeadCheckDown == LOW) && (mpsEnable >= 3) && (currentTime - previousTimer3 >= sysArray[8])) {
          mfcount++;
          hookNext = 3;
          if (mpsEnable == 4) {
            machStop(1);
            runCheck = 0;
            hookNext = 0;
            Serial.println(F("Motor stopped due to ERROR[0034]"));
            Serial.print("preTime: ");
            Serial.print(previousTimer3);
            Serial.print(" - ");
            Serial.print("currentTime: ");
            Serial.print(currentTime);
            Serial.print(" > ");
            Serial.print("varTime: ");
            Serial.println(sysArray[8]);
            previousTimer3 = currentTime;
            //Turn off machine
          }
          digitalWrite(solenoidArray[3], HIGH);
          Serial.println("Hook Cycle | Strip Off OUT");
          Serial.println("Malfunction detected");
        }
      }
      //Send Head Up
      if (hookNext == 3) {
        int StripOffCheck = digitalRead(sensorArray[5]);
        if (StripOffCheck == LOW) {
          digitalWrite(solenoidArray[2], LOW);
          digitalWrite(panelLed3, HIGH);
          digitalWrite(panelLed2, LOW);
          hookNext = 4;
        }
      }
      if (hookNext == 4) {
        int HeadUpCheck = digitalRead(sensorArray[7]);
        if (HeadUpCheck == LOW) {
          Serial.println("Hook Cycle | Reset");
          digitalWrite(solenoidArray[1], LOW);
          digitalWrite(solenoidArray[3], LOW);
          digitalWrite(panelLed3, LOW);
          hookNext = 0;
        }
      }// END OF HOOK CYCLE
      /* When logicCount Variable reaches 100,
          Trigger TimeKeeper to run
          TimeKeeper will reset logicCount back to 0.
      */
      if (logicCount >= 100) {
        TimeKeeper();
      }
    }//END OF ACTIVE MODE
    /*Start of INACTIVE MODE
       Inactive Mode:
       - Change time variables
       - Listen for keypad input
          - Go into Override Mode (on correct key input)
    */
    if (active == 0) {
      lcd.setCursor(0, 2);
      lcd.print("Time:");
      inactive(sysPosition);
    } // End of active 0 (containing switch)
  } // End of Override Statement (sOverride = 0 or 1)
  /* Start System Override
     - Trigger Relays individually
     - Record state of relay for display and toggle
  */
  if (sOverride == 2) {
    digitalWrite(panelLed1, HIGH);
    digitalWrite(panelLed2, HIGH);
    digitalWrite(panelLed3, HIGH);
    digitalWrite(panelLed4, HIGH);
    digitalWrite(panelLed5, HIGH);
    lcd.setCursor(0, 1);
    lcd.print("OVERRIDE: ON        ");
    lcd.setCursor(0, 2);
    lcd.print("SYSTEM: Relay ");
    bNextLogic = digitalRead(nextButton);
    if ((bNextLogic == LOW) && (currentTime - buttonPreviousTime >= buttonWait)) {
      buttonPreviousTime = currentTime;
      rswitch++;
      if (rswitch >= 8) {
        rswitch = 0;
      }
    }
    saveButtonLogic = digitalRead(saveButton);
    if ((saveButtonLogic == LOW) && (currentTime - buttonPreviousTime >= buttonWait)) {
      buttonPreviousTime = currentTime;
      Override_Trigger(rswitch + 1);
    }
    bDownLogic = digitalRead(downButton);
    if ((bDownLogic == LOW) && (currentTime - buttonPreviousTime >= buttonWait)) {
      buttonPreviousTime = currentTime;
      lcd.setCursor(0, 3);
      lcd.print("Override Deactivated");
      Serial.println("SYSTEM OVERRIDE | Deactivated ");
      preLCDClear = currentTime;
      sOverride = 0;
      sysPosition = 0;
    }
    else {
      //Get keypad input
      char key;
      key = keypad.getKey();
      if (key) {
        int tempb = key - '0';
        //Send keypad input to Override_Trigger function
        Override_Trigger(tempb);
      }
      //Display current selected relay on LCD (For use of manual buttons)
      lcd.setCursor(14, 2);
      lcd.print(rswitch + 1);
    } // End of Else Statment
  } // End of sOverride2
} //End of LOOP Void


void inactive(int sysPosition) {
  //Trigger intital reset when exiting inactive mode
  sOverride = 0;
  railCheckNext = 0;
  digitalWrite(errorLed, HIGH);
  digitalWrite(solenoidArray[0], LOW); //FeedTable
  digitalWrite(solenoidArray[1], LOW); //HookStopper
  digitalWrite(solenoidArray[2], LOW); //Head/Tooling
  digitalWrite(solenoidArray[3], LOW); //StripOff
  digitalWrite(solenoidArray[4], LOW); //CrimpStopper
  digitalWrite(solenoidArray[5], LOW); //Crimp
  digitalWrite(solenoidArray[6], LOW); //Vibrator
  digitalWrite(solenoidArray[7], LOW); //MainAir
  switch (sysPosition) {
    case 0:
      setLEDS(panelLed1);
      lcd.setCursor(0, 1);
      lcd.print("Feed Wait Time:     ");
      changetime(sysPosition);
      break;
    case 1:
      setLEDS(panelLed2);
      lcd.setCursor(0, 1);
      lcd.print("Feed Open Time      ");
      changetime(sysPosition);
      break;
    case 2:
      setLEDS(panelLed3);
      lcd.setCursor(0, 1);
      lcd.print("Hook Cycle Wait     ");
      changetime(sysPosition);
      break;
    case 3:
      setLEDS(panelLed4);
      lcd.setCursor(0, 1);
      lcd.print("Crimp Cycle Wait    ");
      changetime(sysPosition);
      break;
    case 4:
      setLEDS(panelLed5);
      lcd.setCursor(0, 1);
      lcd.print("Crimp Time          ");
      changetime(sysPosition);
      break;
    case 5:
      setLEDS(panelLed1);
      lcd.setCursor(0, 1);
      lcd.print("Vibrator Time     ");
      changetime(sysPosition);
      break;
    case 6:
      setLEDS(panelLed2);
      lcd.setCursor(0, 1);
      lcd.print("Sensor Ignore [MPS] ");
      changetime(sysPosition);
      break;
    case 7:
      setLEDS(panelLed3);
      lcd.setCursor(0, 1);
      lcd.print("Main Cycle [MPS]   ");
      changetime(sysPosition);
      break;
    case 8:
      setLEDS(panelLed4);
      lcd.setCursor(0, 1);
      lcd.print("Head LOC [MPS]     ");
      changetime(sysPosition);
      break;
  } //END OF MAIN SWITCH
} // End of Inactive void
//Save trigger function.
/* This function is for saving values from manual button changes.
   For Keypad function see: changetime
*/
void savetrigger(int sysPosition) {
  if (sysArray[sysPosition] >= 5101) {
    sysArray[sysPosition] = 5100;
    lcd.setCursor(0, 3);
    lcd.print("Max Value hit!");
    Serial.println("SYSTEM: Max value hit when trying to save.");
  }
  unsigned long address = (((vector * memVectorMultiple) + sysPosition) * 2);
  int ytemp = ytemp = sysArray[sysPosition] / 10;
  if (ytemp > 255) {
    ytemp = ytemp - 255;
    EEPROM.update(address, ytemp);
    address = address + 1;
    if (address == EEPROM.length()) {
      address = 0;
      Serial.println("*** SYSTEM ERROR [EE0005]");
    }
    EEPROM.update(address, 255);
    address = address + 1;
    if (address == EEPROM.length()) {
      address = 0;
      Serial.println("*** SYSTEM ERROR [EE0006]");
    }
  }
  if (ytemp < 255) {
    EEPROM.update(address, ytemp);
    address = address + 1;
    if (address == EEPROM.length()) {
      address = 0;
      Serial.println("*** SYSTEM ERROR [EE0007]");
    }
    EEPROM.update(address, 0);
    address = address + 1;
    if (address == EEPROM.length()) {
      address = 0;
      Serial.println("*** SYSTEM ERROR [EE0008]");
    }
  }
  ytemp = sysPosition + 1;
  lcd.setCursor(0, 3);
  lcd.print("EE.Update VAR[");
  lcd.print(ytemp);
  lcd.print("]    ");
  unsigned long currentTime = millis();
  preLCDClear = currentTime;
  int state = digitalRead(errorLed);
  digitalWrite(errorLed, HIGH);
  delay(200);
  digitalWrite(errorLed, LOW);
  delay(200);
  digitalWrite(errorLed, HIGH);
  delay(200);
  digitalWrite(errorLed, LOW);
  delay(200);
  digitalWrite(errorLed, state);
}


void Override_Trigger(int RTrigger) {
  int tempstate = LOW;
  String lcdstate = "OFF";
  unsigned long currentTime = millis();
  if (stateArray[RTrigger] == 1) {
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
  Serial.print("SYSTEM OVERRIDE | Relay ");
  Serial.print(RTrigger);
  Serial.print(" | ");
  Serial.println(lcdstate);
  preLCDClear = currentTime;
}

//Clear last line of LCD every x(seconds)
void lcdControl() {
  unsigned long currentTime = millis();
  lcd.setCursor(10, 0);
  lcd.print(currentTime / 1000);
  if (currentTime - preLCDClear >= lcdClearTime)
  {
    preLCDClear = currentTime;
    lcd.setCursor(0, 3);
    lcd.print("                    ");
  }
}

//Set one LED to HIGH.
void setLEDS(byte LEDSnumber)
{
  digitalWrite(panelLed1, LOW);
  digitalWrite(panelLed2, LOW);
  digitalWrite(panelLed3, LOW);
  digitalWrite(panelLed4, LOW);
  digitalWrite(panelLed5, LOW);

  digitalWrite(LEDSnumber, HIGH);
}


void machStop(byte airoff) {
  digitalWrite(solenoidArray[8], HIGH);
  for (byte k; k < 7; k++) {
    digitalWrite(solenoidArray[k], LOW);
  }
  if (airoff >= 1) {
    digitalWrite(solenoidArray[7], LOW);
  }
  feedNext = 0;
  hookNext = 0;
  railCheckNext = 0;
  return;
}

void mpsInput() {
  char key;
  lcd.setCursor(0, 1);
  lcd.print("Enter key for MPS");
  lcd.setCursor(0, 2);
  lcd.print("Current: ");
  lcd.print(mpsEnable);
  key = keypad.getKey();
  while (!key)
  {
    key = keypad.getKey();
    lcdControl();
    if (key) {
      if (key == '#') {
        return;
      }
      int keyValue = key - '0';
      Serial.println(keyValue);
      if (keyValue > 5) {
        keyValue = 5;
      }
      mpsEnable = keyValue;
      lcd.setCursor(0, 3);
      lcd.print("MPS set to: ");
      lcd.print(mpsEnable);
      EEPROM.update(mpsMemLoc, mpsEnable);
      Serial.print("SYSTEM: Updated mpsEnable: ");
      Serial.println(mpsEnable);
      preLCDClear = millis();
    }
  }
}

void recvWithEndMarker() {
  static byte ndx = 0;
  char endMarker = '\n';
  char rc;

  while (Serial.available() > 0 && newData == false) {
    rc = Serial.read();

    if (rc != endMarker) {
      receivedChars[ndx] = rc;
      ndx++;
      apple = apple += rc;
      if (ndx >= numChars) {
        ndx = numChars - 1;
      }
    }
    else {
      receivedChars[ndx] = '\0'; // terminate the string
      ndx = 0;
      newData = true;
    }
  }
}
void checkData() {
  if (newData == true) {
    if (apple.length() >= 5) {
      if (apple.substring(0, 6) == "EEPROM") {
        eepromUpdate();
      }
      if (apple.substring(0, 3) == "PIN") {
        pinUpdate();
      }
      if (apple.substring(0, 8) == "OVERRIDE") {
        if ((sOverride == 0) || (sOverride == 1)) {
          sOverride = 2;
        } else {
          sOverride = 0;
        }
      }
      if (apple.substring(0, 8) == "SENCHECK") {
        sensorCheckActivator();
      }
      if (apple.substring(0, 6) == "SITREP") {
        for (byte k; k < solArraySize; k++) {
          Serial.print("SOL.");
          Serial.print(k);
          Serial.print(".");
          Serial.println(solenoidArray[k]);
          delay(220);
        }
        for (byte k; k < senArraySize; k++) {
          Serial.print("SEN.");
          Serial.print(k);
          Serial.print(".");
          Serial.println(sensorArray[k]);
          delay(220);
        }
        for (byte k = 0; k < sysLength; k++) {
          byte address = k * 2;
          byte alpha = EEPROM.read(address);
          address++;
          byte beta = EEPROM.read(address);
          int total = ((alpha + beta) * 10);
          Serial.print("EMU.");
          Serial.print(k);
          Serial.print(".");
          Serial.println(total);
          delay(220);
        }
        sensorCheckActivator();
        Serial.println("SITREP COMPLETE");
      }
    }
    newData = false;
    apple = "";
  }
}


void pinUpdate() {
  boolean value = LOW;
  byte lastPos = 0;
  char pear[] = {0};
  for (byte k = 4; k <= apple.length(); k++) {
    int charIndx = k - 4;
    //Add numbers to array(pear) till it finds a '.'
    if (receivedChars[k] != '.') {
      pear[charIndx] = receivedChars[k];
      delay(1);
    } else {
      //Terminate the array with a null
      pear[charIndx] = '\0';
      delay(10);
      lastPos = k;
      break;
    }
  }
  int pTree = atoi(pear);
  if (pTree >= 64) {
    pTree = 64;
  }
  lastPos++;
  if (receivedChars[lastPos] == '\0') {
    Serial.println("ERROR FOUND");
    return;
  } else if (receivedChars[lastPos] == '0') {
    value = LOW;
  } else if (receivedChars[lastPos] == '1') {
    value = HIGH;
  }
  for (byte pinCheck = 0; pinCheck < 10; pinCheck++) {
    if (pTree == solenoidArray[pinCheck]) {
      stateArray[pinCheck] = value;
    }
  }
  Serial.print("SUPD [");
  Serial.print(pTree);
  Serial.print("] Value: ");
  Serial.println(value);
  digitalWrite(pTree, value);
}
void senCheck() {
  byte indx = 0;
  Serial.print("INL.");
  Serial.print(senArraySize);
  Serial.print(".");
  for (byte k = 0; k < senArraySize; k++) {
    indx++;
    boolean senOutput = digitalRead(sensorArray[k]);
    if (senOutput == HIGH) {
      Serial.print("1");
      orchard[k] = 1;
    } else {
      Serial.print("0");
      orchard[k] = 0;
    }
  }
  orchard[indx] = '\0';
  Serial.println(); //Send the completed serial array
}

void sensorCheckActivator() {
  boolean carrier;
  if (initial == 1) {
    senCheck();
    initial = 0;
  } else {
    for (byte k = 0; k < senArraySize; k++) {
      carrier = digitalRead(sensorArray[k]);
      if (carrier == HIGH) {
        if (orchard[k] != 1) {
          senCheck();
        }
      }
      if (carrier == LOW) {
        if (orchard[k] != 0) {
          senCheck();
        }
      }
    }
  }
}

//Write how long it took to run 100 parts & reset logicCount
void TimeKeeper() {
  unsigned long tempvarj = ((millis() - precountTime) / 1000);
  Serial.print("CTN Run Time: ");
  Serial.println(tempvarj);
  lcd.setCursor(11, 1);
  lcd.print("CTN:");
  lcd.setCursor(15, 1);
  lcd.print(tempvarj);
  logicCount = 0;
}


void changetime(int sysPosition) {
  lcd.setCursor(5, 2);
  lcd.print(sysArray[sysPosition]);
  lcd.print("      ");
  lcd.setCursor(pos, 2);
  char key;
  key = keypad.getKey();
  if (key) {
    if ((key == 'A') || (key == 'a')) {
      mpsInput();
      Serial.println("MPS Activated");
      return;
    }
    if (key == 'B') {
      mfcount = 0;
      Serial.print(F("SYSTEM | Reset Malfunction count"));
      lcd.setCursor(0, 3);
      lcd.print("Reset MalFunc Count");
      preLCDClear = millis();
    }
    if (key == 'C') {
      jindx = 0;
      vectorChange();
    }
    lcd.print(key);
    pos++;
    lcd.setCursor(pos, 2);
    arraya[jindx++] = key;
    arraya[jindx];
    if (pos > 20) {
      pos = 15;
    }
    if (key == '*') {
      int tempa = atoi(arraya);
      Serial.print("SYSTEM | Keypad Input: ");
      Serial.println(tempa);
      if ((tempa == passcode) && (active == 0)) {
        //VERY IMPORTANT!  Check to see if active is 0, so that override isn't turned on while machine running.
        sOverride = 2;
        pos = 15;
        lcd.setCursor(pos, 2);
        lcd.print("       ");
        jindx = 0;
        return;
      }
      if (tempa > 5100) {
        tempa = 5100;
        Serial.println("WARNING: MAX VALUE HIT");
        lcd.setCursor(0, 3);
        lcd.print("ERROR: MAX VALUE HIT");
        preLCDClear = millis();
      }
      if (tempa >= 2550) {
        sysArray[sysPosition] = tempa;
        int ytemp = sysArray[sysPosition] / 10;
        unsigned long address = (((vector * memVectorMultiple) + sysPosition) * 2);
        //address = sysPosition * 2;
        ytemp = ytemp - 255;
        EEPROM.update(address, 255);
        address = address + 1;
        if (address == EEPROM.length()) {
          address = 0;
          Serial.println("*** SYSTEM ERROR [EE0001]");
        }
        EEPROM.update(address, ytemp);
        if (address == EEPROM.length()) {
          address = 0;
          Serial.println("*** SYSTEM ERROR [EE0002]");
        }
        Serial.print("SYSTEM | EEPROM | ");
        Serial.print(ytemp + 255);
        Serial.print(" was wrote to EEPROM address: ");
        Serial.println(sysPosition);
      }
      if (tempa < 2550) {
        sysArray[sysPosition] = tempa;
        int ytemp = sysArray[sysPosition] / 10;
        unsigned long address = (((vector * memVectorMultiple) + sysPosition) * 2);
        Serial.print("DEBUG Address 1: ");
        Serial.println(address);
        EEPROM.update(address, ytemp);
        address = address + 1;
        if (address == EEPROM.length()) {
          address = 0;
          Serial.println("*** SYSTEM ERROR [EE0003]");
        }
        Serial.print("DEBUG Address 2: ");
        Serial.println(address);
        EEPROM.update(address, 0);
        if (address == EEPROM.length()) {
          address = 0;
          Serial.println("*** SYSTEM ERROR [EE0004]");
        }
        Serial.print("EEPROM | ");
        Serial.print(ytemp);
        Serial.print(" was wrote to EEPROM address: ");
        Serial.println(address);
      }
      pos = 15;
      lcd.setCursor(pos, 2);
      lcd.print("      ");
      jindx = 0;
      return;
    }
    if (key == '#') {
      pos = 15;
      lcd.setCursor(pos, 2);
      lcd.print("     ");
      jindx = 0;
      return;
    }
  } //End of If(Key)
}
//End of ChangeTime function


int timeValue(byte memAddress) {
  int memBlockOne = EEPROM.read(memAddress);
  //times the value from the EEPROM * 10 to get desired result for this sketch.
  memBlockOne = memBlockOne * 10;
  //See if the value falls within the limits of the variable.
  if ((memBlockOne > 2550) || (memBlockOne < 0)) {
    Serial.print("ERROR | Corrupted memory LOC:");
    Serial.print(memAddress);
    Serial.print(" Result:");
    Serial.println(memBlockOne);
    lcd.setCursor(0, 0);
    lcd.print("MEMCORE:");
    lcd.print(memAddress);
    return;
  }
  //Increment the address by one to get second value.
  memAddress++;
  int memBlockTwo = EEPROM.read(memAddress);
  //Do the same as memBlockOne
  memBlockTwo = memBlockTwo * 10;
  if ((memBlockOne > 2550) || (memBlockOne < 0)) {
    Serial.print("ERROR | Corrupted memory LOC:");
    Serial.print(memAddress);
    Serial.print(" Result:");
    Serial.println(memBlockOne);
    lcd.setCursor(0, 0);
    lcd.print("MEMCORE:");
    lcd.print(memAddress);
    return;
  }
  //Load the value into the system array for use by the sketch.
  int timeTotal = memBlockTwo + memBlockOne;
  memAddress--;
  //Small delay to keep from overprocessing
  delay(10);
  return timeTotal;
}

void eepromUpdate() {
  byte masterIndex = 7;
  byte slaveIndex = 0;
  char grape[numChars] = {0};
  Serial.println("Processing EEPROM Update...");
  for (byte k = masterIndex; k <= apple.length(); k++) {
    if (receivedChars[k] != '.') {
      grape[slaveIndex] = receivedChars[k];
      slaveIndex++;
    }
    else {
      grape[slaveIndex] = '\0';
      masterIndex = k + 1;
      slaveIndex = 0;
      break;
    }
    delay(10);
  }
  int updateAddress = atoi(grape);
  //Serial.println(updateAddress);
  if (updateAddress >= EEPROM.length()) {
    Serial.println("EEPROM LIMIT HIT");
    updateAddress = 0;
  }
  for (byte k = masterIndex; k <= apple.length(); k++) {
    grape[slaveIndex] = receivedChars[k];
    slaveIndex++;
  }
  grape[slaveIndex] = '\0';
  int eepromValue = atoi(grape);
  Serial.print("EEPROM.update(");
  Serial.print(updateAddress);
  Serial.print(", ");
  Serial.print(eepromValue);
  Serial.println(")");
  EEPROM.update(updateAddress, eepromValue);
  for (byte k; k < sysLength; k++) {
    sysArray[k] = timeValue(k * 2);
  }
}

void EEPROM_Read() {
  //Load EEPROM Memory
  int memAddress = ((vector * memVectorMultiple) * 2);
  for (byte k = 0; k < sysLength; k++) {
    int memBlockOne = EEPROM.read(memAddress);
    Serial.print("X23 Address[ ");
    Serial.print(memAddress);
    Serial.print(" ] Value: ");
    Serial.println(memBlockOne);
    memBlockOne = memBlockOne * 10;
    if ((memBlockOne > 2550) || (memBlockOne < 0)) {
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
    if ((memBlockTwo > 2550) || (memBlockTwo < 0)) {
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
    memAddress ++;//Increase for next address read
    Serial.print("EEPROM[");
    Serial.print(k);
    Serial.print("]: ");
    Serial.println(sysArray[k]);
    delay(10);
  }
}

void vectorChange() {
  lcd.setCursor(0,1);
  lcd.print("Memory Vector:      ");
  lcd.setCursor(pos, 2);
  boolean complete = false;
  char keyArrayB[] = {0};
  while (complete == false) {
    char key;
    key = keypad.getKey();
    if (key) {
      pos++;
      lcd.setCursor(pos, 2);
      keyArrayB[jindx++] = key;
      keyArrayB[jindx];
      if (pos > 20) {
        pos = 15;
      }

      if (key == '*') {
        int tempa = atoi(keyArrayB);
        tempa = tempa / 2;
        switch (tempa) {
          case 206:
            lcd.setCursor(0,3);
            lcd.print("Loaded 412 timing");
            Serial.println("Loaded 412 settings");
            EEPROM.update(100,0);
            vector = 0;
            Serial.println("Vector 0");
            EEPROM_Read();
            lcd.setCursor(17,0);
            lcd.print("412");
            break;
          case 207:
            lcd.setCursor(0,3);
            lcd.print("Loaded 414 timing");
            Serial.println("Loaded 414 settings");
            EEPROM.update(100,1);
            vector = 1;
            EEPROM_Read();
            Serial.println("Vector 1");
            lcd.setCursor(17,0);
            lcd.print("414");
            break;
          case 250:
            lcd.setCursor(0,3);
            lcd.print("Loaded 500 timing");
            Serial.println("Loaded 500 settings");
            EEPROM.update(100,2);
            vector = 2;
            Serial.println("Vector 2");
            EEPROM_Read();
            lcd.setCursor(17,0);
            lcd.print("500");
            break;
          default:
            lcd.setCursor(0,3);
            lcd.print("INVALID INPUT");
            Serial.println("Recieved invalid input");
            break;
        }
        pos = 15;
        jindx = 0;
        pos = 15;
        lcd.setCursor(pos, 2);
        lcd.print("      ");
        complete = true;
        preLCDClear = millis();
      }
      if (key == '#') {
        pos = 15;
        lcd.setCursor(pos, 2);
        lcd.print("      ");
        jindx = 0;
      }
    }
  }
}
