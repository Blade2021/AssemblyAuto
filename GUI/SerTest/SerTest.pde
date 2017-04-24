import processing.serial.*;
import controlP5.*;

Serial myPort;  // Create object from Serial class
String inByte;
String s;
ControlP5 cp5;
String temp;
String endchar = "\n";
Textarea consoletext;
Println console;
//Variables
boolean firstContact = false;
boolean globalState [] = {false, false, false, false, false, false, false, false};
int pin [] = {22, 24, 26, 28, 30, 32, 34, 36};

void setup() 
{  
  size(800,500);
  cp5 = new ControlP5(this);
  cp5.enableShortcuts();
  
  cp5.addTextfield("console")
    .setSize(360,35)
    .setCaptionLabel("Console")
    .setPosition(20,430)
    .setFont(createFont("arial",16))
    .setAutoClear(true)
    ;
    
  cp5.addTextfield("Timer1")
    .setId(1)
    .setPosition(20,70)
    .setSize(200,35)
    .setFont(createFont("arial",12))
    .setAutoClear(true)
    .setCaptionLabel("Timer 1")
    ;
  cp5.addTextfield("Timer2")
    .setId(2)
    .setPosition(20,130)
    .setSize(200,35)
    .setFont(createFont("arial",12))
    .setAutoClear(true)
    .setCaptionLabel("Timer 2")
    ;
  cp5.addTextfield("Timer3")
    .setId(3)
    .setPosition(20,190)
    .setSize(200,35)
    .setFont(createFont("arial",12))
    .setAutoClear(true)
    .setCaptionLabel("Timer 3")
    ;
  cp5.addTextfield("Timer4")
    .setId(4)
    .setPosition(20,250)
    .setSize(200,35)
    .setFont(createFont("arial",12))
    .setAutoClear(true)
    .setCaptionLabel("Timer 4")
    ;
  cp5.addTextfield("Timer5")
    .setId(5)
    .setPosition(20,310)
    .setSize(200,35)
    .setFont(createFont("arial",12))
    .setAutoClear(true)
    .setCaptionLabel("Timer 5")
    ;
  cp5.addTextfield("Timer6")
    .setId(6)
    .setPosition(20,370)
    .setSize(200,35)
    .setFont(createFont("arial",12))
    .setAutoClear(true)
    .setCaptionLabel("Timer 6")
    ;
  cp5.addButton("button1")
    .setSize(60,20)
    .setCaptionLabel("LED On")
    .setPosition(235,70)
    ;
  cp5.addButton("button2")
    .setSize(60,20)
    .setCaptionLabel("LED Off")
    .setPosition(235,100)
    ;
  cp5.addButton("button3")
    .setSize(60,20)
    .setCaptionLabel("Relays On")
    .setPosition(235,130)
    ;
  cp5.addButton("button4")
    .setSize(60,20)
    .setCaptionLabel("Relays Off")
    .setPosition(235,160)
    ;
  cp5.addBang("Relay1")
     .setPosition(310,50)
     .setId(1)
     .setSize(60,30)
     .setCaptionLabel("relay1")
     //.setColorBackground(color(255, 12, 0, 255))
     //.setColorActive(color(35, 255, 53, 255))
     ;
     
  cp5.addBang("Relay2")
     .setPosition(310,90)
     .setSize(60,30)
     .setId(2)
     .setCaptionLabel("Relay 2")
     ;
       
  cp5.addBang("Relay3")
     .setPosition(310,130)
     .setSize(60,30)
     .setId(3)
     .setCaptionLabel("Relay 3")
     ;
       
  cp5.addBang("Relay4")
     .setPosition(310,170)
     .setSize(60,30)
     .setId(4)
     .setCaptionLabel("Relay 4")
     ;
       
  cp5.addBang("Relay5")
     .setPosition(310,210)
     .setSize(60,30)
     .setId(5)
     .setCaptionLabel("Relay 5")
     ;
  cp5.addBang("Relay6")
     .setPosition(310,250)
     .setSize(60,30)
     .setId(6)
     .setCaptionLabel("Relay 6")
     ;
  cp5.addBang("Relay7")
     .setPosition(310,290)
     .setSize(60,30)
     .setId(7)
     .setCaptionLabel("Relay 7")
     ;
  cp5.addBang("Relay8")
     .setPosition(310,330)
     .setSize(60,30)
     .setId(8)
     .setCaptionLabel("Relay 8")
     ;
consoletext = cp5.addTextarea("txt")
                  .setPosition(440,20)
                  .setSize(350, 465)
                  .setFont(createFont("", 12))
                  .setLineHeight(14)
                  .setColor(color(255, 255, 255, 255))
                  .setColorBackground(color(100, 100))
                  .setColorForeground(color(255, 100));
  ;
  cp5.addTextlabel("ctext")
    .setText("Console")
    .setPosition(436,3)
    .setColorValue(255)
    .setFont(createFont("Arial",14));
  cp5.addTextlabel("stitle")
    .setText("Machine Serial")
    .setPosition(10,10)
    .setColorValue(color(200, 17, 0, 255))
    .setFont(createFont("Arial",24));
  cp5.addTextlabel("subtitle")
    .setText("Control Panel")
    .setPosition(30,35)
    .setColorValue(200)
    .setFont(createFont("Arial",16));
    
  console = cp5.addConsole(consoletext);
  
  
  //change the 0 to a 1 or 2 etc. to match your port
  myPort = new Serial(this, "COM3", 9600);
  myPort.bufferUntil('\n');
  
}

