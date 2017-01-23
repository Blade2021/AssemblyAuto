#include <EEPROM.h>

//System Load
const int ErrorLED = 12; //Error LED
const int StopButton1 = 2;
const int StopButton2 = 4;
const int ResetButton = 3;

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
const int MotorRelay = 41;

//Load in Variables
int Safe = 0;
int Error = 0;
int ResetCheck = 0;

//Buttons
const int UpButton = 6;
const int DownButton = 7;
const int ToggleButton = 40;
const int NextButton = 8;
int buttonWait = 300;
unsigned long buttonPreviousTime = 0;

//LCD Variables
unsigned long buttonCurrentTime = 0;
int BNextLogic = 0;
int BUpLogic = 0;
int BDownLogic = 0;
int ToggleLogic = 0;
int x = 0;
int a = 1;

//Setting up the Timers
unsigned long previousTimer1 = 0;
unsigned long previousTimer2 = 0;
unsigned long previousTimer3 = 0;
unsigned long previousTimer4 = 0;
unsigned long previousTimer5 = 0;
unsigned long y[] = {1000, 1000, 1000, 2300, 2000, 3000}; // TIME VARIABLES


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
  pinMode(ErrorLED, OUTPUT);
  pinMode(ToolHead, OUTPUT);
  pinMode(StripOff, OUTPUT);
  pinMode(HookStopper, OUTPUT);
  pinMode(CrimpStopper, OUTPUT);
  pinMode(Crimp, OUTPUT);
  pinMode(FeedTable, OUTPUT);
  pinMode(MainAir, OUTPUT);
  pinMode(HookShaker, OUTPUT);
  pinMode(MotorRelay, OUTPUT);

  //Assign Variables
  digitalWrite(MainAir, HIGH);
  digitalWrite(MotorRelay, HIGH);
}
void loop() {
  //Safety Check
  Safe = (digitalRead(StopButton1) + digitalRead(StopButton2));
  if (Safe >= 1){
    digitalWrite(MainAir, LOW);
    digitalWrite(MotorRelay, LOW);
//    digitalWrite(MotorControl, LOW);    //******** MOTOR CONTROL RELAY ?? *************
    digitalWrite(ErrorLED, HIGH);
    digitalWrite(MotorRelay, LOW);
    Serial.println("Stop Button Activated");
    return;
    }
  if (Error >= 1){
    digitalWrite(MainAir, LOW);
//    digitalWrite(MotorRelay, LOW);
//    digitalWrite(MotorControl, LOW);    //******** MOTOR CONTROL RELAY ?? *************
    digitalWrite(ErrorLED, HIGH);
    return;
    }
  ResetCheck = digitalRead(ResetButton);
  if (ResetCheck == HIGH){
    Error = 0;
    Safe = 1;
    Serial.println("Reset Activated");
    digitalWrite(MainAir, HIGH);
    //digitalWrite(MotorRelay, HIGH);
    digitalWrite(ErrorLED, LOW);
  }
  
  //MAIN MACHINE LOOP START
  FeedLoop = digitalRead(StartFeedButton) + digitalRead(FeedTrigger);  //Cycle Start Button  OR  Feed Sensor Activates
  if (FeedLoop >= 1){
    Serial.println("Feed Cycle Activated");
    FeedCheck = analogRead(HangerRackFull);
    if (FeedCheck < 10){
      Serial.println("ERROR: Hanger Rack NOT Full.");
      Error = 1;
      digitalWrite(ErrorLED, HIGH);
      return;
      }
      unsigned long currentTimer1 = millis();
      if (currentTimer1 - previousTimer1 >= y[0]){
        Serial.println("Feed Cycle [FEED OPEN]");
        //previousTimer1 = currentTimer1;
        digitalWrite(FeedTable, HIGH);
        FeedNext = 1;
        FeedLoop = 0;
      }
  }
  if (FeedNext == 1){
      unsigned long currentTimer1 = millis();
       if (currentTimer1 - previousTimer1 >= y[1]){
        Serial.println("Feed Cycle [FEED CLOSE]");
        previousTimer1 = currentTimer1;
        digitalWrite(FeedTable, LOW);
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
      unsigned long currentTimer3 = millis();
      if (currentTimer3 - previousTimer3 >= y[2]){
        digitalWrite(HookShaker, LOW);
        previousTimer3 = currentTimer3;
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
    HookCheck = analogRead(HookRailEmpty);
    if (HookCheck <= 10){
      Serial.println("ERROR: Hook Check failed");
      Error = 1;
      digitalWrite(ErrorLED, HIGH);
      return;
      }else{
    //no timer
    digitalWrite(HookStopper, HIGH);
    HookNext = 1;
    HookLoop = 0;
    }
  }
  if (HookNext == 1){
    unsigned long currentTimer2 = millis();
    if (currentTimer2 - previousTimer2 >= y[3]){
      //previousTimer2 - currentTimer2;
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
    //wait for head to register down position
    digitalWrite(StripOff, HIGH);
  }
  if (HookNext == 3){
    Serial.println("Hook Cycle (Step 4)");
    int StripoffCheck = 0;
    StripoffCheck = digitalRead(StripOffOut);
    if (StripoffCheck <= 10){
      return;
    }
    //wait for strip off to register out position
    digitalWrite(ToolHead, LOW);
    HookNext = 4;
  }
  if (HookNext == 4){
    Serial.println("Hook Cycle (Step 5)");
    int HeadUpCheck = 0;
    HeadUpCheck = analogRead(HeadUp);
    if (HeadUpCheck < 400){
      return;
    }
    //wait for head to register up position
    digitalWrite(StripOff, LOW);
    digitalWrite(HookStopper, LOW);
    HookNext = 0;
  }
  CrimpLoop = analogRead(CrimpCycleStart);
  if (CrimpLoop >= 10){
    Serial.println("Crimp Cycle Activated");
    digitalWrite(CrimpStopper, HIGH);
    unsigned long currentTimer4 = millis();
    if (currentTimer4 - previousTimer4 >= y[4]){
      previousTimer4 - currentTimer4;
      digitalWrite(Crimp, HIGH);
      CrimpNext = 1;
      CrimpLoop = 0;
    }
  }
  if (CrimpNext == 1){
    unsigned long currentTimer5 = millis();
    if (currentTimer5 - previousTimer5 >= y[5]){
      Serial.println("Crimp Cycle (Step 2)");
      previousTimer5 - currentTimer5;
      digitalWrite(Crimp, LOW);
      digitalWrite(CrimpStopper, LOW);
      CrimpLoop = 0;
      CrimpNext = 0;
    }
  }
}
void StopAll(){
  digitalWrite(MainAir, LOW);
  Serial.println("EMERGENCY STOP TRIGGERED");
  digitalWrite(ErrorLED, HIGH);
  ResetCheck = digitalRead(ResetButton);
  if (ResetCheck == HIGH){
    return;
  }
  else{
  }
}
void LCD() {
  buttonCurrentTime = millis();
  BNextLogic = digitalRead(NextButton);
  if ((BNextLogic == HIGH) && (buttonCurrentTime - buttonPreviousTime >= buttonWait)){
    x++;
    if (x >= 7){
      x = 0;
      Serial.print("Time VAR: ");
      Serial.print(x+1);
      Serial.println(" selected");
    }
    else {
    Serial.print("Time VAR: ");
    Serial.print(x+1);
    Serial.println(" selected");
    }
  }
  BUpLogic = digitalRead(UpButton);
  if ((BUpLogic == HIGH) && (buttonCurrentTime - buttonPreviousTime >= buttonWait)){
    buttonPreviousTime = buttonCurrentTime;
    y[x] = y[x] + 100;
    Serial.print("TimeVar ");
    Serial.print(x+1);
    Serial.print(" is now: ");
    Serial.println(y[x]);
  }
  BDownLogic = digitalRead(DownButton);
  if ((BDownLogic == HIGH) && (buttonCurrentTime - buttonPreviousTime >= buttonWait)){
    y[x] = y[x] - 100;
    buttonPreviousTime = buttonCurrentTime;
    Serial.print("TimeVar ");
    Serial.print(x+1);
    Serial.print(" is now: ");
    Serial.println(y[x]);
  }
}
