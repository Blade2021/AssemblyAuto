#include <EEPROM.h>
const byte Relay1 = 22;
const byte Relay2 = 24;
const byte Relay3 = 26;
const byte Relay4 = 28;
const byte Relay5 = 30;
const byte Relay6 = 32;
const byte Relay7 = 34;
const byte Relay8 = 36;
boolean checkvar = true;

byte Active = 0;
boolean state [] = {LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW};
int pinArray[] = {22, 24, 26, 28, 30, 32, 34, 36};
const byte solenoidArray[8] = {22, 24, 26, 28, 30, 32, 34, 36};
int sensorArray[] = {38, 40, 42, 44, 46, 48, 50, 51};
const byte refractor = 10;
const byte numChars = 32;
char receivedChars[numChars];   // an array to store the received data
char pear[] = {0};
long reciever = 0;
unsigned long land[refractor];
boolean newData = false;
String apple = "";
unsigned long previousTime = 0;
long checkTime = 800;
int x;
int y;

void setup() {
    Serial.begin(9600);
    Serial.println("<Controller is ready>");
    pinMode(13, OUTPUT);
    pinMode(10, OUTPUT);
    pinMode(Relay1, OUTPUT);
    pinMode(Relay2, OUTPUT);
    pinMode(Relay3, OUTPUT);
    pinMode(Relay4, OUTPUT);
    pinMode(Relay5, OUTPUT);
    pinMode(Relay6, OUTPUT);
    pinMode(Relay7, OUTPUT);
    pinMode(Relay8, OUTPUT);
    for(byte set;set<8;set++){
      pinMode(sensorArray[set], INPUT);
      delay(10);
    }
    reloadArray();
}

void loop() {
  unsigned long currentTime = millis();
    recvWithEndMarker();
    showNewData();
    if((land[1] == 230) && (checkvar == true)){
      Active = 1;
      checkvar = false;
    }
    if((land[1] == 200) && (checkvar == true)){
      Active = 0;
      checkvar = false;
    }
    if((land[1] == 220) && (checkvar == true)){
      for(byte t = 0; t<8; t++){
        Serial.println("INCOMING:");
        Serial.println(state[t]);
      }
      checkvar = false;
    }
  if (Active == 1){
    if(currentTime - previousTime >= checkTime){
      previousTime = currentTime;
      static byte index = random(0,7);
      state[index] = !state[index];
      if (state[index] == LOW){
        x = LOW;
        y = 0;
      }
      else {
        x = HIGH;
        y = 1;
      }
      digitalWrite(solenoidArray[index], x);
      Serial.print("PIN.");
      Serial.print(solenoidArray[index]);
      Serial.print(".");
      Serial.print(y);
      Serial.println(" ");
      index++;
      if (index > 7){
        index = 0;
      }
    }
  }
}

void recvWithEndMarker() {
    static byte ndx = 0;
    char endMarker = '\n';
    char rc;
    
    while (Serial.available() > 0 && newData == false) {
        rc = Serial.read();

        if (rc != endMarker) {
            receivedChars[ndx] = rc;
            ndx++;
            apple = apple += rc;
            if (ndx >= numChars) {
                ndx = numChars - 1;
            }
        }
        else {
            receivedChars[ndx] = '\0'; // terminate the string
            ndx = 0;
            newData = true;
        }
    }
}

void showNewData() {
    if (newData == true) {
        Serial.println(receivedChars);
        //reciever = atoi(receivedChars);
        newData = false;
        if (apple.length() >= 5){
          if (apple.substring(0,6) == "EEPROM") {
            eepromUpdate();
          }
          if (apple.substring(0,3) == "PIN") {
            pinUpdate();
          }
          if (apple.substring(0,4) == "VARU") {
            variableUpdate();
          }
          if (apple.substring(0,4) == "CALL") {
            reCall();
          }
          if (apple.substring(0,6) == "SITREP") {
            for(byte t;t<8;t++){
              Serial.print("SEN.");
              Serial.print(t);
              Serial.print(".");
              Serial.println(sensorArray[t]);
              delay(1);
              Serial.print("SOL.");
              Serial.print(t);
              Serial.print(".");
              Serial.println(solenoidArray[t]);
              if(t<6){
                int mem = EEPROM.read(t);
                Serial.print("EMU.");
                Serial.print(t);
                Serial.print(".");
                Serial.println(mem);
              }
            }
          }
        }
        apple = "";
    }
}

