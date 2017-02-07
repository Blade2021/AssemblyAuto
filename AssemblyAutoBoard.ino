#include <Keypad.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>

//System Load
const int StopButton1 = 2;
const int StopButton2 = 4;
const int ResetButton = 3;

//LEDs
const int FeedLed = 43;
const int PanelLed2 = 44;
const int HookLed = 45;
const int CrimpLed = 46;
const int PanelLed5 = 47;
const int ErrorLed = 38;

//Load Sensors
const int HookRailEmpty = 15;
const int HookRailFull = 16;
const int HangerRackFull = 17;
const int HookCycleStart = 18;
const int FeedTrigger = 19;
const int HeadUp = 20;
const int HeadDown = 21;
const int StripOffOut = 22;
const int CrimpCycleStart = 23;
const int CrimpMicro = 24;
const int StartFeedButton = 25;

//Load Solenoids
const int ToolHead = 25;
const int StripOff = 26;
const int HookStopper = 27;
const int CrimpStopper = 28;
const int Crimp = 29;
const int FeedTable = 30;
const int MainAir = 31;
//System Outputs
const int HookShaker = 32; //*** 120 VAC ***
const int MotorRelay = 41; //*** 120 VAC ***

//Load in Variables
int Safe = 0;
int Error = 0;
int ResetCheck = 0;

//Buttons
const int NextButton = 26;
const int SaveButton = 30;
const int UpButton = 32;
const int DownButton = 28;
const int ToggleButton = 40;
int buttonWait = 300;

//LCD Variables
int BNextLogic = 0;
int BUpLogic = 0;
int BDownLogic = 0;
int BSelLogic = 0;
int x = 0;
int a = 1;
int LCDClearTime = 5000;
int pos=14;
int j = 0;
char arraya [] = {0, 1, 2, 3, 0};

//Setting up the Timers
unsigned long preLCDClear = 0;
unsigned long buttonPreviousTime = 0;
unsigned long previouscurrentTime = 0;
unsigned long previousTimer2 = 0;
unsigned long previousTimer3 = 0;
unsigned long previousTimer4 = 0;
unsigned long previousTimer5 = 0;
unsigned long y[] = {1000, 1000, 1000, 2300, 2000, 3000}; // TIME VARIABLES

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {53, 51, 49, 47}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {45, 43, 41, 39};
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

int FeedLoop = 0;
int FeedCheck = 0;
int FeedNext = 0;
int HookNext = 0;
int HookLoop = 0;
int HookCheck = 0;
int CrimpLoop = 0;
int CrimpNext = 0;
int RailCheck = 0;
int RailCheckNext = 0;

