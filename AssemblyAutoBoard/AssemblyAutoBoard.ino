/*  VERSION 1.2.7
    COMPILED SUCCESSFULLY ON 03.22.17
*/
#include <Keypad.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>

//Panel Buttons
const int StartFeedButton = 6;
const int NextButton = 42;
const int SaveButton = 46;
const int UpButton = 48;
const int DownButton = 44;
const int ToggleButton = 50;
// Panel LEDs
const int PanelLed1 = 51;
const int PanelLed2 = 49;
const int PanelLed3 = 47;
const int PanelLed4 = 45;
const int PanelLed5 = 43;
const int ErrorLed = 13;
//Sensors
const int HookRailEmpty = A0;
const int HookRailFull = A4;
const int HangerRackFull = A1;
const int HookCycleStart = A2;
const int HeadUp = A7;
const int HeadDown = A6;
const int StripOffOut = A5;
const int CrimpCycleStart = A3;
//Solenoids
const int ToolHead = 16;
const int StripOff = 17;
const int HookStopper = 8;
const int CrimpStopper = 18;
const int Crimp = 19;
const int FeedTable = 7;
const int MainAir = 14;
const int HookShaker = 15;
//LCD Variables
int x = 0;
const int LCDClearTime = 7000;
int pos = 15;
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
unsigned long y[] = {1000, 1000, 1000, 1000, 2300, 2000, 3000};
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
int ToggleLogic = 0;
int FeedLoop = 0;
int FeedCheck = 0;
int FeedNext = 0;
int HookNext = 0;
int HookLoop = 0;
int HookCheck = 0;
int CrimpLoop = 0;
int CrimpNext = 0;
int RailCheck = LOW;
int RailCheckNext = 0;
int rswitch = 0;
int SOverride = 1;
char StateArray[] = {0, 0, 0, 0, 0, 0}; //Include extra 0 for the NULL END
int passcode = 7777;
int Error = 0;
//LOGIC CONTROLS
int LogicCount = 0;
int BNextLogic = 0;
int BUpLogic = 0;
int BDownLogic = 0;
int SaveButtonTrigger = 0;
int ManualFeed = 0;
int SecStart = 0;

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
  pinMode(ToolHead, OUTPUT);
  pinMode(HookStopper, OUTPUT);
  pinMode(CrimpStopper, OUTPUT);
  pinMode(Crimp, OUTPUT);
  pinMode(FeedTable, OUTPUT);
  pinMode(MainAir, OUTPUT);
  pinMode(HookShaker, OUTPUT);
  pinMode(StripOff, OUTPUT);
  //Photo
  pinMode(HookCycleStart, INPUT_PULLUP);
  pinMode(HangerRackFull, INPUT_PULLUP);
  pinMode(CrimpCycleStart, INPUT_PULLUP);
  //Prox
  pinMode(HookRailEmpty, INPUT_PULLUP);
  pinMode(HookRailFull, INPUT_PULLUP);
  pinMode(HeadUp, INPUT_PULLUP);
  pinMode(HeadDown, INPUT_PULLUP);
  pinMode(StripOffOut, INPUT_PULLUP);
  // END OF PINMODE
  
  Serial.begin(9600);
  Serial.println("Starting...");
  Serial.println("Program Version 1.2.7");
  lcd.begin(20, 4);
  lcd.setCursor(0, 0);
  lcd.print("Run Time: ");
  lcd.setCursor(2, 1);
  lcd.print("*** BOOTING ***");
  
  //Load EEPROM Memory
  for (int k = 0; k < 6; k++) {
    int f = k * 2;
    int ytemp = EEPROM.read(f);
    ytemp = ytemp * 10;
    f++;
    int gtemp = gtemp = EEPROM.read(f);
    gtemp = gtemp * 10;
    y[k] = gtemp + ytemp;
    Serial.print("EEPROM[");
    Serial.print(k);
    Serial.print("]: ");
    Serial.println(y[k]);
    delay(10);
  }
  Serial.println("********** System Variables ***********");
  Serial.print("Button Wait Time: ");
  Serial.println(buttonWait);
  Serial.print("LCD Clear Time: ");
  Serial.println(LCDClearTime);
  Serial.print("LCD Default POS: ");
  Serial.println(pos);
  Serial.print("Override Passcode: ");
  Serial.println(passcode);
  Serial.println();
}