void eepromUpdate() {
  int tree = 0;
  char grape[numChars] = {0};
  int u = 0;
    Serial.println("Processing EEPROM Update....");
    for (int y = 7; y <= apple.length(); y++){
      if(receivedChars[y] != '.'){
        int z = y - 7;
        pear[z] = receivedChars[y];
      }
      else{
        int z = y - 7;
        pear[z] = '\0';
        u = y + 1;
        break;
      }
      delay(10);
    }
    reciever = atoi(pear);
    if (reciever >= EEPROM.length()){
      reciever = 0;
      Serial.println("EEPROM LIMIT HIT!");
    }
    for(u; u <= apple.length(); u++) {
      grape[tree] = receivedChars[u];
      tree++;
    }
    int value = atoi(grape);
    if (value >= 256){
      value = 255;
    }
    Serial.print("EEPROM.update(");
    Serial.print(reciever);
    Serial.print(", ");
    Serial.print(value);
    Serial.println(")");
    EEPROM.update(reciever,value);
    reloadArray();
}

void pinUpdate() {
  boolean value = LOW;
  int u = 0;
    //Serial.println("Processing PIN Update....");
    for (int y = 4; y <= apple.length(); y++){
      if(receivedChars[y] != '.'){
        int z = y - 4;
        pear[z] = receivedChars[y];
        delay(1);
      }
      else{
        int z = y - 4;
        pear[z] = '\0';
        delay(10);
        u = y;
        break;
      }
      delay(10);
    }
    reciever = atoi(pear);
    if (reciever >= 64){
      reciever = 64;
    }
    u++; //Now equals 1 + last postion
    if (receivedChars[u] == '0'){
      value = LOW;
    } else {
      value = HIGH;
    }
    for(byte checkmate = 0; checkmate < 8; checkmate++){
      if (reciever == solenoidArray[checkmate]){
        state[checkmate] = !state[checkmate];
      }
    }
    Serial.print("Updated PIN [");
    Serial.print(reciever);
    Serial.print("] Value: ");
    Serial.println(value);
    digitalWrite(reciever,value);
}

void reCall(){
  byte z;
  byte u;
  byte all = 0;
  byte pos = apple.lastIndexOf('A');
  if (pos == 5){
    all = 1;
  } else {
    for (int y = 5; y <= apple.length(); y++){
      if (receivedChars[y] != '.'){
        z = y - 5;
        if (receivedChars[z]=='A'){
        } else {
          pear[z] = receivedChars[y];
        }
      } else {
        z = y - 5;
        pear[z] = '\0';
        u = y + 1;
      }
      delay(10);
    }
  }
  if (all == 0){
    reciever = atoi(pear);
    Serial.print("ARRAY: LoC: ");
    Serial.print(reciever);
    Serial.print(" Value: ");
    Serial.println(state[reciever]);
  } else {
    for (byte k=0;k<8;k++){
      Serial.print("ARRAY: LoC: ");
      Serial.print(k);
      Serial.print(" Value: ");
      Serial.println(state[k]);
      delay(50);
    }
  }
}

void variableUpdate(){
  checkvar = true;
  byte u = 0;
  byte z = 0;
  char grape[numChars];
  for (byte y = 5; y <= apple.length(); y++){
    if (receivedChars[y] != '.'){
      z = y - 5;
      pear[z] = receivedChars[y];
    } else {
      z = y - 5;
      pear[z] = '\0';
      u = y + 1;
    }
  }
  reciever = atoi(pear);
  for (byte y = u; y <= apple.length(); y++){
    if (receivedChars[y] != '.'){
      z = y - u;
      grape[z] = receivedChars[y];
    } else {
      z = y - u;
      grape[z] = '\0';
      u = y + 1;
    }
  }
  int value2 = atoi(grape);
  Serial.print("Array Index: ");
  Serial.print(reciever);
  Serial.print("Value: ");
  Serial.println(value2);
  land[reciever] = value2;
}

void reloadArray(){
  for(int k = 0; k < refractor; k++){
    land[k] = EEPROM.read(k);
    delay(10);
    }
}

void SensorCheck(byte checkpin, byte relaypin){
  //boolean MainAir = HIGH;
  byte tempx;
  boolean currentstate;
  boolean nextstate;
  for(byte t=0; t<8; t++){
    if(pinArray[t] == relaypin){
      tempx = t;
      //assign tempx to value for next step
    }
  }
  boolean scair = HIGH;  //Remove this before final upload
  //boolean scair = digitalRead(MainAir);
  if (scair == HIGH){
    boolean currentstate = state[tempx];
    digitalWrite(relaypin, !state[tempx]);
    delay(1300);
    //boolean nextstate = digitalRead(checkpin)
    digitalWrite(relaypin, state[tempx]);
    if(currentstate != nextstate){
      Serial.print("Sensor: ");
      Serial.print(checkpin);
      Serial.println(" is working!");
    }
    else {
      Serial.print("Sensor: ");
      Serial.print(checkpin);
      Serial.println(" is NOT working!");
    }
  }
}


