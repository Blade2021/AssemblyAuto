
#include <Key.h>
#include <Keypad.h>
#include <LiquidCrystal.h>


const int NextButton = 26;
const int SaveButton = 30;
const int UpButton = 32;
const int DownButton = 28;
const int ToggleButton = 40;
const int Led1 = 46;
const int Led2 = 44;
const int Led3 = 42; //42
const int Led4 = 8; //8
const int Led5 = 7; //7
const int Sensor = 9;
const int ErrorLed = 38;

const int Relay1 = 15;
const int Relay2 = 16;
const int Relay3 = 17;
const int Relay4 = 18;
const int Relay5 = 19;

int BNextLogic = 0;
int BUpLogic = 0;
int BDownLogic = 0;
int ToggleLogic = 0;
int SaveButtonTrigger = 0;
int x = 0;
int a = 1;
int j = 0;
char arraya [] = {0, 1, 2, 3, 0};



unsigned long previousTimer = 0;  //Previous Time for MAIN TIMER
unsigned int y[] = {600, 600, 600, 600, 600};  // Time ARRAY for MAIN TIMER

unsigned long buttonPreviousTime = 0;  //Previous Time for Button Timer
unsigned long previousTimer2 = 0;
int buttonWait = 200;  //Button wait Variable
int TestCheck = 0;
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
int pos=13;
boolean SenCheck = LOW;

void setup() {
  pinMode(NextButton, INPUT);
  pinMode(SaveButton, INPUT);
  pinMode(UpButton, INPUT);
  pinMode(DownButton, INPUT);
  pinMode(ToggleLogic, INPUT);
  pinMode(Sensor, INPUT_PULLUP);
  pinMode(Led1, OUTPUT);
  pinMode(Led2, OUTPUT);
  pinMode(Led3, OUTPUT);
  pinMode(Led4, OUTPUT);
  pinMode(Led5, OUTPUT);
  pinMode(Relay1, OUTPUT);
  pinMode(Relay2, OUTPUT);
  pinMode(Relay3, OUTPUT);
  pinMode(Relay4, OUTPUT);
  pinMode(Relay5, OUTPUT);
  pinMode(ErrorLed, OUTPUT);
  Serial.begin(9600);
  Serial.println("Starting...");
  attachInterrupt(digitalPinToInterrupt(38), savetrigger, CHANGE);
  lcd.begin(20,4);
  lcd.setCursor(0,0);
  lcd.print("Run Time: ");
  lcd.setCursor(2,1);
  lcd.print("Status: ");
}

