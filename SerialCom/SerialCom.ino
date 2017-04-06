
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
}

void loop() {
    recvWithEndMarker();
    showNewData();
    /*
    switch(x){
    case 1:
      Serial.println("Test success 1");
      x = 0;
      break;
    case 2:
      Serial.println("Test success 2");
      x = 0;
      break;
    default:
      break;
    }*/
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
    x = atoi(pear);
    if (x >= 256){
      x = 1;
    }
    u++;
    for(u; u <= apple.length(); u++) {
      grape[tree] = receivedChars[u];
      tree++;
    }
    int value = atoi(grape);
    value = atoi(grape);
    if (value >= 256){
      value = 255;
    }
    Serial.print("EEPROM.update(");
    Serial.print(x);
    Serial.print(", ");
    Serial.print(value);
    Serial.println(")");
}

void pinApple() {
  boolean value = LOW;
  char grape[numChars] = {0};
  int u = 0;
    Serial.println("Processing EEPROM Update....");
    for (int y = 4; y <= apple.length(); y++){
      if(receivedChars[y] != '.'){
        int z = y - 4;
        pear[z] = receivedChars[y];
      }
      else{
        int z = y - 7;
        pear[z] = '\0';
        u = y;
        break;
      }
      delay(10);
    }
    x = atoi(pear);
    if (x >= 64){
      x = 64;
    }
    u++;
    if (receivedChars[u] == 1){
      value = HIGH;
    } else {
      value = LOW;
    }
    Serial.print("digitalWrite(");
    Serial.print(x);
    Serial.print(", ");
    Serial.print(value);
    Serial.println(")");
}