void loop() {
  unsigned long currentTime = millis();
  lcdClear();
  lcd.setCursor(10, 0);
  lcd.print(millis() / 1000);
  if (SOverride == 0 || SOverride == 1) {
    if (SOverride == 0) {
      for (int indx = 0; indx < 7; indx++) {
        StateArray[indx] = 0;
        Serial.print("Relay status INDEX: ");
        Serial.print(indx);
        Serial.println(" reset.");
      }
      digitalWrite(PanelLed1, LOW);
      digitalWrite(PanelLed2, LOW);
      digitalWrite(PanelLed3, LOW);
      digitalWrite(PanelLed4, LOW);
      digitalWrite(PanelLed5, LOW);
      SOverride = 1;
    }
    BNextLogic = digitalRead(NextButton);
    if ((BNextLogic == HIGH) && (currentTime - buttonPreviousTime >= buttonWait)) {
      buttonPreviousTime = currentTime;
      x++;
      if (x >= 6) {
        x = 0;
        Serial.print("Time VAR: ");
        Serial.print(x + 1);
        Serial.print(" selected. | ");
        Serial.println(y[x]);
      }
      else {
        Serial.print("Time VAR: ");
        Serial.print(x + 1);
        Serial.print(" selected. | ");
        Serial.println(y[x]);
      }
    }
    BUpLogic = digitalRead(UpButton);
    if ((BUpLogic == HIGH) && (currentTime - buttonPreviousTime >= buttonWait)) {
      y[x] = y[x] + 100;
      buttonPreviousTime = currentTime;
      Serial.print("TimeVar ");
      Serial.print(x + 1);
      Serial.print(" is now: ");
      Serial.println(y[x]);
    }
    BDownLogic = digitalRead(DownButton);
    if ((BDownLogic == HIGH) && (currentTime - buttonPreviousTime >= buttonWait)) {
      y[x] = y[x] - 100;
      buttonPreviousTime = currentTime;
      Serial.print("TimeVar ");
      Serial.print(x + 1);
      Serial.print(" is now: ");
      Serial.println(y[x]);
    }
    SaveButtonTrigger = digitalRead(SaveButton);
    if ((SaveButtonTrigger == HIGH) && (currentTime - buttonPreviousTime >= buttonWait)) {
      buttonPreviousTime = currentTime;
      if (SOverride == 0 || SOverride == 1) {
        Serial.print("TimeVar ");
        Serial.print(x + 1);
        Serial.println(" saved.");
        savetrigger(x);
      }
    }
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
    if (Active == 1) {
      digitalWrite(MainAir, HIGH);
      ManualFeed = digitalRead(StartFeedButton);
      FeedLoop = digitalRead(HookCycleStart);
      FeedCheck = digitalRead(HangerRackFull);
      if (((FeedLoop == LOW) && (Error == 0)) || ((SecStart == 1) && (FeedCheck == LOW)) || (ManualFeed == HIGH)) {
        //if ((FeedLoop == LOW) && (Error == 0) || (ManualFeed == HIGH)){
        if (FeedNext == 0) {
          // FEED ACTIVATED
          Serial.println("Feed Cycle Activated");
          FeedCheck = digitalRead(HangerRackFull);
          if ((FeedCheck == LOW) && (SecStart != 1)) {
            Error = 0;
            digitalWrite(ErrorLed, LOW);
          }
          if (LogicCount == 0) {
            precountTime = currentTime;
          }
          if ((FeedCheck == HIGH) && (SecStart != 1)) {
            Serial.println("ERROR: Hanger Rack NOT full.");
            lcd.setCursor(0, 3);
            lcd.print("ERROR: Hanger Rack");
            preLCDClear = currentTime;
            Error = 1;
            SecStart = 1;
          }
          else {
            LogicCount++;
            SecStart = 0;
            Error = 0;
            lcd.setCursor(0, 1);
            lcd.print("SYSTEM:             ");
            lcd.setCursor(8,1);
            lcd.print(LogicCount);
            digitalWrite(PanelLed1, HIGH);
            FeedNext = 1;
            previousTimer1 = currentTime;
          }
        }
      }
      // FEED OPEN
      if ((FeedNext == 1) && (currentTime - previousTimer1 >= y[0])) {
        previousTimer1 = currentTime;
        digitalWrite(FeedTable, HIGH);
        Serial.println("Feed Cycle | FEED OPEN");
        FeedNext = 2;
      }
      //FEED CLOSE
      if ((FeedNext == 2) && (currentTime - previousTimer1 >= y[1])) {
        Serial.println("Feed Cycle | FEED CLOSE");
        previousTimer1 = currentTime;
        digitalWrite(FeedTable, LOW);
        digitalWrite(PanelLed1, LOW);
        FeedNext = 0;
      }
      // END OF FEED CYCLE
      // Vibrator Cycle
      RailCheck = digitalRead(HookRailFull);
      if ((RailCheck == HIGH) && (RailCheckNext == 0)) {
        Serial.println("Rail Check Activated");
        previousTimer2 = currentTime;
        digitalWrite(PanelLed5, HIGH);
        digitalWrite(HookShaker, HIGH);
        RailCheckNext = 1;
      }
      if (RailCheckNext == 1) {
        RailCheck = digitalRead(HookRailFull);
        if (RailCheck == LOW) {
          previousTimer2 = currentTime;
          RailCheckNext = 2;
        }
      }
      if (RailCheckNext == 2) {
        RailCheck = digitalRead(HookRailFull);
        if (RailCheck == HIGH) {
          RailCheckNext = 1;
        }
        if ((RailCheck == LOW) && (currentTime - previousTimer2 >= y[5])) {
          digitalWrite(HookShaker, LOW);
          digitalWrite(PanelLed5, LOW);
          previousTimer2 = currentTime;
          Serial.println("Rail Check Finsihed");
          RailCheckNext = 0;
        }
      }
      // END OF VIBRATOR CYCLE
      // Crimp Cycle
      CrimpLoop = digitalRead(CrimpCycleStart);
      if ((CrimpLoop == LOW) && (CrimpNext == 0)) {
        Serial.println("Crimp Cycle Activated");
        digitalWrite(PanelLed4, HIGH);
        digitalWrite(CrimpStopper, HIGH);
        previousTimer4 = currentTime;
        CrimpNext = 1;
      }
      if ((CrimpNext == 1) && (currentTime - previousTimer4 >= y[3])) {
        previousTimer4 = currentTime;
        digitalWrite(Crimp, HIGH);
        Serial.println("Crimp Cycle | Crimp");
        CrimpNext = 2;
      }
      if ((CrimpNext == 2) && (currentTime - previousTimer4 >= y[4])) {
        Serial.println("Crimp Cycle | Reset");
        previousTimer4 = currentTime;
        digitalWrite(Crimp, LOW);
        digitalWrite(CrimpStopper, LOW);
        digitalWrite(PanelLed4, LOW);
        CrimpNext = 0;
      }
      // Hook Cycle
      HookLoop = digitalRead(HookCycleStart);
      if ((HookLoop == LOW) && (HookNext == 0)) {
        Serial.println("Hook Cycle Activated");
        digitalWrite(PanelLed2, HIGH);
        boolean HookCheck;
        HookCheck = digitalRead(HookRailEmpty);
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
          digitalWrite(HookStopper, HIGH);
          HookNext = 1;
        }
      }
      if ((HookNext == 1) && (currentTime - previousTimer3 >= y[2])) {
        previousTimer3 = currentTime;
        Serial.println("Hook Cycle | Tool/Head OUT");
        digitalWrite(ToolHead, HIGH);
        HookNext = 2;
      }
      if (HookNext == 2) {
        int HeadCheckDown = digitalRead(HeadDown);
        if (HeadCheckDown == LOW) {
          digitalWrite(StripOff, HIGH);
          HookNext = 3;
          Serial.println("Hook Cycle | Strip Off OUT");
        }
      }
      if (HookNext == 3) {
        int StripoffCheck = digitalRead(StripOffOut);
        if (StripoffCheck == LOW) {
          digitalWrite(ToolHead, LOW);
          digitalWrite(PanelLed3, HIGH);
          digitalWrite(PanelLed2, LOW);
          HookNext = 4;
        }
      }
      if (HookNext == 4) {
        int HeadUpCheck = digitalRead(HeadUp);
        if (HeadUpCheck == LOW) {
          Serial.println("Hook Cycle | Reset");
          digitalWrite(StripOff, LOW);
          digitalWrite(HookStopper, LOW);
          digitalWrite(PanelLed3, LOW);
          HookNext = 0;
        }
      }// END OF HOOK CYCLE
      if (LogicCount >= 100) {
        TimeKeeper();
      }
    }//END OF ACTIVE
    if (Active == 0) {
      lcd.setCursor(0, 2);
      lcd.print("Time:");
      inactive(x);
    } // End of Active 0 (containing switch)
  } // End of Override Statement
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
    int SaveButtonTrigger = 0;
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
      x = 0;
    }
    else {
      char key;
      key = keypad.getKey();
      if (key) {
        char bxyz[] = {0};
        bxyz[j++] = key;
        bxyz[j];
        int tempb = atoi(bxyz);
        Override_Trigger(tempb);
        j = 0;
      }
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


void inactive(int x) {
  digitalWrite(ErrorLed, HIGH);
  digitalWrite(FeedTable, LOW);
  digitalWrite(ToolHead, LOW);
  digitalWrite(HookStopper, LOW);
  digitalWrite(CrimpStopper, LOW);
  digitalWrite(Crimp, LOW);
  digitalWrite(MainAir, LOW);
  digitalWrite(HookShaker, LOW);
  if (y[6] == passcode) {
    Serial.println("***** Override ACTIVATED *****");
    y[6] = 0;
    SOverride = 2;
    return;
  }
  switch (x) {
    case 0:
      setLEDS(PanelLed1);
      lcd.setCursor(0, 1);
      lcd.print("Feed Wait Time:     ");
      changetime(x);
      break;
    case 1:
      setLEDS(PanelLed2);
      lcd.setCursor(0, 1);
      lcd.print("Feed Open Time      ");
      changetime(x);
      break;
    case 2:
      setLEDS(PanelLed3);
      lcd.setCursor(0, 1);
      lcd.print("Hook Cycle Wait     ");
      changetime(x);
      break;
    case 3:
      setLEDS(PanelLed4);
      lcd.setCursor(0, 1);
      lcd.print("Crimp Cycle Wait    ");
      changetime(x);
      break;
    case 4:
      setLEDS(PanelLed5);
      lcd.setCursor(0, 1);
      lcd.print("Crimp Time          ");
      changetime(x);
      break;
    case 5:
      setLEDS(PanelLed1);
      lcd.setCursor(0, 1);
      lcd.print("Vibrator Time     ");
      changetime(x);
      break;
  } //END OF MAIN SWITCH
} // End of Inactive void


void savetrigger(int x) {
  if (y[x] >= 5101){
    y[x] = 5100;
    lcd.setCursor(0,3);
    lcd.print("Max Value hit!");
    Serial.println("SYSTEM: Max value hit when trying to save.");
  }
  int address = x * 2;
  int ytemp = ytemp = y[x] / 10;
  if (ytemp > 255) {
    ytemp = ytemp - 255;
    EEPROM.update(address, ytemp);
    address = address + 1;
    if (address == EEPROM.length()) {
      address = 0;
      Serial.println("*** SYSTEM ERROR [EE0003]");
    }
    EEPROM.update(address, 255);
    address = address + 1;
    if (address == EEPROM.length()) {
      address = 0;
      Serial.println("*** SYSTEM ERROR [EE0004]");
    }
  }
  if (ytemp < 255) {
    EEPROM.update(address, ytemp);
    address = address + 1;
    if (address == EEPROM.length()) {
      address = 0;
      Serial.println("*** SYSTEM ERROR [EE0005]");
    }
    EEPROM.update(address, 0);
    address = address + 1;
    if (address == EEPROM.length()) {
      address = 0;
      Serial.println("*** SYSTEM ERROR [EE0006]");
    }
  }
  ytemp = x + 1;
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
//************************* VOID END ******************************


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
    digitalWrite(FeedTable, tempstate);
    lcd.setCursor(0, 3);
    lcd.print("Relay 1 SET TO: ");
    lcd.print(lcdstate);
    lcd.print(" ");
    Serial.print("SYSTEM OVERRIDE | Relay 1 | ");
    Serial.println(lcdstate);
    preLCDClear = currentTime;
  }
  if (RTrigger == 2) {
    digitalWrite(HookStopper, tempstate);
    lcd.setCursor(0, 3);
    lcd.print("Relay 2 SET TO: ");
    lcd.print(lcdstate);
    lcd.print(" ");
    Serial.print("SYSTEM OVERRIDE | Relay 2 | ");
    Serial.println(lcdstate);
    preLCDClear = currentTime;
  }
  if (RTrigger == 3) {
    digitalWrite(ToolHead, tempstate);
    lcd.setCursor(0, 3);
    lcd.print("Relay 3 SET TO: ");
    lcd.print(lcdstate);
    lcd.print(" ");
    Serial.print("SYSTEM OVERRIDE | Relay 3 | ");
    Serial.println(lcdstate);
    preLCDClear = currentTime;
  }
  if (RTrigger == 4) {
    digitalWrite(StripOff, tempstate);
    lcd.setCursor(0, 3);
    lcd.print("Relay 4 SET TO: ");
    lcd.print(lcdstate);
    lcd.print(" ");
    Serial.print("SYSTEM OVERRIDE | Relay 4 | ");
    Serial.println(lcdstate);
    preLCDClear = currentTime;
  }
  if (RTrigger == 5) {
    digitalWrite(CrimpStopper, tempstate);
    lcd.setCursor(0, 3);
    lcd.print("Relay 5 SET TO: ");
    lcd.print(lcdstate);
    lcd.print(" ");
    Serial.print("SYSTEM OVERRIDE | Relay 5 | ");
    Serial.println(lcdstate);
    preLCDClear = currentTime;
  }
  if (RTrigger == 6) {
    digitalWrite(Crimp, tempstate);
    lcd.setCursor(0, 3);
    lcd.print("Relay 6 SET TO: ");
    lcd.print(lcdstate);
    lcd.print(" ");
    Serial.print("SYSTEM OVERRIDE | Relay 6 | ");
    Serial.println(lcdstate);
    preLCDClear = currentTime;
  }
  if (RTrigger == 7) {
    digitalWrite(HookShaker, tempstate);
    lcd.setCursor(0, 3);
    lcd.print("Relay 7 SET TO: ");
    lcd.print(lcdstate);
    lcd.print(" ");
    Serial.print("SYSTEM OVERRIDE | Relay 7 | ");
    Serial.println(lcdstate);
    preLCDClear = currentTime;
  }
  if (RTrigger == 8) {
    digitalWrite(MainAir, tempstate);
    lcd.setCursor(0, 3);
    lcd.print("Relay 8 SET TO: ");
    lcd.print(lcdstate);
    lcd.print(" ");
    Serial.print("SYSTEM OVERRIDE | Relay 8 | ");
    Serial.println(lcdstate);
    preLCDClear = currentTime;
  }
  else {
  }
}


