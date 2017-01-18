int NextButton = 10;
//int SelButton = 32;
int UpButton = 6;
int DownButton = 7;
int ToggleButton = 40;
int LED1 = 11;
int LED2 = 12;
int LED3 = 9;

int BNextLogic = 0;
//int BSelLogic = 0;
int BUpLogic = 0;
int BDownLogic = 0;
int ToggleLogic = 0;
int x = 0;
int a = 1;

unsigned long previousTimer = 0;
unsigned long y[] = {1000, 2000, 3000};

void setup() {
//  pinMode(NextButton, INPUT);
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
  BNextLogic = digitalRead(NextButton);
  if (BNextLogic == HIGH){
    x++;
    if (x >= 3){
      x = 0;
      Serial.print("Time VAR: ");
      Serial.print(x+1);
      Serial.println(" selected");
      delay(200);
    }
    else {
    Serial.print("Time VAR: ");
    Serial.print(x+1);
    Serial.println(" selected");
    delay(200);
    }
  }
  BUpLogic = digitalRead(UpButton);
  if (BUpLogic == HIGH){
    y[x] = y[x] + 100;
    Serial.print("TimeVar ");
    Serial.print(x+1);
    Serial.print(" is now: ");
    Serial.println(y[x]);
    delay(100);
  }
  BDownLogic = digitalRead(DownButton);
  if (BDownLogic == HIGH){
    y[x] = y[x] - 100;
    Serial.print("TimeVar ");
    Serial.print(x+1);
    Serial.print(" is now: ");
    Serial.println(y[x]);
    delay(100);
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