//*************************** SETUP START ****************************
void setup(){
  Serial.begin(9600); //Set Serial Speed
  Serial.println("Starting Setup...");
  attachInterrupt(2, StopAll, FALLING);
  for (int i = 0; i < 5; i++){
    int f = EEPROM.read(i);
    if (!f){
      Error = 1;
      Serial.println("Error reading EEPROM");
      return;
    }
    f = f * 100;
    y[i] = f;
    Serial.print("Time Var");
    Serial.print(i);
    Serial.print(":");
    Serial.print(f);
    delay(100);
  }
  //INPUT
  pinMode(StopButton1, INPUT);
  pinMode(StopButton2, INPUT);
  pinMode(HookRailEmpty, INPUT);
  pinMode(HookRailFull, INPUT);
  pinMode(HangerRackFull, INPUT);
  pinMode(HookCycleStart, INPUT);
  pinMode(FeedTrigger, INPUT);
  pinMode(HeadUp, INPUT);
  pinMode(HeadDown, INPUT);
  pinMode(StripOffOut, INPUT);
  pinMode(CrimpCycleStart, INPUT);
  pinMode(CrimpMicro, INPUT);
  pinMode(StartFeedButton, INPUT);
  pinMode(ResetButton, INPUT);
  //OUTPUT
  pinMode(ToolHead, OUTPUT);
  pinMode(StripOff, OUTPUT);
  pinMode(HookStopper, OUTPUT);
  pinMode(CrimpStopper, OUTPUT);
  pinMode(Crimp, OUTPUT);
  pinMode(FeedTable, OUTPUT);
  pinMode(MainAir, OUTPUT);
  pinMode(HookShaker, OUTPUT);
  pinMode(MotorRelay, OUTPUT);
  //LED
  pinMode(FeedLed, OUTPUT);
  pinMode(PanelLed2, OUTPUT);
  pinMode(HookLed, OUTPUT);
  pinMode(CrimpLed, OUTPUT);
  pinMode(PanelLed5, OUTPUT);
  pinMode(ErrorLed, OUTPUT);
  //Assign Variables
  digitalWrite(MainAir, HIGH);
  digitalWrite(MotorRelay, HIGH);
  Serial.println("********** System Variables ***********");
  Serial.print("Button Wait Time: ");
  Serial.println(buttonWait);
  Serial.print("LCD Clear Time: ");
  Serial.println(LCDClearTime);
  Serial.print("LCD Default POS: ");
  Serial.println(pos);
  Serial.println("********** End of Setup ***********");
}
void loop() {
  unsigned long currentTime = millis();  //TIME
  lcdClear();
  lcd.setCursor(11,0);
  lcd.print(millis() / 1000);

  
  //**************    Safety Check   ************
  Safe = (digitalRead(StopButton1) + digitalRead(StopButton2));
  if (Safe >= 1){
    digitalWrite(MainAir, LOW);
    digitalWrite(MotorRelay, LOW);
//    digitalWrite(MotorControl, LOW);    //******** MOTOR CONTROL RELAY ?? *************
    digitalWrite(ErrorLed, HIGH);
    digitalWrite(MotorRelay, LOW);
    Serial.println("Stop Button Activated");
    return;
    }
  if (Error >= 1){
    digitalWrite(MainAir, LOW);
//    digitalWrite(MotorRelay, LOW);
//    digitalWrite(MotorControl, LOW);    //******** MOTOR CONTROL RELAY ?? *************
    digitalWrite(ErrorLed, HIGH);
    return;
    }
  ResetCheck = digitalRead(ResetButton);
  if (ResetCheck == HIGH){
    Error = 0;
    Safe = 1;
    Serial.println("Reset Activated");
    digitalWrite(MainAir, HIGH);
    //digitalWrite(MotorRelay, HIGH);
    digitalWrite(ErrorLed, LOW);
  }
  boolean SaveButtonTrigger = LOW;
  BNextLogic = digitalRead(NextButton);
  if ((BNextLogic == HIGH) && (currentTime - buttonPreviousTime >= buttonWait)){
    buttonPreviousTime = currentTime;
    x++;
    if (x >= 5){
      x = 0;
      Serial.print("Time VAR: ");
      Serial.print(x+1);
      Serial.print(" selected. | ");
      Serial.println(y[x]);
    }
    else {
    Serial.print("Time VAR: ");
    Serial.print(x+1);
    Serial.print(" selected. | ");
    Serial.println(y[x]);
    }
  }
  BUpLogic = digitalRead(UpButton);
  if ((BUpLogic == HIGH) && (currentTime - buttonPreviousTime >= buttonWait)){
    y[x] = y[x]+100;
    buttonPreviousTime = currentTime;
    Serial.print("TimeVar ");
    Serial.print(x+1);
    Serial.print(" is now: ");
    Serial.println(y[x]);
    lcd.setCursor(0,3);
    lcd.print("Var[");
    lcd.print(x);
    lcd.print("] set to:");
    lcd.print(y[x]);
  }
  BDownLogic = digitalRead(DownButton);
  if ((BDownLogic == HIGH) && (currentTime - buttonPreviousTime >= buttonWait)){
    y[x] = y[x]-100;
    buttonPreviousTime = currentTime;
    Serial.print("TimeVar ");
    Serial.print(x+1);
    Serial.print(" is now: ");
    Serial.println(y[x]);
  }
  SaveButtonTrigger = digitalRead(SaveButton);
  if ((SaveButtonTrigger) && (currentTime - buttonPreviousTime >= buttonWait)){
    buttonPreviousTime = currentTime;
    Serial.print("TimeVar ");
    Serial.print(x+1);
    Serial.println(" saved.");
    savetrigger(x);
  }

  
  //MAIN MACHINE LOOP START
  FeedLoop = digitalRead(StartFeedButton) + digitalRead(FeedTrigger);  //Cycle Start Button  OR  Feed Sensor Activates
  if (FeedLoop >= 1){
    Serial.println("Feed Cycle Activated");
    FeedCheck = analogRead(HangerRackFull);
    if (FeedCheck < 10){
      Serial.println("ERROR: Hanger Rack NOT Full.");
      Error = 1;
      digitalWrite(ErrorLed, HIGH);
      return;
    }
    if (currentTime - previouscurrentTime >= y[0]){
      Serial.println("Feed Cycle [FEED OPEN]");
      previouscurrentTime = currentTime;
      digitalWrite(FeedTable, HIGH);
      digitalWrite(FeedLed, HIGH);
      FeedNext = 1;
      FeedLoop = 0;
    }
  }
  if (FeedNext == 1){
       if (currentTime - previouscurrentTime >= y[1]){
        Serial.println("Feed Cycle [FEED CLOSE]");
        previouscurrentTime = currentTime;
        digitalWrite(FeedTable, LOW);
        digitalWrite(FeedLed, LOW);
        FeedNext = 0;  //End the Feed until called again
      }
  }
  RailCheck = analogRead(HookRailFull);
  if (RailCheck <= 10){
    Serial.println("Rail Check Activated");
    digitalWrite(HookShaker, HIGH);
    RailCheckNext = 1;
  }
  if (RailCheckNext == 1){
    if (RailCheck > 10){
      if (currentTime - previousTimer3 >= y[2]){
        digitalWrite(HookShaker, LOW);
        previousTimer3 = currentTime;
        RailCheckNext = 0;
      }
      else{
        return;
      }
    }
  }
  HookLoop = analogRead(HookCycleStart);
  if (HookLoop >= 10){
    Serial.println("Hook Cycle Activated");
    digitalWrite(HookLed, HIGH);
    HookCheck = analogRead(HookRailEmpty);
    if (HookCheck <= 10){
      Serial.println("ERROR: Hook Check failed");
      Error = 1;
      digitalWrite(HookLed, LOW);
      digitalWrite(ErrorLed, HIGH);
      return;
      }else{
      //no timer
      digitalWrite(HookStopper, HIGH);
      HookNext = 1;
      HookLoop = 0;
    }
  }
  if (HookNext == 1){
    if (currentTime - previousTimer2 >= y[3]){
      previousTimer2 - currentTime;
      Serial.println("Hook Cycle (Step 2)");
    digitalWrite(ToolHead, HIGH);
    HookNext = 2;
    }
  }
  if (HookNext == 2){
    Serial.println("Hook Cycle (Step 3)");
    int HeadCheckDown = 0;
    HeadCheckDown = analogRead(HeadDown);
    if (HeadCheckDown < 400){
      return;
    }
    else {
      //wait for head to register down position
      digitalWrite(StripOff, HIGH);
      HookNext = 3;
    }
  }
  if (HookNext == 3){
    Serial.println("Hook Cycle (Step 4)");
    int StripoffCheck = 0;
    StripoffCheck = digitalRead(StripOffOut);
    if (StripoffCheck <= 10){
      return;
    }
    else{
      //wait for strip off to register out position
      digitalWrite(ToolHead, LOW);
      HookNext = 4;
    }
  }
  if (HookNext == 4){
    Serial.println("Hook Cycle (Step 5)");
    int HeadUpCheck = 0;
    HeadUpCheck = analogRead(HeadUp);
    if (HeadUpCheck < 400){
      return;
    }
    else {
      //wait for head to register up position
      digitalWrite(StripOff, LOW);
      digitalWrite(HookStopper, LOW);
      digitalWrite(HookLed, LOW);
      HookNext = 0;
    }
  }
  CrimpLoop = analogRead(CrimpCycleStart);
  if (CrimpLoop >= 10){
    Serial.println("Crimp Cycle Activated");
    digitalWrite(CrimpLed, HIGH);
    digitalWrite(CrimpStopper, HIGH);
    if (currentTime - previousTimer4 >= y[4]){
      previousTimer4 = currentTime;
      digitalWrite(Crimp, HIGH);
      CrimpNext = 1;
      CrimpLoop = 0;
    }
  }
  if (CrimpNext == 1){
    if (currentTime - previousTimer5 >= y[5]){
      Serial.println("Crimp Cycle (Step 2)");
      previousTimer5 = currentTime;
      digitalWrite(Crimp, LOW);
      digitalWrite(CrimpStopper, LOW);
      digitalWrite(CrimpLed, LOW);
      CrimpLoop = 0;
      CrimpNext = 0;
    }
  }
  else {
        digitalWrite(ErrorLed, HIGH);
        switch (x){
          case 0:
            setLED(FeedLed);
            lcd.setCursor(0,1);
            lcd.print("Feed Wait Time:     ");
            changetime(x);
            break;
          case 1:
            setLED(PanelLed2);
            lcd.setCursor(0,1);
            lcd.print("Feed Open Time      ");
            changetime(x);
            break;
          case 2:
            setLED(HookLed);
            lcd.setCursor(0,1);
            lcd.print("Hook Cycle Wait     ");
            changetime(x);
            break;
          case 3:
            setLED(CrimpLed);
            lcd.setCursor(0,1);
            lcd.print("Crimp Cycle Wait    ");
            changetime(x);
            break;
          case 4:
            setLED(PanelLed5);
            lcd.setCursor(0,1);
            lcd.print("Crimp Time          ");
            changetime(x);
            break;
        }
    }
}
void StopAll(){
  digitalWrite(MainAir, LOW);
  Serial.println("EMERGENCY STOP TRIGGERED");
  digitalWrite(ErrorLed, HIGH);
  ResetCheck = digitalRead(ResetButton);
  if (ResetCheck == HIGH){
    return;
  }
  else{
  }
}
void savetrigger(int x){
  int address = 0;
  address = x;
  int ytemp = 0;
  ytemp = y[x]/10;
  /*EEPROM.update(address,ytemp);
  address = address + 1;
  if (address == EEPROM.length()){
    address = 0;
  }*/
  ytemp=x+1;
  lcd.setCursor(0,3);
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
void changetime(int x){
  lcd.setCursor(5,2);
  lcd.print(y[x]);
  lcd.print("     ");
  lcd.setCursor(pos,2);
  char key;
  key = keypad.getKey();
  if(key){
    lcd.print(key);
    pos++;
    lcd.setCursor(pos,2);
    arraya[j++] = key;
    arraya[j];
    if (pos > 20){
      pos = 14;
    }
    if(key=='*'){
      int tempa = atoi(arraya);
      Serial.println(tempa);
      if (tempa > 2550){
        tempa = 2550;
        Serial.println("WARNING: MAX VALUE HIT");
        lcd.setCursor(0,3);
        lcd.print("ERROR: MAX VALUE HIT");
        unsigned long currentTime = millis();
        preLCDClear = currentTime;
      }
      y[x]=tempa;
      int ytemp = 0;
      int address = 0;
      ytemp = y[x]/10;
      address = x;
      /*EEPROM.update(address, ytemp);
      address = address + 1;
      if (address == EEPROM.length()){
        address = 0;
      }*/
      Serial.print(ytemp);
      Serial.print(" was wrote to EEPROM address: ");
      Serial.println(x);
      Serial.println("Ran array process function.");
      pos = 14;
      lcd.setCursor(pos,2);
      lcd.print("       ");
      j = 0;
      return;
    }
    if(key=='#'){
      pos = 14;
      lcd.setCursor(pos,2);
      lcd.print("       ");
      j = 0;
      return;
    }
  } //End of If(Key)
} //End of ChangeTime Void
void lcdClear(){
  unsigned long currentTime = millis();
  if(currentTime - preLCDClear >= LCDClearTime)
  {
    lcd.setCursor(0,3);
    lcd.print("                    ");
  }
}
void setLED(byte LEDnumber)
{
  digitalWrite(FeedLed, LOW);
  digitalWrite(PanelLed2, LOW);
  digitalWrite(HookLed, LOW);
  digitalWrite(CrimpLed, LOW);
  digitalWrite(PanelLed5, LOW);
  digitalWrite(LEDnumber, HIGH);
}
