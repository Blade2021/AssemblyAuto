const byte sensorArray[] = {A0, A1, A2, A3, A4, A5, A6, A7};
const byte solenoidArray[] = {14, 15, 19, 18, 17, 16, 8, 7, 9};

void setup() {
  // put your setup code here, to run once:
  for(byte k;k < 8; k++){
    pinMode(sensorArray[k],INPUT);
    delay(10);
    pinMode(solenoidArray[k], OUTPUT);
    delay(10);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  for(byte x;x < 8; x++){
    boolean y = digitalRead(sensorArray[x]);
    if(y == HIGH){
      digitalWrite(solenoidArray[x], HIGH);
    }
    else {
      digitalWrite(solenoidArray[x], LOW);
    }
  }
}
