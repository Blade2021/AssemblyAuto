
const byte numChars = 32;
char receivedChars[numChars];   // an array to store the received data
long x;
boolean newData = false;

void setup() {
    Serial.begin(9600);
    Serial.println("<Arduino is ready>");
}

void loop() {
    recvWithEndMarker();
    showNewData();
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
    }
}