void changetime(int x) {
  unsigned long currentTime = millis();
  lcd.setCursor(5, 2);
  lcd.print(y[x]);
  lcd.print("      ");
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
        y[6] = passcode;
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
      if (tempa > 2550) {
        y[x] = tempa;
        int ytemp = 0;
        int address = 0;
        ytemp = y[x] / 10;
        address = x * 2;
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
        Serial.println(x);
      }
      if (tempa < 2550) {
        y[x] = tempa;
        int ytemp = 0;
        int address = 0;
        ytemp = y[x] / 10;
        address = x * 2;
        EEPROM.update(address, ytemp);
        address = address + 1;
        if (address == EEPROM.length()) {
          address = 0;
        }
        EEPROM.update(address, 0);
        if (address == EEPROM.length()) {
          address = 0;
        }
        Serial.print("EEPROM | ");
        Serial.print(ytemp);
        Serial.print(" was wrote to EEPROM address: ");
        Serial.println(x);
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

//End of ChangeTime Void
void lcdClear() {
  unsigned long currentTime = millis();
  if (currentTime - preLCDClear >= LCDClearTime)
  {
    preLCDClear = currentTime;
    lcd.setCursor(0, 3);
    lcd.print("                    ");
  }
}


void setLEDS(byte LEDSnumber)
{
  digitalWrite(PanelLed1, LOW);
  digitalWrite(PanelLed2, LOW);
  digitalWrite(PanelLed3, LOW);
  digitalWrite(PanelLed4, LOW);
  digitalWrite(PanelLed5, LOW);

  digitalWrite(LEDSnumber, HIGH);
}


void setLED(byte LEDnumber)
{
  if (LEDnumber == PanelLed4 || LEDnumber == PanelLed5) {
    digitalWrite(PanelLed4, LOW);
    digitalWrite(PanelLed5, LOW);

    digitalWrite(LEDnumber, HIGH);
  }
  if (LEDnumber == PanelLed1) {
    digitalWrite(PanelLed1, LOW);

    digitalWrite(LEDnumber, HIGH);
  }
  if (LEDnumber == PanelLed3 || LEDnumber == PanelLed2) {
    digitalWrite(PanelLed3, LOW);
    digitalWrite(PanelLed2, LOW);
    digitalWrite(LEDnumber, HIGH);
  }
}


void TimeKeeper() {
  unsigned long tempvarj = ((millis() - precountTime) / 1000);
  Serial.print("CTN Run Time: ");
  Serial.println(tempvarj);
  lcd.setCursor(0, 2);
  lcd.print("CTN Time:           ");
  lcd.setCursor(15, 2);
  lcd.print(tempvarj);
  LogicCount = 0;
}
