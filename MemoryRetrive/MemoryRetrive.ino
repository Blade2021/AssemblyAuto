#include <EEPROM.h>

#define DATASPEED 19200

const byte numChars = 32; // Array character limit
char receivedChars[numChars]; // Recieved bytes from serial input
String apple = ""; // Incoming serial data string
boolean newData = false;
byte debug = 0;

void setup(){
    Serial.begin(DATASPEED);
    Serial.println("Serial read program v1.0");

}

void loop(){
    if (Serial.available() > 0)
    {
        recvWithEndMarker();
    }
    if (newData == true)
    {
        checkData();
    }
}

void recvWithEndMarker()
{
  static byte ndx = 0;
  char endMarker = '\n';
  char rc;

  while (Serial.available() > 0 && newData == false)
  {
    rc = Serial.read();

    if (rc != endMarker)
    {
      receivedChars[ndx] = rc;
      ndx++;
      apple = apple += rc;
      if (ndx >= numChars)
      {
        ndx = numChars - 1;
      }
    }
    else
    {
      receivedChars[ndx] = '\0'; // terminate the string
      ndx = 0;
      Serial.println(receivedChars);
      newData = true;
    }
  }
}


void checkData()
{
  if (newData == true)
  {
    if (apple.length() >= 5)
    {
      if (apple.substring(0, 4) == "READ")
      {
        Serial.println("debug 3242");
        readFunction();
      }
      if (apple.substring(0, 7) == "READALL")
      {
        Serial.println("debug 4567");
        readAllFunction();
      }
    }
    newData = false;
    apple = "";
  }
}

void readFunction(){
    int startLocation = firstValue();
    int endLocation = (lastValue()+1);
    for(int indx = startLocation; indx < endLocation; indx++){
        Serial.print("Location: ");
        Serial.print(indx);
        Serial.print(" Data: ");
        Serial.println(EEPROM.read(indx));
        delay(10);
    }
}

void readAllFunction(){
    for(int indx = 0; indx < EEPROM.length(); indx++){
        Serial.print("Location: ");
        Serial.print(indx);
        Serial.print(" Data: ");
        Serial.println(EEPROM.read(indx));
        delay(10);
    }
}

int firstValue(){
  char masterArray[numChars];
  byte slaveindx;
  byte value_start = apple.indexOf('.');
  if(debug >= 2){
  Serial.print("V Start: ");
  Serial.println(value_start);
  }
  byte value_end = apple.indexOf('.', value_start+1);
  if(debug >= 2){
  Serial.print("V End: ");
  Serial.println(value_end);
  }
  for (byte k = value_start+1; k < value_end; k++){
    masterArray[slaveindx] = receivedChars[k];
    slaveindx++;
  }
  masterArray[slaveindx] = '\0';
  Serial.println(masterArray);
  int value = atoi(masterArray);
  if(debug >= 2){
  Serial.print("fvF firstValue: ");
  Serial.println(value);
  }
  return value;
}

int lastValue(){
  char masterArray[numChars];
  byte slaveindx = 0;
  byte value_end = apple.lastIndexOf('.');
  if(debug >= 2){
  Serial.print("V End (2): ");
  Serial.println(value_end);
  }
  for (byte k = value_end+1; k < apple.length(); k++){
    masterArray[slaveindx] = receivedChars[k];
    slaveindx++;
  }
  masterArray[slaveindx] = '\0';
  int lastvalue = atoi(masterArray);
  if(debug >= 2){
    Serial.print("lvF lastValue: ");
    Serial.println(lastvalue);
  }
  return lastvalue;
}