void draw() {
  background(0);
  fill(255);
  for (int i=0;i<globalState.length;i++){
    if (globalState[i] == false){
      fill(color(200, 20, 0, 255));
      rect(385, 50+i*40, 30, 30);
    }
    else{
      fill(color(123, 255, 0, 255));
      rect(385, 50+i*40, 30, 30);
    }
  }
}
void controlEvent(ControlEvent test) {
  if(test.isAssignableFrom(Textfield.class)){
    if("console".equals(test.getName())){
      temp = test.getStringValue();
      if("Update".equals(temp)){
        println("Control success");
        myPort.write("44");
      }else if("LED On".equals(temp)){
        myPort.write("2");
        println("Sent: LED ON");
      }else if("LED Off".equals(temp)){
        myPort.write("1");
        println("Sent: LED OFF");
      }else if("Help".equals(temp)){
        println("Help:");
        println("************************************");
        println("/  Commands:");
        println("/  - EEPROM.x.y - Updates EEPROM address (x) with value (y)");
        println("/  - PIN.x.y - Updates PIN (x) with value of (y)"); 
        println("/    [ Value should be 0 or 1 ONLY!]");
        println("/  - Call.x - Calls current value of position (x) in array");
        println("/  - Varu.x.y - Updates the position (x) with value of (y) in array");
        println("/");
        println("/");
        println("/  Timer values should stay within value of 5100");
        println("/");
        println("********** END OF HELP **************");
      }else {
        myPort.write(temp.toUpperCase() + '\n');
        println("Sent: " +temp.toUpperCase());
      }
    }
    boolean StrTest = test.getName().startsWith("Timer");
    
    if(StrTest == true){
      println("Sending EEPROM Update to controller");
      println("Updating Timer: " +test.getId() +" to Value: " +test.getStringValue());
      Timefunc(test.getId()-1, test.getStringValue());
    }
  }
}
public void button1(){
  println("Sent: pin.10.1");
  myPort.write("PIN.10.1" +endchar);
}
public void button2(){
  println("Sent: pin.10.0");
  myPort.write("PIN.10.0" +endchar);
}
public void button3(){
  println("Sent: Relays On");
  myPort.write("PIN.22.0" +endchar);
  myPort.write("PIN.24.0" +endchar);
  myPort.write("PIN.26.0" +endchar);
  myPort.write("PIN.28.0" +endchar);
  myPort.write("PIN.30.0" +endchar);
  myPort.write("PIN.32.0" +endchar);
  myPort.write("PIN.34.0" +endchar);
  myPort.write("PIN.36.0" +endchar);
}
public void button4(){
  println("Sent: Relays Off");
  myPort.write("PIN.22.1" +endchar);
  myPort.write("PIN.24.1" +endchar);
  myPort.write("PIN.26.1" +endchar);
  myPort.write("PIN.28.1" +endchar);
  myPort.write("PIN.30.1" +endchar);
  myPort.write("PIN.32.1" +endchar);
  myPort.write("PIN.34.1" +endchar);
  myPort.write("PIN.36.1" +endchar);
}
public void Relay1(){
  globalState[0] = !globalState[0];
  RelayControl(pin[0], globalState[0]);
}
public void Relay2(){
  globalState[1] = !globalState[1];
  RelayControl(pin[1], globalState[1]);
}
public void Relay3(){
  globalState[2] = !globalState[2];
  RelayControl(pin[2], globalState[2]);
}
public void Relay4(){
  globalState[3] = !globalState[3];
  RelayControl(pin[3], globalState[3]);
}
public void Relay5(){
  globalState[4] = !globalState[4];
  RelayControl(pin[4], globalState[4]);
}
public void Relay6(){
  globalState[5] = !globalState[5];
  RelayControl(pin[5], globalState[5]);
}
public void Relay7(){
  globalState[6] = !globalState[6];
  RelayControl(pin[6], globalState[6]);
}
public void Relay8(){
  globalState[7] = !globalState[7];
  RelayControl(pin[7], globalState[7]);
}
public void RelayControl(int Id, boolean Flag){
  println("Triggered relay Control");
  byte status = 0;
  if(Flag == true){
    status = 1;
  } else {
    status = 0;
  }
  myPort.write("PIN." +Id +'.' +status +endchar);
}

public void Timefunc(int Id, String value){
  byte overboard = 0;
  int control = Integer.parseInt(value);
  if(control > 5100){
    control=5100; 
  }
  control = control/10;
  if(control >= 256){
    overboard = 1;
  }
  if(overboard == 0){
    myPort.write("EEPROM." +Id +'.' +control +endchar);
    Id++;
    myPort.write("EEPROM." +Id +'.' +"0" +endchar);
  }
  if(overboard == 1){
    control = control - 255;
    myPort.write("EEPROM." +Id +'.' +"255" +endchar);
    Id++;
    myPort.write("EEPROM." +Id +'.' +control +endchar);
  }
}

void serialEvent(Serial myPort) {
  inByte = myPort.readStringUntil('\n');
  if (inByte != null){
    inByte = trim(inByte);
    println("Recieved: " +inByte);
    if(inByte.contains("PIN.")){
      //String grave = inByte.substring(12, 14);
      char result = inByte.charAt(5);
      char state = inByte.charAt(6);
      boolean isOn;
      int gstatetrigger = Character.getNumericValue(result);
      if (state=='H'){
        isOn = true;
      } else {
        isOn = false;
      }
      gstatetrigger--;
      globalState[gstatetrigger] = isOn;
      delay(10);
    }
    if (firstContact == false) {
      if (inByte.equals("<Controller is ready>")) {
        myPort.clear();
        firstContact = true;
        //myPort.write("A");
        println("SYSTEM: Contact Made");
        delay(100);
        println("Control ready!");
      }
    }
  }
}