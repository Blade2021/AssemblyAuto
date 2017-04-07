const byte numChars = 32;
char receivedChars[numChars];   // an array to store the received data
char pear[] = {0};
long reciever = 0;
unsigned long land[] = {1000, 200, 10, 14};
boolean newData = false;
String apple = "";

void setup() {
    Serial.begin(9600);
    Serial.println("<Controller is ready>");
    pinMode(13, OUTPUT);
    pinMode(10, OUTPUT);
}

void loop() {
    recvWithEndMarker();
    showNewData();
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
        reciever = atoi(receivedChars);
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
        /*
        Serial.print(receivedChars[y]);
        Serial.print(" was assigned to pear: ");
        Serial.println(z);
        */
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
    
    if (reciever >= 256){
      reciever = 1;
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
}

void pinUpdate() {
  boolean value = LOW;
  int u = 0;
    Serial.println("Processing PIN Update....");
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
    Serial.print("Updated PIN [");
    Serial.print(reciever);
    Serial.print("] Value: ");
    Serial.println(value);
    digitalWrite(reciever,value);
}

void reCall(){
  int z;
  int u;
  for (int y = 5; y <= apple.length(); y++){
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
  Serial.print("ARRAY: LoC: ");
  Serial.print(reciever);
  Serial.print(" Value: ");
  Serial.println(land[reciever]);
}

void variableUpdate(){
  int u = 0;
  int z = 0;
  char grape[numChars];
  for (int y = 5; y <= apple.length(); y++){
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
  for (int y = u; y <= apple.length(); y++){
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
  Serial.print("Value 1: ");
  Serial.println(reciever);
  Serial.print("Value 2: ");
  Serial.println(value2);
  land[reciever] = value2;
}

