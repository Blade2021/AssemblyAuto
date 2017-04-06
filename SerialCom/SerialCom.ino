
const byte numChars = 32;
char receivedChars[numChars];   // an array to store the received data
char pear[] = {0};
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
        x = atoi(receivedChars);
        newData = false;
        if (apple.length() >= 5){
          checkApple();
        }
        apple = "";
    }
}

void checkApple() {
  int tree = 0;
  char grape[numChars] = {0};
  int u = 0;
  if (apple.substring(0,6) == "EEPROM") {
    Serial.print("Apple before trim: ");
    Serial.println(apple);
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
    Serial.print("X = ");
    Serial.println(x);
    u++;
    for(u; u <= apple.length(); u++) {
      grape[tree] = receivedChars[u];
      tree++;
    }
    int value = atoi(grape);
    value = atoi(grape);
    Serial.print("Final Value = ");
    Serial.println(value);
  }
}


