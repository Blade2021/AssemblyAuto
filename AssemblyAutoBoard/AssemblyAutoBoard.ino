
/*  VERSION 1.2.9
    Last succcessful run: 1.2.8
    Last Upload SHA Token: 8de98c
*/
#include <Keypad.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>

//Panel Buttons
const byte StartFeedButton = 6;
const byte NextButton = 42;
const byte SaveButton = 46;
const byte UpButton = 48;
const byte DownButton = 44;
const byte ToggleButton = 50;
// Panel LEDs
const byte PanelLed1 = 51;
const byte PanelLed2 = 49;
const byte PanelLed3 = 47;
const byte PanelLed4 = 45;
const byte PanelLed5 = 43;
const byte ErrorLed = 13;
//Sensors
const byte SensorArray[8] = {A0, A1, A2, A3, A4, A5, A6, A7};
/* SENSOR LIST
 * A0 - HookRailEmpty
 * A1 - HangerRackFull
 * A2 - HookCycleStart
 * A3 - SolenoidArray[4]CycleStart
 * A4 - HookRailRull
 * A5 - SolenoidArray[1]Out
 * A6 - HeadDown
 * A7 - HeadUp
 */
//Solenoids
const byte SolenoidArray[8] = {16, 17, 8, 18, 19, 7, 14, 15};
/* 
 * 16 - ToolHead
 * 17 - StripOff
 * 8  - HookStopper
 * 18 - CrimpStopper
 * 19 - Crimp
 * 7  - FeedTable
 * 14 - MainAir
 * 15 - HookShaker
 */
//LCD Variables
byte sysPosition = 0;
const int LCDClearTime = 7000;
byte pos = 15;
int j = 0;
char arraya [] = {0, 1, 2, 3, 0};

//Time Controls
const int buttonWait = 300;
unsigned long preLCDClear = 0;
unsigned long buttonPreviousTime = 0;
unsigned long previousTimer1 = 0;
unsigned long previousTimer2 = 0;
unsigned long previousTimer3 = 0;
unsigned long previousTimer4 = 0;
unsigned long precountTime = 0;
long sysArray[] = {1000, 1000, 1000, 1000, 2300, 2000, 3000};

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
boolean Active = LOW;
byte ToggleLogic = 0;
byte FeedLoop = 0;
byte FeedCheck = 0;
byte FeedNext = 0;
byte HookNext = 0;
byte HookLoop = 0;
byte HookCheck = 0;
byte CrimpLoop = 0;
byte CrimpNext = 0;
byte RailCheck = LOW;
byte RailCheckNext = 0;
byte rswitch = 0;
byte SOverride = 1;
char StateArray[8] = {0}; //Include extra 0 for the NULL END
int passcode = 7777;
byte Error = 0;

//LOGIC CONTROLS
byte LogicCount = 0; //Counter of material flow
byte BNextLogic = 0; //Button Next Logic
byte BUpLogic = 0;
byte BDownLogic = 0;
byte SaveButtonTrigger = 0;
byte ManualFeed = 0;
byte SecStart = 0;

