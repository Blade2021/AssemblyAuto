const int NextButton = 30;
const int SaveButton = 38;
const int UpButton = 28;
const int DownButton = 26;
const int ToggleButton = 32;
const int Led1 = 11;
const int Led2 = 10;
const int Led3 = 9;
const int Led4 = 8;
const int Led5 = 7;
const int ErrorLed = 12;

/*
class TimeKeeper
{
  unsigned long previousTime;
  long TimeVar1;
  long TimeVar2;
  long TimeVar3;
  int a = 1;
  
  public:
  TimeKeeper(long TimeDelay1, long TimeDelay2, long TimeDelay3)
  {
    TimeVar1 = TimeDelay1;
    TimeVar2 = TimeDelay2;
    TimeVar3 = TimeDelay3;
    previousTime = 0;
    a = 1;
  }

  void Update()
  {
    unsigned long currentTime = millis();
    if ((a==1) && (currentTime - previousTime >= TimeVar1)){
      previousTime = currentTime;
      Serial.println("SEQ1.1 Activated");
      a = 2;
    }
    if ((a==2) && (currentTime - previousTime >= TimeVar2)){
      previousTime = currentTime;
      Serial.println("SEQ2.1 Activated");
      a = 3;
    }
    if ((a==3) && (currentTime - previousTime >= TimeVar3)){
      previousTime = currentTime;
      Serial.println("SEQ3.1 Activated");
      a = 1;
    }
  }
};
*/
int BNextLogic = 0;
int BUpLogic = 0;
int BDownLogic = 0;
int ToggleLogic = 0;
int SaveButtonTrigger = 0;
int x = 0;
int a = 1;

unsigned long previousTimer = 0;  //Previous Time for MAIN TIMER
unsigned int y[] = {100, 100, 100, 100, 100};  // Time ARRAY for MAIN TIMER

unsigned long buttonPreviousTime = 0;  //Previous Time for Button Timer
unsigned long previousTimer2 = 0;
int buttonWait = 200;  //Button wait Variable


void setup() {
  pinMode(NextButton, INPUT);
  pinMode(SaveButton, INPUT);
  pinMode(UpButton, INPUT);
  pinMode(DownButton, INPUT);
  pinMode(ToggleLogic, INPUT);
  pinMode(Led1, OUTPUT);
  pinMode(Led2, OUTPUT);
  pinMode(Led3, OUTPUT);
  pinMode(Led4, OUTPUT);
  pinMode(Led5, OUTPUT);
  pinMode(ErrorLed, OUTPUT);
  Serial.begin(9600);
  Serial.println("Starting...");
  attachInterrupt(digitalPinToInterrupt(38), savetrigger, CHANGE);
}