void loop() {
//  ins.Update();
  lcd.setCursor(11,0);
  lcd.print(millis() / 1000);
  /*SenCheck = digitalRead(Sensor);
  if (SenCheck == HIGH){
    digitalWrite(Led3, HIGH);
  }
  if (SenCheck == LOW){
    digitalWrite(Led3, LOW);
  }*/
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
    ToggleLogic = digitalRead(Sensor);
    if (ToggleLogic == HIGH){
      digitalWrite(ErrorLed, LOW);
      lcd.setCursor(10,1);
      lcd.print("Active    ");
      unsigned long Timer1 = millis();
      if (a == 1){
      if (Timer1 - previousTimer >= y[0]){
        previousTimer = Timer1;
        lcd.setCursor(0,2);
        lcd.print("Feed Table Opened   ");
        lcd.setCursor(0,3);
        lcd.print(y[0]);
        lcd.print("     ");
        Serial.print("SEQ 1 | Running at: ");
        Serial.print(y[0]);
        Serial.print("  |  Time: ");
        Serial.println(Timer1);
        digitalWrite(Relay1, HIGH);
        digitalWrite(Relay2, LOW);
        digitalWrite(Relay3, LOW);
        digitalWrite(Relay4, LOW);
        digitalWrite(Relay5, LOW);
        setLED(Led1);
        a = 2;
      }
      }
      if (a == 2){
      if (Timer1 - previousTimer >= y[1]){
        previousTimer = Timer1;
        lcd.setCursor(0,2);
        lcd.print("Feed Table Closed   ");
        lcd.setCursor(0,3);
        lcd.print(y[1]);
        lcd.print("     ");
        Serial.print("SEQ 2 | Running at: ");
        Serial.print(y[1]);
        Serial.print("  |  Time: ");
        Serial.println(Timer1);
        digitalWrite(Relay2, HIGH);
        digitalWrite(Relay1, LOW);
        digitalWrite(Relay3, LOW);
        digitalWrite(Relay4, LOW);
        digitalWrite(Relay5, LOW);
        setLED(Led2);
        a = 3;
      }
      }
      if (a == 3){
      if (Timer1 - previousTimer >= y[2]){
        previousTimer = Timer1;
        lcd.setCursor(0,2);
        lcd.print("Hook Head Wait      ");
        lcd.setCursor(0,3);
        lcd.print(y[2]);
        lcd.print("     ");
        Serial.print("SEQ 3 | Running at: ");
        Serial.print(y[2]);
        Serial.print("  |  Time: ");
        Serial.println(Timer1);
        a = 4;
        digitalWrite(Relay3, HIGH);
        digitalWrite(Relay2, LOW);
        digitalWrite(Relay1, LOW);
        digitalWrite(Relay4, LOW);
        digitalWrite(Relay5, LOW);
        setLED(Led3);
      }
      }
      if (a == 4){
      if (Timer1 - previousTimer >= y[3]){
        previousTimer = Timer1;
        lcd.setCursor(0,2);
        lcd.print("Crimp Wait          ");
        lcd.setCursor(0,3);
        lcd.print(y[3]);
        lcd.print("     ");
        Serial.print("SEQ 4 | Running at: ");
        Serial.print(y[3]);
        Serial.print("  |  Time: ");
        Serial.println(Timer1);
        a = 5;
        digitalWrite(Relay4, HIGH);
        digitalWrite(Relay2, LOW);
        digitalWrite(Relay3, LOW);
        digitalWrite(Relay1, LOW);
        digitalWrite(Relay5, LOW);
        setLED(Led4);
      }
      }
      if (a == 5){
      if (Timer1 - previousTimer >= y[4]){
        previousTimer = Timer1;
        lcd.setCursor(0,2);
        lcd.print("Crimp Time Activated");
        lcd.setCursor(0,3);
        lcd.print(y[4]);
        lcd.print("     ");
        Serial.print("SEQ 5 | Running at: ");
        Serial.print(y[4]);
        Serial.print("  |  Time: ");
        Serial.println(Timer1);
        a = 1;
        digitalWrite(Relay5, HIGH);
        digitalWrite(Relay2, LOW);
        digitalWrite(Relay3, LOW);
        digitalWrite(Relay4, LOW);
        digitalWrite(Relay1, LOW);
        setLED(Led5);
        }
        } 
      }
      else {
        lcd.setCursor(10,1);
        lcd.print("Inactive  ");
        digitalWrite(ErrorLed, HIGH);
        char input;
        switch (x){
          case 0:
            setLED(Led1);
            lcd.setCursor(0,2);
            lcd.print("Feed Wait Time:     ");
            lcd.setCursor(0,3);
            lcd.print(y[x]);
            lcd.print("     ");
            lcd.setCursor(pos,3);
            changetime(x);
            break;
          case 1:
            setLED(Led2);
            lcd.setCursor(0,2);
            lcd.print("Feed Open Time      ");
            lcd.setCursor(0,3);
            lcd.print(y[x]);
            lcd.print("     ");
            lcd.setCursor(pos,3);
            changetime(x);
            break;
          case 2:
            setLED(Led3);
            lcd.setCursor(0,2);
            lcd.print("Hook Cycle Wait     ");
            lcd.setCursor(0,3);
            lcd.print(y[x]);
            lcd.print("     ");
            lcd.setCursor(pos,3);
            changetime(x);
            break;
          case 3:
            setLED(Led4);
            lcd.setCursor(0,2);
            lcd.print("Crimp Cycle Wait    ");
            lcd.setCursor(0,3);
            lcd.print(y[x]);
            lcd.print("     ");
            lcd.setCursor(pos,3);
            changetime(x);
            break;
          case 4:
            setLED(Led5);
            lcd.setCursor(0,2);
            lcd.print("Crimp Time          ");
            lcd.setCursor(0,3);
            lcd.print(y[x]);
            lcd.print("     ");
            lcd.setCursor(pos,3);
            changetime(x);
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
void changetime(int x){
  char key;
  key = keypad.getKey();
  if(key){
    lcd.print(key);
    pos++;
    lcd.setCursor(pos,3);
    arraya[j++] = key;
    arraya[j];
    if (pos > 20){
      pos = 13;
    }
    if(key=='*'){
      int tempa = atoi(arraya);
      Serial.println(tempa);
      y[x]=tempa;
      Serial.println("Ran array process function.");
      pos = 13;
      lcd.setCursor(pos,3);
      lcd.print("       ");
      j = 0;
      return;
    }
    if(key=='#'){
      pos = 13;
      lcd.setCursor(pos,3);
      lcd.print("       ");
      j = 0;
      return;
    }
  }
}
void setLED(byte LEDnumber)
{
  digitalWrite(Led1, LOW);
  digitalWrite(Led2, LOW);
  digitalWrite(Led3, LOW);
  digitalWrite(Led4, LOW);
  digitalWrite(Led5, LOW);
  digitalWrite(LEDnumber, HIGH);
}