void setup() {
  //LEDs
  pinMode(PanelLed1, OUTPUT);
  pinMode(PanelLed2, OUTPUT);
  pinMode(PanelLed3, OUTPUT);
  pinMode(PanelLed4, OUTPUT);
  pinMode(PanelLed5, OUTPUT);
  pinMode(ErrorLed, OUTPUT);
  //Buttons
  pinMode(StartFeedButton, INPUT);
  pinMode(NextButton, INPUT);
  pinMode(SaveButton, INPUT);
  pinMode(UpButton, INPUT);
  pinMode(DownButton, INPUT);
  pinMode(ToggleButton, INPUT);
  //Solenoids
  pinMode(SolenoidArray[0], OUTPUT);
  pinMode(SolenoidArray[2], OUTPUT);
  pinMode(SolenoidArray[3], OUTPUT);
  pinMode(SolenoidArray[4], OUTPUT);
  pinMode(SolenoidArray[5], OUTPUT);
  pinMode(SolenoidArray[6], OUTPUT);
  pinMode(SolenoidArray[7], OUTPUT);
  pinMode(SolenoidArray[1], OUTPUT);
  //Photo
  pinMode(SensorArray[2], INPUT_PULLUP);
  pinMode(SensorArray[1], INPUT_PULLUP);
  pinMode(SensorArray[3], INPUT_PULLUP);
  //Prox
  pinMode(SensorArray[0], INPUT_PULLUP);
  pinMode(SensorArray[4], INPUT_PULLUP);
  pinMode(SensorArray[7], INPUT_PULLUP);
  pinMode(SensorArray[6], INPUT_PULLUP);
  pinMode(SensorArray[5], INPUT_PULLUP);
  // END OF PINMODE
  
  Serial.begin(9600);
  Serial.println("Starting...");
  Serial.println("Program Version 1.2.9");
  lcd.begin(20, 4);
  lcd.setCursor(0, 0);
  lcd.print("Run Time: ");
  lcd.setCursor(2, 1);
  lcd.print("*** BOOTING ***");
  
  //Load EEPROM Memory
  for (byte k = 0; k < 6; k++) {
    byte f = k * 2;
    int ytemp = EEPROM.read(f);
    ytemp = ytemp * 10;
    f++;
    int gtemp = EEPROM.read(f);
    gtemp = gtemp * 10;
    sysArray[k] = gtemp + ytemp;
    Serial.print("EEPROM[");
    Serial.print(k);
    Serial.print("]: ");
    Serial.println(sysArray[k]);
    delay(10);
  }
  Serial.println(F("*** System Variables ***"));
  Serial.print("Button Wait Time: ");
  Serial.println(buttonWait);
  Serial.print("LCD Clear Time: ");
  Serial.println(LCDClearTime);
  Serial.print("LCD Default POS: ");
  Serial.println(pos);
  Serial.print("Override Passcode: ");
  Serial.println(passcode);
  Serial.println();
  lcd.setCursor(0,1);
  lcd.print("                    ");
}