void loop() {
//  ins.Update();
  unsigned long buttonCurrentTime = millis();
  BNextLogic = digitalRead(NextButton);
  if ((BNextLogic == HIGH) && (buttonCurrentTime - buttonPreviousTime >= buttonWait)){
    buttonPreviousTime = buttonCurrentTime;
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
  if ((BUpLogic == HIGH) && (buttonCurrentTime - buttonPreviousTime >= buttonWait)){
    y[x] = y[x]+100;
    buttonPreviousTime = buttonCurrentTime;
    Serial.print("TimeVar ");
    Serial.print(x+1);
    Serial.print(" is now: ");
    Serial.println(y[x]);
  }
  BDownLogic = digitalRead(DownButton);
  if ((BDownLogic == HIGH) && (buttonCurrentTime - buttonPreviousTime >= buttonWait)){
    y[x] = y[x]-100;
    buttonPreviousTime = buttonCurrentTime;
    Serial.print("TimeVar ");
    Serial.print(x+1);
    Serial.print(" is now: ");
    Serial.println(y[x]);
  }
  SaveButtonTrigger = digitalRead(SaveButton);
  if ((SaveButtonTrigger) && (buttonCurrentTime - buttonPreviousTime >= buttonWait)){
    buttonPreviousTime = buttonCurrentTime;
    Serial.print("TimeVar ");
    Serial.print(x+1);
    Serial.println(" saved.");
    savetrigger();
  }
  else {
    ToggleLogic = digitalRead(ToggleButton);
    if (ToggleLogic == HIGH){
      digitalWrite(ErrorLed, LOW);
      unsigned long Timer1 = millis();
      if (a == 1){
      if (Timer1 - previousTimer >= y[0]){
        previousTimer = Timer1;
        Serial.print("SEQ 1 | Running at: ");
        Serial.print(y[0]);
        Serial.print("  |  Time: ");
        Serial.println(Timer1);
        digitalWrite(Led1, HIGH);
        digitalWrite(Led2, LOW);
        digitalWrite(Led3, LOW);
        digitalWrite(Led4, LOW);
        digitalWrite(Led5, LOW);
        a = 2;
      }
      }
      if (a == 2){
      if (Timer1 - previousTimer >= y[1]){
        previousTimer = Timer1;
        Serial.print("SEQ 2 | Running at: ");
        Serial.print(y[1]);
        Serial.print("  |  Time: ");
        Serial.println(Timer1);
        digitalWrite(Led2, HIGH);
        digitalWrite(Led1, LOW);
        digitalWrite(Led3, LOW);
        digitalWrite(Led4, LOW);
        digitalWrite(Led5, LOW);
        a = 3;
      }
      }
      if (a == 3){
      if (Timer1 - previousTimer >= y[2]){
        previousTimer = Timer1;
        Serial.print("SEQ 3 | Running at: ");
        Serial.print(y[2]);
        Serial.print("  |  Time: ");
        Serial.println(Timer1);
        a = 4;
        digitalWrite(Led3, HIGH);
        digitalWrite(Led2, LOW);
        digitalWrite(Led1, LOW);
        digitalWrite(Led4, LOW);
        digitalWrite(Led5, LOW);
      }
      }
      if (a == 4){
      if (Timer1 - previousTimer >= y[3]){
        previousTimer = Timer1;
        Serial.print("SEQ 4 | Running at: ");
        Serial.print(y[3]);
        Serial.print("  |  Time: ");
        Serial.println(Timer1);
        a = 5;
        digitalWrite(Led4, HIGH);
        digitalWrite(Led2, LOW);
        digitalWrite(Led3, LOW);
        digitalWrite(Led1, LOW);
        digitalWrite(Led5, LOW);
      }
      }
      if (a == 5){
      if (Timer1 - previousTimer >= y[4]){
        previousTimer = Timer1;
        Serial.print("SEQ 5 | Running at: ");
        Serial.print(y[4]);
        Serial.print("  |  Time: ");
        Serial.println(Timer1);
        a = 1;
        digitalWrite(Led5, HIGH);
        digitalWrite(Led2, LOW);
        digitalWrite(Led3, LOW);
        digitalWrite(Led4, LOW);
        digitalWrite(Led1, LOW);
        }
        } 
      }
      else {
        digitalWrite(ErrorLed, HIGH);
        switch (x){
          case 0:
            digitalWrite(Led1, HIGH);
            digitalWrite(Led2, LOW);
            digitalWrite(Led3, LOW);
            digitalWrite(Led4, LOW);
            digitalWrite(Led5, LOW);
            break;
          case 1:
            digitalWrite(Led2, HIGH);
            digitalWrite(Led1, LOW);
            digitalWrite(Led3, LOW);
            digitalWrite(Led4, LOW);
            digitalWrite(Led5, LOW);
            break;
          case 2:
            digitalWrite(Led3, HIGH);
            digitalWrite(Led2, LOW);
            digitalWrite(Led1, LOW);
            digitalWrite(Led4, LOW);
            digitalWrite(Led5, LOW);
            break;
          case 3:
            digitalWrite(Led4, HIGH);
            digitalWrite(Led2, LOW);
            digitalWrite(Led3, LOW);
            digitalWrite(Led1, LOW);
            digitalWrite(Led5, LOW);
            break;
          case 4:
            digitalWrite(Led5, HIGH);
            digitalWrite(Led2, LOW);
            digitalWrite(Led3, LOW);
            digitalWrite(Led4, LOW);
            digitalWrite(Led1, LOW);
            break;
        }
      }
   }
}
void savetrigger(){
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

