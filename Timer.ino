const int NextButton = 10;
//int SelButton = 32;
const int UpButton = 6;
const int DownButton = 7;
const int ToggleButton = 40;
const int LED1 = 11;
const int LED2 = 12;
const int LED3 = 9;


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
int BNextLogic = 0;
//int BSelLogic = 0;
int BUpLogic = 0;
int BDownLogic = 0;
int ToggleLogic = 0;
int x = 0;
int a = 1;

unsigned long previousTimer = 0;  //Previous Time for MAIN TIMER
unsigned int y[] = {1000, 2000, 3000};  // Time ARRAY for MAIN TIMER

unsigned long buttonPreviousTime = 0;  //Previous Time for Button Timer
int buttonWait = 300;  //Button wait Variable
int buttonCurrentTime = 0;

TimeKeeper ins(1000, 2300, 1100);

void setup() {
  pinMode(NextButton, INPUT);
//  pinMode(SelButton, INPUT);
  pinMode(UpButton, INPUT);
  pinMode(DownButton, INPUT);
  pinMode(ToggleLogic, INPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  Serial.begin(9600);
  Serial.println("Starting...");
}

void loop() {
  ins.Update();
  buttonCurrentTime = millis();
  BNextLogic = digitalRead(NextButton);
  if ((BNextLogic == HIGH) && (buttonCurrentTime - buttonPreviousTime >= buttonWait)){
    buttonPreviousTime = buttonCurrentTime;
    x++;
    if (x >= 3){
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
  int BUpLogic = 0;
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
  else {
    ToggleLogic = digitalRead(ToggleButton);
    if (ToggleLogic == HIGH){  
      unsigned long Timer1 = millis();
      if (a == 1){
      if (Timer1 - previousTimer >= y[0]){
        previousTimer = Timer1;
        Serial.print("SEQ 1 | Running at: ");
        Serial.print(y[0]);
        Serial.print("  |  Time: ");
        Serial.println(Timer1);
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
        a = 1;
      }
      }
    }
    if (x == 0){
      digitalWrite(LED1, HIGH);
      digitalWrite(LED2, LOW);
      digitalWrite(LED3, LOW);
    }
    if (x == 1){
      digitalWrite(LED2, HIGH);
      digitalWrite(LED1, LOW);
      digitalWrite(LED3, LOW);
    }
    if (x == 2){
      digitalWrite(LED3, HIGH);
      digitalWrite(LED2, LOW);
      digitalWrite(LED1, LOW);
    }
    
  }
}