void loop() {
  //Main Timer to keep track of entire machine!
  unsigned long currentTime = millis(); 
  //Call LCD Clear function to clear 4th line of LCD
  lcdClear();
  lcd.setCursor(10, 0);
  //Print run time on first line of LCD
  lcd.print(millis() / 1000);
  // Check SOverride  If 0 or 1, It is considered "off"
  if (SOverride == 0 || SOverride == 1) {
    //Run initial reset of all LED's and reset Relay status
    if (SOverride == 0 && Active != 0) { 
      for (byte indx = 0; indx < 8; indx++) {
        StateArray[indx] = 0;
        Serial.print("Relay status INDEX: ");
        Serial.print(indx);
        Serial.println(" reset.");
      }
      //Add null to end of StateArray.  StateArray is used to keep track of the state of the relays.
      //StateArray[7] = '\0'; 
      lcd.setCursor(0,1);
      lcd.print("                    ");
      digitalWrite(PanelLed1, LOW);
      digitalWrite(PanelLed2, LOW);
      digitalWrite(PanelLed3, LOW);
      digitalWrite(PanelLed4, LOW);
      digitalWrite(PanelLed5, LOW);
      //Reset the count after leaving SOverride or inactive mode
      LogicCount = 0;
      SOverride = 1; //Exit initial reset
    }
    //Listen for Next Button (Goes through different values inside sysArray)
    BNextLogic = digitalRead(NextButton);
    if ((BNextLogic == HIGH) && (currentTime - buttonPreviousTime >= buttonWait)) {
      buttonPreviousTime = currentTime;
      sysPosition++;
      //Reset back to value 0 if you get to the end of the array. (minus one for the override)
      if (sysPosition >= 6) {
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
    BUpLogic = digitalRead(UpButton);
    if ((BUpLogic == HIGH) && (currentTime - buttonPreviousTime >= buttonWait)) {
      sysArray[sysPosition] = sysArray[sysPosition] + 100;
      buttonPreviousTime = currentTime;
      Serial.print("TimeVar ");
      Serial.print(sysPosition + 1);
      Serial.print(" is now: ");
      Serial.println(sysArray[sysPosition]);
    }
    //Lower the value of the selected variable inside the sysArray (Reduce Time)
    BDownLogic = digitalRead(DownButton);
    if ((BDownLogic == HIGH) && (currentTime - buttonPreviousTime >= buttonWait)) {
      sysArray[sysPosition] = sysArray[sysPosition] - 100;
      buttonPreviousTime = currentTime;
      Serial.print("TimeVar ");
      Serial.print(sysPosition + 1);
      Serial.print(" is now: ");
      Serial.println(sysArray[sysPosition]);
    }
    //Save the new value to the memory for next reset.
    SaveButtonTrigger = digitalRead(SaveButton);
    if ((SaveButtonTrigger == HIGH) && (currentTime - buttonPreviousTime >= buttonWait)) {
      buttonPreviousTime = currentTime;
      if (SOverride == 0 || SOverride == 1) {
        Serial.print("TimeVar ");
        Serial.print(sysPosition + 1);
        Serial.println(" saved.");
        savetrigger(sysPosition);  //go to savetrigger function to save.
      }
    }
    /* Trigger Active mode on/off
     *  Active Mode: Sets wether the machine should read the sensors or ignore them.
     */
    ToggleLogic = digitalRead(ToggleButton);
    if ((ToggleLogic == HIGH) && (Active == 0)) {
      Active = 1;
      digitalWrite(PanelLed1, LOW);
      digitalWrite(PanelLed2, LOW);
      digitalWrite(PanelLed3, LOW);
      digitalWrite(PanelLed4, LOW);
      digitalWrite(PanelLed5, LOW);
      digitalWrite(ErrorLed, LOW);
    }
    if (ToggleLogic == LOW) {
      Active = 0;
      digitalWrite(ErrorLed, HIGH);
    }
// Active Mode start.  Machine will read sensors and run relays.
    if (Active == 1) {
      digitalWrite(SolenoidArray[6], HIGH);
      ManualFeed = digitalRead(StartFeedButton);
      FeedLoop = digitalRead(SensorArray[2]);
      FeedCheck = digitalRead(SensorArray[1]);
      /* FeedLoop - Check your main cycle sensor
       * Error - Check to see if the machine went into Error Mode
       * SecStart - Restart machine after FeedCheck activates again
       * FeedCheck - Check Feed station for material.
       * ManualFeed - Ignore other variables and trigger on button press
       */
      if (((FeedLoop == LOW) && (Error == 0)) || ((SecStart == 1) && (FeedCheck == LOW)) || (ManualFeed == HIGH)) {
        if (FeedNext == 0) {
          // FEED ACTIVATED
          Serial.println("Feed Cycle Activated");
          lcd.setCursor(0,2);
          lcd.print("Feed Reset:");
          //Start counting time for TimeKeepr function
          if (LogicCount == 0) {
            precountTime = currentTime;
          }
          //Check Feed station for material.
          FeedCheck = digitalRead(SensorArray[1]);
          if ((FeedCheck == HIGH) && (SecStart != 1)) {
            Serial.println("ERROR: Hanger Rack NOT full.");
            lcd.setCursor(0, 3);
            lcd.print("ERROR: Hanger Rack");
            preLCDClear = currentTime;
            Error = 1;
            SecStart = 1;
            lcd.setCursor(11,2);
            lcd.print("ON ");
          }
          else {
            //Add one to logic count
            LogicCount++;
            SecStart = 0;
            lcd.setCursor(11,2);
            lcd.print("OFF");
            Error = 0;
            lcd.setCursor(0, 1);
            lcd.print("SC: ");
            lcd.setCursor(4,1);
            lcd.print(LogicCount);
            lcd.print("  ");
            digitalWrite(PanelLed1, HIGH);
            digitalWrite(ErrorLed, LOW);
            FeedNext = 1;
            previousTimer1 = currentTime;
          }
        }
      }
      // FEED OPEN
      if ((FeedNext == 1) && (currentTime - previousTimer1 >= sysArray[0])) {
        previousTimer1 = currentTime;
        digitalWrite(SolenoidArray[5], HIGH);
        Serial.println("Feed Cycle | FEED OPEN");
        FeedNext = 2;
      }
      //FEED CLOSE
      if ((FeedNext == 2) && (currentTime - previousTimer1 >= sysArray[1])) {
        Serial.println("Feed Cycle | FEED CLOSE");
        previousTimer1 = currentTime;
        digitalWrite(SolenoidArray[5], LOW);
        digitalWrite(PanelLed1, LOW);
        FeedNext = 0;
      }
      // END OF FEED CYCLE
      // Vibrator Cycle
      RailCheck = digitalRead(SensorArray[4]);
      if ((RailCheck == HIGH) && (RailCheckNext == 0)) {
        Serial.println("Rail Check Activated");
        previousTimer2 = currentTime;
        digitalWrite(PanelLed5, HIGH);
        digitalWrite(SolenoidArray[7], HIGH);
        RailCheckNext = 1;
      }
      if (RailCheckNext == 1) {
        RailCheck = digitalRead(SensorArray[4]);
        if (RailCheck == LOW) {
          previousTimer2 = currentTime;
          RailCheckNext = 2;
        }
      }
      if (RailCheckNext == 2) {
        RailCheck = digitalRead(SensorArray[4]);
        if (RailCheck == HIGH) {
          RailCheckNext = 1;
        }
        if ((RailCheck == LOW) && (currentTime - previousTimer2 >= sysArray[5])) {
          digitalWrite(SolenoidArray[7], LOW);
          digitalWrite(PanelLed5, LOW);
          previousTimer2 = currentTime;
          Serial.println("Rail Check Finsihed");
          RailCheckNext = 0;
        }
      }
      // END OF VIBRATOR CYCLE
      // Crimp Cycle
      CrimpLoop = digitalRead(SensorArray[3]);
      if ((CrimpLoop == LOW) && (CrimpNext == 0)) {
        Serial.println("SolenoidArray[4] Cycle Activated");
        digitalWrite(PanelLed4, HIGH);
        digitalWrite(SolenoidArray[3], HIGH);
        previousTimer4 = currentTime;
        CrimpNext = 1;
      }
      if ((CrimpNext == 1) && (currentTime - previousTimer4 >= sysArray[3])) {
        previousTimer4 = currentTime;
        digitalWrite(SolenoidArray[4], HIGH);
        Serial.println("SolenoidArray[4] Cycle | SolenoidArray[4]");
        CrimpNext = 2;
      }
      if ((CrimpNext == 2) && (currentTime - previousTimer4 >= sysArray[4])) {
        Serial.println("SolenoidArray[4] Cycle | Reset");
        previousTimer4 = currentTime;
        digitalWrite(SolenoidArray[4], LOW);
        digitalWrite(SolenoidArray[3], LOW);
        digitalWrite(PanelLed4, LOW);
        CrimpNext = 0;
      }
      // Hook Cycle
      HookLoop = digitalRead(SensorArray[2]);
      if ((HookLoop == LOW) && (HookNext == 0)) {
        Serial.println("Hook Cycle Activated");
        digitalWrite(PanelLed2, HIGH);
        boolean HookCheck;
        HookCheck = digitalRead(SensorArray[0]);
        if (HookCheck == HIGH) {
          Serial.println("ERROR: Hook Check failed");
          lcd.setCursor(0, 3);
          lcd.print("ERROR: Hook Check");
          preLCDClear = currentTime;
          //Error = 1;
          digitalWrite(PanelLed2, LOW);
          FeedLoop = 0;
          FeedNext = 0;
          digitalWrite(ErrorLed, HIGH);
        }
        if (HookCheck == LOW) {
          previousTimer3 = currentTime;
          digitalWrite(SolenoidArray[2], HIGH);
          HookNext = 1;
        }
      }
      if ((HookNext == 1) && (currentTime - previousTimer3 >= sysArray[2])) {
        previousTimer3 = currentTime;
        Serial.println("Hook Cycle | Tool/Head OUT");
        digitalWrite(SolenoidArray[0], HIGH);
        HookNext = 2;
      }
      if (HookNext == 2) {
        int HeadCheckDown = digitalRead(SensorArray[6]);
        if (HeadCheckDown == LOW) {
          digitalWrite(SolenoidArray[1], HIGH);
          HookNext = 3;
          Serial.println("Hook Cycle | Strip Off OUT");
        }
      }
      if (HookNext == 3) {
        int StripOffCheck = digitalRead(SensorArray[5]);
        if (StripOffCheck == LOW) {
          digitalWrite(SolenoidArray[0], LOW);
          digitalWrite(PanelLed3, HIGH);
          digitalWrite(PanelLed2, LOW);
          HookNext = 4;
        }
      }
      if (HookNext == 4) {
        int HeadUpCheck = digitalRead(SensorArray[7]);
        if (HeadUpCheck == LOW) {
          Serial.println("Hook Cycle | Reset");
          digitalWrite(SolenoidArray[1], LOW);
          digitalWrite(SolenoidArray[2], LOW);
          digitalWrite(PanelLed3, LOW);
          HookNext = 0;
        }
      }// END OF HOOK CYCLE
      /* When LogicCount Variable reaches 100,
       *  Trigger TimeKeeper to run
       *  TimeKeeper will reset LogicCount back to 0.
       */
      if (LogicCount >= 100) {
        TimeKeeper();
      }
    }//END OF ACTIVE MODE
    /*Start of INACTIVE MODE
     * Inactive Mode:
     * - Change time variables
     * - Listen for keypad input
     * - Go into Override Mode (on key input)
     */
    if (Active == 0) {
      lcd.setCursor(0, 2);
      lcd.print("Time:");
      inactive(sysPosition);
    } // End of Active 0 (containing switch)
  } // End of Override Statement (SOverride = 0 or 1)
  /* Start System Override
   * - Trigger Relays individually
   * - Record state of relay for display and toggle
   */
  if (SOverride == 2) {
    digitalWrite(PanelLed1, HIGH);
    digitalWrite(PanelLed2, HIGH);
    digitalWrite(PanelLed3, HIGH);
    digitalWrite(PanelLed4, HIGH);
    digitalWrite(PanelLed5, HIGH);
    lcd.setCursor(0, 1);
    lcd.print("OVERRIDE: ON        ");
    BNextLogic = digitalRead(NextButton);
    if ((BNextLogic == HIGH) && (currentTime - buttonPreviousTime >= buttonWait)) {
      buttonPreviousTime = currentTime;
      rswitch++;
      if (rswitch >= 8) {
        rswitch = 0;
      }
    }
    SaveButtonTrigger = digitalRead(SaveButton);
    if ((SaveButtonTrigger == HIGH) && (currentTime - buttonPreviousTime >= buttonWait)) {
      buttonPreviousTime = currentTime;
      Override_Trigger(rswitch + 1);
    }
    BDownLogic = digitalRead(DownButton);
    if ((BDownLogic == HIGH) && (currentTime - buttonPreviousTime >= buttonWait)) {
      buttonPreviousTime = currentTime;
      lcd.setCursor(0, 3);
      lcd.print("Override Deactivated");
      Serial.println("SYSTEM OVERRIDE | Deactivated ");
      preLCDClear = currentTime;
      SOverride = 0;
      sysPosition = 0;
    }
    else {
      //Get keypad input
      char key;
      key = keypad.getKey();
      if (key) {
        char bxyz[] = {0};
        bxyz[j++] = key;
        bxyz[j];
        int tempb = atoi(bxyz);
        //Send keypad input to Override_Trigger function
        Override_Trigger(tempb);
        j = 0;
      }
      //Display current selected relay on LCD (For use of manual buttons)
      switch (rswitch) {
        case 0:
          lcd.setCursor(0, 2);
          lcd.print("SYSTEM: Relay 1    ");
          break;
        case 1:
          lcd.setCursor(0, 2);
          lcd.print("SYSTEM: Relay 2    ");
          break;
        case 2:
          lcd.setCursor(0, 2);
          lcd.print("SYSTEM: Relay 3    ");
          break;
        case 3:
          lcd.setCursor(0, 2);
          lcd.print("SYSTEM: Relay 4    ");
          break;
        case 4:
          lcd.setCursor(0, 2);
          lcd.print("SYSTEM: Relay 5    ");
          break;
        case 5:
          lcd.setCursor(0, 2);
          lcd.print("SYSTEM: Relay 6    ");
          break;
        case 6:
          lcd.setCursor(0, 2);
          lcd.print("SYSTEM: Relay 7    ");
          break;
        case 7:
          lcd.setCursor(0, 2);
          lcd.print("SYSTEM: Relay 8    ");
          break;
        case 8:
          break;
      } // End of Rswitch
    } // End of Else Statment
  } // End of SOverride2
} // *****************************  End of LOOP Void ************************


void inactive(int sysPosition) {
  //Trigger intital reset when exiting inactive mode
  SOverride = 0;
  digitalWrite(ErrorLed, HIGH);
  digitalWrite(SolenoidArray[5], LOW);
  digitalWrite(SolenoidArray[0], LOW);
  digitalWrite(SolenoidArray[2], LOW);
  digitalWrite(SolenoidArray[3], LOW);
  digitalWrite(SolenoidArray[4], LOW);
  digitalWrite(SolenoidArray[6], LOW);
  digitalWrite(SolenoidArray[7], LOW);
  //If passcode was inserted from keypad, trigger override mode.
  if (sysArray[6] == passcode) {
    Serial.println("***** Override ACTIVATED *****");
    sysArray[6] = 0;
    SOverride = 2;
    return;
  }
  switch (sysPosition) {
    case 0:
      setLEDS(PanelLed1);
      lcd.setCursor(0, 1);
      lcd.print("Feed Wait Time:     ");
      changetime(sysPosition);
      break;
    case 1:
      setLEDS(PanelLed2);
      lcd.setCursor(0, 1);
      lcd.print("Feed Open Time      ");
      changetime(sysPosition);
      break;
    case 2:
      setLEDS(PanelLed3);
      lcd.setCursor(0, 1);
      lcd.print("Hook Cycle Wait     ");
      changetime(sysPosition);
      break;
    case 3:
      setLEDS(PanelLed4);
      lcd.setCursor(0, 1);
      lcd.print("SolenoidArray[4] Cycle Wait    ");
      changetime(sysPosition);
      break;
    case 4:
      setLEDS(PanelLed5);
      lcd.setCursor(0, 1);
      lcd.print("SolenoidArray[4] Time          ");
      changetime(sysPosition);
      break;
    case 5:
      setLEDS(PanelLed1);
      lcd.setCursor(0, 1);
      lcd.print("Vibrator Time     ");
      changetime(sysPosition);
      break;
  } //END OF MAIN SWITCH
} // End of Inactive void

//Save trigger function.
/* This function is for saving values from manual button changes.
 * For Keypad function see: changetime
 */
void savetrigger(int sysPosition) {
  if (sysArray[sysPosition] >= 5101){
    sysArray[sysPosition] = 5100;
    lcd.setCursor(0,3);
    lcd.print("Max Value hit!");
    Serial.println("SYSTEM: Max value hit when trying to save.");
  }
  unsigned long address = sysPosition * 2;
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
  int state = digitalRead(ErrorLed);
  digitalWrite(ErrorLed, HIGH);
  delay(200);
  digitalWrite(ErrorLed, LOW);
  delay(200);
  digitalWrite(ErrorLed, HIGH);
  delay(200);
  digitalWrite(ErrorLed, LOW);
  delay(200);
  digitalWrite(ErrorLed, state);
}


void Override_Trigger(int RTrigger) {
  int tempstate = LOW;
  String lcdstate = "OFF";
  unsigned long currentTime = millis();
  if (StateArray[RTrigger] == 1) {
    tempstate = LOW;
    lcdstate = "OFF";
    StateArray[RTrigger] = 0;
  }
  else
  {
    StateArray[RTrigger] = 1;
    lcdstate = "ON";
    tempstate = HIGH;
  }
  if (RTrigger == 1) {
    digitalWrite(SolenoidArray[5], tempstate);
    lcd.setCursor(0, 3);
    lcd.print("Relay 1 SET TO: ");
    lcd.print(lcdstate);
    lcd.print(" ");
    Serial.print("SYSTEM OVERRIDE | Relay 1 | ");
    Serial.println(lcdstate);
    preLCDClear = currentTime;
  }
  if (RTrigger == 2) {
    digitalWrite(SolenoidArray[2], tempstate);
    lcd.setCursor(0, 3);
    lcd.print("Relay 2 SET TO: ");
    lcd.print(lcdstate);
    lcd.print(" ");
    Serial.print("SYSTEM OVERRIDE | Relay 2 | ");
    Serial.println(lcdstate);
    preLCDClear = currentTime;
  }
  if (RTrigger == 3) {
    digitalWrite(SolenoidArray[0], tempstate);
    lcd.setCursor(0, 3);
    lcd.print("Relay 3 SET TO: ");
    lcd.print(lcdstate);
    lcd.print(" ");
    Serial.print("SYSTEM OVERRIDE | Relay 3 | ");
    Serial.println(lcdstate);
    preLCDClear = currentTime;
  }
  if (RTrigger == 4) {
    digitalWrite(SolenoidArray[1], tempstate);
    lcd.setCursor(0, 3);
    lcd.print("Relay 4 SET TO: ");
    lcd.print(lcdstate);
    lcd.print(" ");
    Serial.print("SYSTEM OVERRIDE | Relay 4 | ");
    Serial.println(lcdstate);
    preLCDClear = currentTime;
  }
  if (RTrigger == 5) {
    digitalWrite(SolenoidArray[3], tempstate);
    lcd.setCursor(0, 3);
    lcd.print("Relay 5 SET TO: ");
    lcd.print(lcdstate);
    lcd.print(" ");
    Serial.print("SYSTEM OVERRIDE | Relay 5 | ");
    Serial.println(lcdstate);
    preLCDClear = currentTime;
  }
  if (RTrigger == 6) {
    digitalWrite(SolenoidArray[4], tempstate);
    lcd.setCursor(0, 3);
    lcd.print("Relay 6 SET TO: ");
    lcd.print(lcdstate);
    lcd.print(" ");
    Serial.print("SYSTEM OVERRIDE | Relay 6 | ");
    Serial.println(lcdstate);
    preLCDClear = currentTime;
  }
  if (RTrigger == 7) {
    digitalWrite(SolenoidArray[7], tempstate);
    lcd.setCursor(0, 3);
    lcd.print("Relay 7 SET TO: ");
    lcd.print(lcdstate);
    lcd.print(" ");
    Serial.print("SYSTEM OVERRIDE | Relay 7 | ");
    Serial.println(lcdstate);
    preLCDClear = currentTime;
  }
  if (RTrigger == 8) {
    digitalWrite(SolenoidArray[6], tempstate);
    lcd.setCursor(0, 3);
    lcd.print("Relay 8 SET TO: ");
    lcd.print(lcdstate);
    lcd.print(" ");
    Serial.print("SYSTEM OVERRIDE | Relay 8 | ");
    Serial.println(lcdstate);
    preLCDClear = currentTime;
  }
}


void changetime(int sysPosition) {
  unsigned long currentTime = millis();
  lcd.setCursor(5, 2);
  lcd.print(sysArray[sysPosition]);
  lcd.print("       ");
  lcd.setCursor(pos, 2);
  char key;
  key = keypad.getKey();
  if (key) {
    lcd.print(key);
    pos++;
    lcd.setCursor(pos, 2);
    arraya[j++] = key;
    arraya[j];
    if (pos > 20) {
      pos = 15;
    }
    if (key == '*') {
      int tempa = atoi(arraya);
      Serial.print("SYSTEM | Keypad Input: ");
      Serial.println(tempa);
      if (tempa == passcode) {
        sysArray[6] = passcode;
        pos = 15;
        lcd.setCursor(pos, 2);
        lcd.print("       ");
        j = 0;
        return;
      }
      if (tempa > 5100) {
        tempa = 5100;
        Serial.println("WARNING: MAX VALUE HIT");
        lcd.setCursor(0, 3);
        lcd.print("ERROR: MAX VALUE HIT");
        preLCDClear = currentTime;
      }
      if (tempa >= 2550) {
        sysArray[sysPosition] = tempa;
        int ytemp = 0;
        unsigned long address = 0;
        ytemp = sysArray[sysPosition] / 10;
        address = sysPosition * 2;
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
        int ytemp = 0;
        unsigned long address = 0;
        ytemp = sysArray[sysPosition] / 10;
        address = sysPosition * 2;
        EEPROM.update(address, ytemp);
        address = address + 1;
        if (address == EEPROM.length()) {
          address = 0;
          Serial.println("*** SYSTEM ERROR [EE0003]");
        }
        EEPROM.update(address, 0);
        if (address == EEPROM.length()) {
          address = 0;
          Serial.println("*** SYSTEM ERROR [EE0004]");
        }
        Serial.print("EEPROM | ");
        Serial.print(ytemp);
        Serial.print(" was wrote to EEPROM address: ");
        Serial.println(sysPosition);
      }
      pos = 15;
      lcd.setCursor(pos, 2);
      lcd.print("      ");
      j = 0;
      return;
    }
    if (key == '#') {
      pos = 15;
      lcd.setCursor(pos, 2);
      lcd.print("      ");
      j = 0;
      return;
    }
  } //End of If(Key)
}
//End of ChangeTime function

//Clear last line of LCD every x(seconds)
void lcdClear() {
  unsigned long currentTime = millis();
  if (currentTime - preLCDClear >= LCDClearTime)
  {
    preLCDClear = currentTime;
    lcd.setCursor(0, 3);
    lcd.print("                    ");
  }
}

//Set one LED to HIGH.
void setLEDS(byte LEDSnumber)
{
  digitalWrite(PanelLed1, LOW);
  digitalWrite(PanelLed2, LOW);
  digitalWrite(PanelLed3, LOW);
  digitalWrite(PanelLed4, LOW);
  digitalWrite(PanelLed5, LOW);

  digitalWrite(LEDSnumber, HIGH);
}

//Write how long it took to run 100 parts & reset LogicCount
void TimeKeeper() {
  unsigned long tempvarj = ((millis() - precountTime) / 1000);
  Serial.print("CTN Run Time: ");
  Serial.println(tempvarj);
  lcd.setCursor(11,1);
  lcd.print("CTN:");
  lcd.setCursor(15, 1);
  lcd.print(tempvarj);
  LogicCount = 0;
}
