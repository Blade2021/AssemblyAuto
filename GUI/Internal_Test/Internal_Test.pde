import processing.serial.*;

Serial myPort;
String endchar = "\n";
String inByte;
boolean override = false;

void setup() {
  myPort = new Serial(this, "COM9", 19200);
  myPort.clear();
  delay(1000);
  
}

void draw(){
  background(0);
}

void serialEvent(Serial myPort) {
  inByte = myPort.readStringUntil('\n');
  if (inByte != null) {
    inByte = trim(inByte);
    println("RX: " +inByte);
    if (inByte.contains("SITREP")){
      sitrep();
    }
    if (inByte.contains("OVERRIDE")){
      override = !override;
      String temp;
      if (override == true){
        temp = "ON";
      } else {
        temp = "OFF";
      }
      myPort.write("Override " +temp +endchar);
    }
  }
}

void sitrep(){
  
for(byte k = 0; k<9; k++){
    String data = ("EMU." +k + ".200" +endchar);
    myPort.write(data);
    delay(40);
  }
  for(byte k = 0; k<8; k++){
    String data = ("SEN." +k +"." +(k+20) +endchar);
    myPort.write(data);
    delay(40);
  }
  for(byte k = 0; k<8; k++){
    String data = ("SOL." +k +"." +(k+30) +endchar);
    myPort.write(data);
    delay(60);
  }
  myPort.write("SITREP COMPLETE" +endchar);
}