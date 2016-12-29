//System Load
//const int buttonPin = 2; //Button pin attached to board
//const int stopButtonPin = 3;
//const int ledPin = 13; //Pin attached to LED
const int ErrorLED = 12; //Error LED
const int StopButton1 = 3;
const int StopButton2 = 4;

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

//Load in Variables
int Safe = 0;
int Error = 0;

//Setting up the Timers
unsigned long previousTimer1 = 0;
unsigned long previousTimer2 = 0;
unsigned long previousTimer3 = 0;
unsigned long previousTimer4 = 0;
unsigned long previousTimer5 = 0;
const long Timer1INV = 1000;
const long Timer1INV2 = 1000;
const long Timer2INV = 1000;
const long Timer3INV = 1000;
const long Timer4INV = 1000;
const long Timer5INV = 1000;

int FeedLoop = 0;
int FeedCheck = 0;
int FeedNext = 0;
int HookNext = 0;
int HookLoop = 0;
int HookCheck = 0;
int CrimpLoop = 0;
int CrimpNext = 0;

//*************************** SETUP START ****************************
void setup(){
  Serial.begin(9600); //Set Serial Speed
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
  //OUTPUT
  pinMode(ErrorLED, OUTPUT);
  pinMode(ToolHead, OUTPUT);
  pinMode(StripOff, OUTPUT);
  pinMode(HookStopper, OUTPUT);
  pinMode(CrimpStopper, OUTPUT);
  pinMode(Crimp, OUTPUT);
  pinMode(FeedTable, OUTPUT);
  pinMode(MainAir, OUTPUT);
}
void startup() {
  // confirm all LED's are working properly
//  digitalWrite(ledPin, HIGH);
  digitalWrite(ErrorLED, HIGH);
  delay(2000);
//  digitalWrite(ledPin, LOW);
  digitalWrite(ErrorLED, LOW);
}
void loop() {
  //Safety Check
  Safe = (digitalRead(StopButton1) + digitalRead(StopButton2));
  if (Safe >= 1){
    return;
    }
  if (Error >= 1){
    return;
    }
  FeedLoop = digitalRead(StartFeedButton) + digitalRead(FeedTrigger);  //Cycle Start Button  OR  Feed Sensor Activates
  if (FeedLoop >= 1){
    FeedCheck = analogRead(HangerRackFull);
    if (FeedCheck < 10){
      Error = 1;
      digitalWrite(ErrorLED, HIGH);
      return;
      }
      unsigned long currentTimer1 = millis();
      if (currentTimer1 - previousTimer1 >= Timer1INV){
        //previousTimer1 = currentTimer1;
        digitalWrite(FeedTable, HIGH);
        FeedNext = 1;
        FeedLoop = 0;
      }
  }
  if (FeedNext == 1){
      unsigned long currentTimer1 = millis();
       if (currentTimer1 - previousTimer1 >= Timer1INV2){
        previousTimer1 = currentTimer1;
        digitalWrite(FeedTable, LOW);
        FeedNext = 0;  //End the Feed until called again
      }
  }
  HookLoop = analogRead(HookCycleStart);
  if (HookLoop >= 10){
    HookCheck = analogRead(HookRailEmpty);
    if (HookCheck <= 10){
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
    if (currentTimer2 - previousTimer2 >= Timer2INV){
      //previousTimer2 - currentTimer2;
    digitalWrite(ToolHead, HIGH);
    HookNext = 2;
    }
  }
  if (HookNext == 2){
    int HeadCheckDown = 0;
    HeadCheckDown = analogRead(HeadDown);
    if (HeadCheckDown < 400){
      return;
    }
    /*int HeadCheckDown(){
      if (analogRead(HeadDown) < 400){
        return;
      }
    }*/
    //wait for head to register down position
    digitalWrite(StripOff, HIGH);
  }
  if (HookNext == 3){
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
    int HeadUpCheck = 0;
    HeadUpCheck = analogRead(HeadUp);
    if (HeadUpCheck < 400){
      return;
    }
    /*
    int HeadUpCheck(){
      if (analogRead(HeadUp) < 400){
        return;
      }
    }*/
    //wait for head to register up position
    digitalWrite(StripOff, LOW);
    digitalWrite(HookStopper, LOW);
    HookNext = 0;
  }
  CrimpLoop = analogRead(CrimpCycleStart);
  if (CrimpLoop >= 10){
    digitalWrite(CrimpStopper, HIGH);
    unsigned long currentTimer4 = millis();
    if (currentTimer4 - previousTimer4 >= Timer4INV){
      previousTimer4 - currentTimer4;
      digitalWrite(Crimp, HIGH);
      CrimpNext = 1;
      CrimpLoop = 0;
    }
  }
  if (CrimpNext == 1){
    unsigned long currentTimer5 = millis();
    if (currentTimer5 - previousTimer5 >= Timer5INV){
      previousTimer5 - currentTimer5;
      digitalWrite(Crimp, LOW);
      digitalWrite(CrimpStopper, LOW);
      CrimpLoop = 0;
      CrimpNext = 0;
    }
  }
}

