const byte numChars = 32;
char receivedChars[numChars];   // an array to store the received data
char pear[] = {0};
long reciever = 0;
int x;
boolean newData = false;
String apple = "";

void setup() {
    Serial.begin(9600);
    Serial.println("<Arduino is ready>");
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
            eepromApple();
          }
          if (apple.substring(0,3) == "pin") {
            pinApple();
          }
        }
        apple = "";
    }
}

void eepromApple() {
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
        u = y;
        break;
      }
      delay(10);
    }
    reciever = atoi(pear);
    
    if (reciever >= 256){
      reciever = 1;
    }
    u++;
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

void pinApple() {
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
    if (receivedChars[u] == '1'){
      value = HIGH;
    } else {
      value = LOW;
    }
    Serial.print("digitalWrite(");
    Serial.print(reciever);
    Serial.print(", ");
    Serial.print(value);
    Serial.println(")");
    digitalWrite(reciever,value);
}


