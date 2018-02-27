import processing.serial.*;
import controlP5.*;

Serial myPort;  //Create serial port
String inByte; //String for incoming data from serial port
String s;
ControlP5 cp5;  //Control P5 Structure
String temp;
String endchar = "\n";  //End char attached to each message
Button sbutton;
DropdownList d1;
boolean relayToggle;
Textarea consoletext;
Println console;

ControlTimer ech;

//Variables

int lastSenWait = 100;
boolean firstContact = false;
int connected = 0;
boolean globalState [] = {false, false, false, false, false, false, false, false};
int [] sensorState = {0, 0, 0, 0};
int pin [] = {22, 24, 26, 28, 30, 32, 34, 36};
int relayArray[] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
int sensorPin [] = {10, 20, 30, 40, 50, 60, 70, 80};
int tabID = 111;
byte timerLength = 8;
boolean varLoad = false;
boolean sensorEcho;
boolean toggleValue = false;
boolean sensorInitial = true;
boolean serialToggle = false;
String port;
boolean override = false; // Override Variable (Check System)
long lastTime = 0;
int waitTime = 100;

void keyValue(int value) {
  if ((relayToggle == true) && (varLoad == true)) {
    RelayControl(relayArray[value], !globalState[value]);
  }
}

void setup() 
{
  size(1200, 700);
  surface.setResizable(true);
  lastTime = millis();
  surface.setTitle("National Hanger - Machine Serial");
  noStroke();
  frameRate(60);
  ech = new ControlTimer();
  ech.setSpeedOfTime(1);
  cp5 = new ControlP5(this);
  cp5.enableShortcuts();

  cp5.addTextfield("console")
    .setSize(360, 35)
    .setCaptionLabel("Console Injection Module")
    .setPosition(20, 630)
    .setFont(createFont("arial", 14))
    .setAutoClear(true)

    ;

  cp5.addTextfield("timer0")
    .setId(1)
    .setPosition(20, 70)
    .setSize(200, 35)
    .setFont(createFont("arial", 16))
    .setAutoClear(false)
    .setCaptionLabel("EEPROM: timer 1")
    .hide()
    ;
  cp5.addTextfield("timer1")
    .setId(2)
    .setPosition(20, 130)
    .setSize(200, 35)
    .setFont(createFont("arial", 16))
    .setAutoClear(false)
    .setCaptionLabel("EEPROM: timer 2")
    .hide()
    ;
  cp5.addTextfield("timer2")
    .setId(3)
    .setPosition(20, 190)
    .setSize(200, 35)
    .setFont(createFont("arial", 16))
    .setAutoClear(false)
    .setCaptionLabel("EEPROM: timer 3")
    .hide()
    ;
  cp5.addTextfield("timer3")
    .setId(4)
    .setPosition(20, 250)
    .setSize(200, 35)
    .setFont(createFont("arial", 16))
    .setAutoClear(false)
    .setCaptionLabel("EEPROM: timer 4")
    .hide()
    ;
  cp5.addTextfield("timer4")
    .setId(5)
    .setPosition(20, 310)
    .setSize(200, 35)
    .setFont(createFont("arial", 16))
    .setAutoClear(false)
    .setCaptionLabel("EEPROM: timer 5")
    .hide()
    ;
  cp5.addTextfield("timer5")
    .setId(6)
    .setPosition(20, 370)
    .setSize(200, 35)
    .setFont(createFont("arial", 16))
    .setAutoClear(false)
    .setCaptionLabel("EEPROM: timer 6")
    .hide()
    ;
  cp5.addTextfield("timer6")
    .setId(7)
    .setPosition(20, 430)
    .setSize(200, 35)
    .setFont(createFont("arial", 16))
    .setAutoClear(false)
    .setCaptionLabel("EEPROM: timer 7")
    .hide()
    ;
  cp5.addTextfield("timer7")
    .setId(8)
    .setPosition(20, 490)
    .setSize(200, 35)
    .setFont(createFont("arial", 16))
    .setAutoClear(false)
    .setCaptionLabel("EEPROM: timer 8")
    .hide()
    ;
 cp5.addTextfield("timer8")
    .setId(8)
    .setPosition(20, 550)
    .setSize(200, 35)
    .setFont(createFont("arial", 16))
    .setAutoClear(false)
    .setCaptionLabel("EEPROM: timer 9")
    .hide()
    ;
  cp5.addButton("button1")
    .setSize(70, 20)
    .setCaptionLabel("LED On")
    .setPosition(235, 70)
    ;
  cp5.addButton("button2")
    .setSize(70, 20)
    .setCaptionLabel("LED Off")
    .setPosition(235, 100)
    ;
  cp5.addButton("button4")
    .setSize(70, 20)
    .setCaptionLabel("Relays On")
    .setPosition(235, 130)
    ;
  cp5.addButton("button3")
    .setSize(70, 20)
    .setCaptionLabel("Relays Off")
    .setPosition(235, 160)
    ;
  sbutton = cp5.addButton("sitrepbutton")
    .setSize(70, 40)
    .setCaptionLabel("Load")
    //.setColorLabel(color(0,255,0))
    .setPosition(235, 190)
    .setFont(createFont("arial", 16))
    .setColorBackground(color(200, 0, 0, 255))
    .setColorForeground(color(200, 0, 0, 180))
    ;
  cp5.addToggle("toggleValue")
    .setPosition(235, 250)
    .setSize(70, 40)
    .setCaptionLabel("Echo")
    ;
  cp5.addToggle("relayToggle")
    .setPosition(235, 310)
    .setSize(70, 40)
    .setCaptionLabel("Relay Key Toggle")
    ;
  cp5.addBang("sensorCheck")
    .setPosition(235, 370)
    .setSize(70, 40)
    .setCaptionLabel("Sensor Check")
    ;
  cp5.addToggle("sensorEcho")
    .setPosition(235, 440)
    .setSize(70, 40)
    .setCaptionLabel("Sensor Check Auto")
    ;
  cp5.addBang("Relay1")
    .setPosition(320, 50)
    .setId(31)
    .setSize(60, 30)
    .setCaptionLabel("Relay1")
    ;
  cp5.addBang("Relay2")
    .setPosition(320, 95)
    .setSize(60, 30)
    .setId(32)
    .setCaptionLabel("Relay 2")
    ;

  cp5.addBang("Relay3")
    .setPosition(320, 140)
    .setSize(60, 30)
    .setId(33)
    .setCaptionLabel("Relay 3")
    ;

  cp5.addBang("Relay4")
    .setPosition(320, 185)
    .setSize(60, 30)
    .setId(34)
    .setCaptionLabel("Relay 4")
    ;

  cp5.addBang("Relay5")
    .setPosition(320, 230)
    .setSize(60, 30)
    .setId(35)
    .setCaptionLabel("Relay 5")
    ;
  cp5.addBang("Relay6")
    .setPosition(320, 275)
    .setSize(60, 30)
    .setId(36)
    .setCaptionLabel("Relay 6")
    ;
  cp5.addBang("Relay7")
    .setPosition(320, 320)
    .setSize(60, 30)
    .setId(37)
    .setCaptionLabel("Relay 7")
    ;
  cp5.addBang("Relay8")
    .setPosition(320, 365)
    .setSize(60, 30)
    .setId(38)
    .setCaptionLabel("Relay 8")
    ;
  cp5.addBang("Relay9")
    .setPosition(320, 410)
    .setSize(60, 30)
    .setId(39)
    .setCaptionLabel("Relay 9")
    .hide()
    ;
  cp5.addBang("soft_exit")
    .setPosition(700, 610)
    .setSize(70, 40)
    .setId(40)
    .setCaptionLabel("Exit")
    ;
  cp5.addBang("portOpen")
    .setPosition(620, 610)
    .setSize(70, 40)
    .setId(41)
    .setCaptionLabel("Open")
    ;
  cp5.addTextlabel("stext")
    .setText("Sensor PINs")
    .setPosition(426, 25)
    .setFont(createFont("Arial", 18))
    .setColor(color(25, 143, 250, 255))
    ;
  cp5.addTextfield("sensor0")
    .setPosition(430, 50)
    .setSize(100, 35)
    .setId(31)
    .setCaptionLabel("LWR Hook Rail")
    .setFont(createFont("arial", 14))
    ;
  cp5.addTextfield("sensor1")
    .setPosition(430, 110)
    .setSize(100, 35)
    .setId(32)
    .setCaptionLabel("Hanger Rack")
    .setFont(createFont("arial", 14))
    ;
  cp5.addTextfield("sensor2")
    .setPosition(430, 170)
    .setSize(100, 35)
    .setId(33)
    .setCaptionLabel("Main Cycle")
    .setFont(createFont("arial", 14))
    ;
  cp5.addTextfield("sensor3")
    .setPosition(430, 230)
    .setSize(100, 35)
    .setId(34)
    .setCaptionLabel("Crimp Cycle")
    .setFont(createFont("arial", 14))
    ;
  cp5.addTextfield("sensor4")
    .setPosition(430, 290)
    .setSize(100, 35)
    .setId(35)
    .setCaptionLabel("Strip Off")
    .setFont(createFont("arial", 14))
    ;
  cp5.addTextfield("sensor5")
    .setPosition(430, 350)
    .setSize(100, 35)
    .setId(36)
    .setCaptionLabel("Head UP")
    .setFont(createFont("arial", 14))
    ;
  cp5.addTextfield("sensor6")
    .setPosition(430, 410)
    .setSize(100, 35)
    .setId(37)
    .setCaptionLabel("Head Down")
    .setFont(createFont("arial", 14))
    ;
  cp5.addTextfield("sensor7")
    .setPosition(430, 470)
    .setSize(100, 35)
    .setId(38)
    .setCaptionLabel("Crimp Cycle")
    .setFont(createFont("arial", 14))
    ;
  cp5.addTextlabel("rtext")
    .setText("Relay PINs")
    .setPosition(560, 25)
    .setFont(createFont("Arial", 18))
    .setColor(color(25, 143, 250, 255))
    ;
  cp5.addTextfield("relayPin0")
    .setPosition(560, 50)
    .setSize(100, 35)
    .setId(40)
    .setCaptionLabel("Relay PIN 1")
    .setFont(createFont("arial", 14))
    .setAutoClear(false);
  ;
  cp5.addTextfield("relayPin1")
    .setPosition(560, 110)
    .setSize(100, 35)
    .setId(41)
    .setCaptionLabel("Relay PIN 2")
    .setFont(createFont("arial", 14))
    .setAutoClear(false);
  ;
  cp5.addTextfield("relayPin2")
    .setPosition(560, 170)
    .setSize(100, 35)
    .setId(42)
    .setCaptionLabel("Relay PIN 3")
    .setFont(createFont("arial", 14))
    .setAutoClear(false);
  ;
  cp5.addTextfield("relayPin3")
    .setPosition(560, 230)
    .setSize(100, 35)
    .setId(43)
    .setCaptionLabel("Relay PIN 4")
    .setFont(createFont("arial", 14))
    .setAutoClear(false);
  ;
  cp5.addTextfield("relayPin4")
    .setPosition(560, 290)
    .setSize(100, 35)
    .setId(44)
    .setCaptionLabel("Relay PIN 5")
    .setFont(createFont("arial", 14))
    .setAutoClear(false);
  ;
  cp5.addTextfield("relayPin5")
    .setPosition(560, 350)
    .setSize(100, 35)
    .setId(45)
    .setCaptionLabel("Relay PIN 6")
    .setFont(createFont("arial", 14))
    .setAutoClear(false);
  ;
  cp5.addTextfield("relayPin6")
    .setPosition(560, 410)
    .setSize(100, 35)
    .setId(46)
    .setCaptionLabel("Relay PIN 7")
    .setFont(createFont("arial", 14))
    .setAutoClear(false);
  ;
  cp5.addTextfield("relayPin7")
    .setPosition(560, 470)
    .setSize(100, 35)
    .setId(47)
    .setCaptionLabel("Relay PIN 8")
    .setFont(createFont("arial", 14))
    .setAutoClear(false);
  ;
  cp5.addTextfield("relayPin8")
    .setPosition(560, 530)
    .setSize(100, 35)
    .setId(48)
    .setCaptionLabel("Relay PIN 9")
    .setFont(createFont("arial", 14))
    .setAutoClear(false)
    .hide()
    ;
  consoletext = cp5.addTextarea("txt")
    .setPosition(790, 30)
    .setSize(400, 655)
    .setFont(createFont("", 12))
    .setLineHeight(14)
    .setColor(color(255, 255, 255, 255))
    .setColorBackground(color(100, 100))
    .setColorForeground(color(255, 100))

    ;
  cp5.addTextlabel("ctext")
    .setText("Console  |  Program/Machine ")
    .setPosition(785, 3)
    .setColor(color(200, 0, 0, 255))
    .setFont(createFont("Arial", 18))
    ;

  cp5.addTextlabel("stitle")
    .setText("Machine Serial")
    .setPosition(10, 10)
    .setColorValue(color(200, 17, 0, 255))
    .setFont(createFont("Arial", 24))
    ;
/*
  cp5.addTextlabel("connectedLabel")
    .setText("Disconnected")
    .setPosition(25, 560)
    .setFont(createFont("Arial", 22))
    .setColor(color(255, 255, 250, 255))
    ;
  cp5.addTextlabel("pingStat")
    .setText("Ping:")
    .setPosition(170, 560)
    .setFont(createFont("Arial", 22))
    .setColor(color(255, 255, 250, 255))
    ;
    */
  cp5.addTextlabel("subtitle")
    .setText("Control Panel BETA")
    .setPosition(30, 35)
    .setColorValue(200)
    .setFont(createFont("Arial", 16))
    ;
  cp5.mapKeyFor(new ControlKey() {
    public void keyEvent() { 
      keyValue(0);
    }
  }
  , '1');
  cp5.mapKeyFor(new ControlKey() {
    public void keyEvent() { 
      keyValue(1);
    }
  }
  , '2');
  cp5.mapKeyFor(new ControlKey() {
    public void keyEvent() { 
      keyValue(2);
    }
  }
  , '3');
  cp5.mapKeyFor(new ControlKey() {
    public void keyEvent() { 
      keyValue(3);
    }
  }
  , '4');
  cp5.mapKeyFor(new ControlKey() {
    public void keyEvent() { 
      keyValue(4);
    }
  }
  , '5');
  cp5.mapKeyFor(new ControlKey() {
    public void keyEvent() { 
      keyValue(5);
    }
  }
  , '6');
  cp5.mapKeyFor(new ControlKey() {
    public void keyEvent() { 
      keyValue(6);
    }
  }
  , '7');
  cp5.mapKeyFor(new ControlKey() {
    public void keyEvent() { 
      keyValue(7);
    }
  }
  , '8');
  cp5.mapKeyFor(new ControlKey() {
    public void keyEvent() { 
      keyValue(8);
    }
  }
  , '9');
  /*cp5.addSlider("sliderTicks2")
   .setPosition(400,670)
   .setWidth(300)
   .setRange(300,1800) // values can range from big to small as well
   .setValue(300)
   .setNumberOfTickMarks(6)
   .setSliderMode(Slider.FLEXIBLE)
   .setCaptionLabel("")
   ;
   */
  cp5.addSlider("waitSlider")
    .setPosition(720, 460)
    .setSize(30, 120)
    .setRange(100, 1900)
    .setValue(100)
    .setNumberOfTickMarks(7)
    .setCaptionLabel("Wait Time")
    .hide()
    ;
  for (int k = 0; k<9; k++) {
    cp5.addTextlabel("sensorLogic"+k)
      .setText("Sensor " +(k+1))
      .setPosition(670, (50+(k*45)))
      .setFont(createFont("Arial", 14))
      .setColor(color(255, 255, 250, 255))
      .hide()
      ;
  }
  d1 = cp5.addDropdownList("drop1")
    //.setPosition(500,550)
    .setPosition(20,80)
    .setSize(160,400)
    .setBarHeight(30)
    .setItemHeight(30)
    .setCaptionLabel("Port")
    .setFont(createFont("Arial", 14))
    .setId(79)
    ;
  console = cp5.addConsole(consoletext);
  String [] sList = Serial.list();
  int index = 0;
  //change the 0 to a 1 or 2 etc. to match your port
  while(index < sList.length){
    d1.addItem(sList[index],sList[index]);
    /*if(sList[index].equals("COM3")){
      myPort = new Serial(this, "COM3", 19200);
      serialToggle = true;
    }
    if(sList[index].equals("COM7")){
      myPort = new Serial(this, "COM7", 19200);
      serialToggle = true;
    }*/
    index++;
  }
}

void draw() {
  background(0);
  fill(255);
  if (sensorEcho == true) {
    cp5.get(Slider.class,"waitSlider").show();
  }
  if(sensorEcho == false){
    cp5.get(Slider.class,"waitSlider").hide();
  }
  if (ech.millis() >= 1000){
    myPort.write("Test Ran" +endchar);
  }
  if (tabID == 111) {
    /*
    for (int i=0; i<globalState.length; i++) {
      if (globalState[i] == false) {
        fill(color(200, 20, 0, 255));
        rect(385, 50+i*45, 30, 30);
      } else {
        fill(color(123, 255, 0, 255));
        rect(385, 50+i*45, 30, 30);
      }
    }
    */
    for (int k = 0; k<sensorState.length; k++) {
      fill(color(0, sensorState[k], 0, 255));
      rect(735, 50+k*45, 30, 30);
    }
    /*
    if (connected == 0) {
      fill(color(200, 20, 0, 255));
      rect(20, 560, 150, 30);
    } else if (connected == 1) {
      fill(color(123, 255, 0, 255));
      rect(20, 560, 150, 30);
    } */
  }
  //delay(10);
}
void controlEvent(ControlEvent test) {
  if (test.getId() == 79){
    port = Serial.list()[int(test.getController().getValue())];
  }
  /*if (test.isAssignableFrom(Slider.class)){
    int sliderValueFinal = Math.round(test.getValue());
  if(sliderValueFinal > 0){
  //waitTime = sliderValue;
  if(lastSenWait != sliderValueFinal){
    myPort.write("SENWAIT." +sliderValueFinal +endchar);
    lastSenWait = sliderValueFinal;
  }
  } else if (lastSenWait <= 0){
   myPort.write("SENWAIT." +"0" +endchar);
   lastSenWait = 0;
  }
}
  */
  if (test.isAssignableFrom(Textfield.class)) {
    if ("console".equals(test.getName())) {
      temp = test.getStringValue();
      temp = temp.toUpperCase();
      //myPort.write(temp.toUpperCase() + '\n');
      //println("Sent: " +temp.toUpperCase());
      if ("LED On".equals(temp)) {
        myPort.write("2");
        println("Sent: LED ON");
      } else if ("LED Off".equals(temp)) {
        myPort.write("1");
        println("Sent: LED OFF");
      } else if ("LED Off".equals(temp)) {
        myPort.write("1");
        println("Sent: LED OFF");
      } else if ("HELP".equals(temp)) {
        println("Help:");
        println("/  Commands:");
        println("/  - Update.x.y - Updates control pin layout.");
        println("      (x) - Array location  (y) - PIN");
        println("     Starts at 0!");
        println("/  - EEPROM.x.y - Updates EEPROM address (x) with value (y)");
        println("/  - PIN.x.y - Updates PIN (x) with value of (y)"); 
        println("/    !!  [ Value should be 0 or 1 ONLY!] !!");
        println("/  - Call.x - Calls current value of position (x) in array");
        println("/  - Varu.x.y - Updates the position (x) with value of (y) in array");
        println("/");
        println("/");
        println("/  Timer values should stay within value of 5100");
        println("/");
        return;
      } else {
        if (temp.contains("UPDATE")) {
          if (temp.contains("EEPROM")) {
            myPort.write(temp + '\n');
            println("Sent: " +temp);
            return;
          }
          //Check for falure to parse command
          /* !!!!!!!!!!!!! Important Note !!!!!!!!!!!!!!!
           
           Add functionallity to search for amount of '.'s used in the command,  If number is more
           then allowed, trigger error.
           
           */

          if (temp.length()<=9) {
            println("*  SYSTEM | Parsing Error [ '" +temp +"' ]");
            return;
          }
          String apple = temp.substring(7, 8);
          String pear = temp.substring(9, temp.length());
          int orange = Integer.parseInt(apple);
          int grape = Integer.parseInt(pear);
          pin[orange] = grape;
          println("Sending Update: LOC[" +orange +"] Value[" +grape +"]");
        } else {
          myPort.write(temp + '\n');
          println("Sent: " +temp);
        }
      }
    }
    boolean StrTest = test.getName().startsWith("timer");
    if (StrTest == true) {
      println("*   Sending EEPROM Update to controller");
      println("*   Updating timer: " +test.getId() +" to Value: " +test.getStringValue());
      Timefunc((test.getId()-1)*2, test.getStringValue());
    }
    //Recieve Pin Update for Relays
    boolean rstrTest = test.getName().startsWith("relayPin");
    if (rstrTest == true) {
      int rstrId = (test.getId()-40);
      relayArray[rstrId] = Integer.parseInt(test.getStringValue());
      println("RelayArray [" +rstrId +"] just updated to: " +relayArray[rstrId]);
    }
  }
} //End of controlEvent

public void keyEvent(int keyValue) {
  println(keyValue);
  //myPort.write(keyValue +endchar);
}
public void button1() {
  println("Sent: pin.10.1");
  myPort.write("PIN.10.1" +endchar);
}
public void button2() {
  println("Sent: pin.10.0");
  myPort.write("PIN.10.0" +endchar);
}

void waitSlider(int sliderValue) {
  //println(sliderValue);
  if(sliderValue > 0){
  //waitTime = sliderValue;
  if(lastSenWait != sliderValue){
    myPort.write("SENWAIT." +sliderValue +endchar);
    lastSenWait = sliderValue;
  }
  } else if (lastSenWait <= 0){
   myPort.write("SENWAIT." +"0" +endchar);
   lastSenWait = 0;
  }
}
public void button3() {
  println("SYSTEM: Relays On");
  if (varLoad != false) {
    for (byte t=0; t<8; t++) {
      myPort.write("PIN." +relayArray[t] +".0" +endchar);
    }
  }
}

public void button4() {
  println("SYSTEM: Relays Off");
  if (varLoad != false) {
    for (byte t=0; t<8; t++) {
      myPort.write("PIN." +relayArray[t] +".1" +endchar);
    }
  }
}

public void soft_exit() {
  myPort.clear();
  myPort.stop();
  //cp5.getController("drop1").show();
  exit();
}

public void portOpen() {
  myPort = new Serial(this, port, 19200);
  cp5.getController("drop1").hide();
  console.clear();
  console.play();
}

public void sensorCheck() {
  myPort.write("SENCHECK" +endchar);
}

public void sitrepbutton() {
  println("*  SYSTEM: SITREP Triggered");
  myPort.write("SITREP" +endchar);
  cp5.get("sitrepbutton").setColorBackground(color(255, 255, 0, 255));
  cp5.get(Button.class, "sitrepbutton").setFont(createFont("arial", 14));
  cp5.get(Button.class, "sitrepbutton").setColorLabel(color(0, 0, 0));
  cp5.get(Button.class, "sitrepbutton").setCaptionLabel("Loading");
  cp5.get("sitrepbutton").setColorForeground(color(255, 255, 0, 180));
  varLoad = true;
}
public void Relay1() {
  if ((varLoad != false) && (override == true)) {
    RelayControl(relayArray[0], !globalState[0]);
  }
}
public void Relay2() {
  if ((varLoad != false) && (override == true)) {
    RelayControl(relayArray[1], !globalState[1]);
  }
}
public void Relay3() {
  if ((varLoad != false) && (override == true)) {
    RelayControl(relayArray[2], !globalState[2]);
  }
}
public void Relay4() {
  if ((varLoad != false) && (override == true)) {
    RelayControl(relayArray[3], !globalState[3]);
  }
}
public void Relay5() {
  if ((varLoad != false) && (override == true)) {
    RelayControl(relayArray[4], !globalState[4]);
  }
}
public void Relay6() {
  if ((varLoad != false) && (override == true)) {
    RelayControl(relayArray[5], !globalState[5]);
  }
}
public void Relay7() {
  if ((varLoad != false) && (override == true)) {
    RelayControl(relayArray[6], !globalState[6]);
  } else {
    println("ERROR: Controller not loaded");
  }
}
public void Relay8() {
  if ((varLoad != false) && (override == true)) {
    RelayControl(relayArray[7], !globalState[7]);
  }
}
public void Relay9() {
  if ((varLoad != false) && (override == true)) {
    RelayControl(relayArray[8], !globalState[8]);
  }
}
public void RelayControl(int Id, boolean Flag) {
  byte status = 0;
  if (Flag == true) {
    status = 1;
  } else {
    status = 0;
  }
  //cp5.get("Relay"+Id).setColorBackground(color(0,255,255,0));
  myPort.write("PIN." +Id +'.' +status +endchar);
}

public void Timefunc(int Id, String value) {
  boolean overboard = false;
  int control = Integer.parseInt(value);
  if (control > 5100) {
    control=5100;
  }
  control = control/10;
  if (control >= 256) {
    overboard = true;
  }
  if (overboard == false) {
    println("#   DEBUG: ID: " +Id +" Value: " +control);
    myPort.write("EEPROM." +Id +'.' +control +endchar);
    Id++;
    println("#   DEBUG: ID: " +Id +" Value: 0");
    myPort.write("EEPROM." +Id +'.' +"0" +endchar);
  }
  if (overboard == true) {
    control = control - 255;
    println("#   DEBUG: ID: " +Id +" Value: 255");
    myPort.write("EEPROM." +Id +'.' +"255" +endchar);
    Id++;
    println("#   DEBUG: ID: " +Id +" Value: " +control);
    myPort.write("EEPROM." +Id +'.' +control +endchar);
  }
}

void serialEvent(Serial myPort) {
  inByte = myPort.readStringUntil('\n');
  if (inByte != null) {
    inByte = trim(inByte);
    println("RX: " +inByte);
    if (inByte.contains("SUPD")) {
      int fBarrier = (inByte.indexOf('[')+1); //Add one to go to the next position
      int lBarrier = inByte.indexOf(']');
      String result = inByte.substring(fBarrier, lBarrier);
      fBarrier = (inByte.indexOf("Value: ")+7); //Add 7 to compensate for "Value: "
      char state = inByte.charAt(fBarrier);
      boolean isOn;
      if (state == '1') {
        isOn = true;
      } else {
        isOn = false;
      }
      int endresult = parseInt(result);
      for (int i = 0; i<relayArray.length; i++) {
        if (endresult == relayArray[i]) {
          globalState[i] = isOn;
        }
      }
      delay(10);
    }//End of PIN Function
    //Inside Main serial function
    if (inByte.contains("SEN.")) {
      //firstvalue determines what controller to EDIT.
      char firstvalue = inByte.charAt(4);
      //secondvalue determines what value to change the controller from first value.
      String secondvalue = inByte.substring(6, inByte.length());

      //Assign the character from firstvalue to string labeled "timer" to change the string to timer1, timer2, timer3, etc.
      String fvstring = "sensor" + firstvalue;
      int firstint = Character.getNumericValue(firstvalue);

      int secondint = Integer.parseInt(secondvalue);
      if (secondint >= 54) {
        String fValue = "A";
        fValue = fValue + (secondint-54);
        cp5.get(Textfield.class, fvstring).setText(fValue);
      }
      if (secondint < 54) {
        sensorPin[firstint] = secondint;
        cp5.get(Textfield.class, fvstring).setText(secondvalue);
      }
    }
    if (inByte.contains("SOL.")) {
      //firstvalue determines what controller to EDIT.
      char firstvalue = inByte.charAt(4);
      //secondvalue determines what value to change the controller from first value.
      String secondvalue = inByte.substring(6, inByte.length());

      //Assign the character from firstvalue to string labeled "timer" to change the string to timer1, timer2, timer3, etc.
      String fvstring = "relayPin" + firstvalue;
      int firstint = Character.getNumericValue(firstvalue);
      int secondint = Integer.parseInt(secondvalue);
      if (firstint >= 8) {
        String auxString = "Relay" + (firstint+1);
        relayArray = expand(relayArray, firstint+1);
        globalState = expand(globalState, firstint+1);
        cp5.get(Textfield.class, fvstring).show();
        cp5.get(Bang.class, auxString).show();
      }
      relayArray[firstint]=secondint;
      cp5.get(Textfield.class, fvstring).setText(secondvalue);
    }
    if (inByte.contains("EMU.")) {
      //firstvalue determines what controller to EDIT.
      char firstvalue = inByte.charAt(4);
      if (Character.getNumericValue(inByte.charAt(4)) > timerLength){
        return;
      }
      //secondvalue determines what value to change the controller from first value.
      String secondvalue = inByte.substring(6, inByte.length());

      //Assign the character from firstvalue to string labeled "timer" to change the string to timer1, timer2, timer3, etc.
      String fvstring = "timer" + firstvalue;
      cp5.get(Textfield.class, fvstring).show();
      cp5.get(Textfield.class, fvstring).setText(secondvalue);
    }
    if (inByte.contains("INL.")) {
      int arraySize = Character.getNumericValue(inByte.charAt(4));
      if (sensorState.length < arraySize) {
        sensorState = expand(sensorState, 8);
      }
      for (byte k = 0; k < arraySize; k++) {
        String tempstring = "sensorLogic" + k;
        cp5.get(Textlabel.class, tempstring).show();
        int value = Character.getNumericValue(inByte.charAt(6+k));
        //println("Debug 3423: LOC[" +(k+5) +"] Value: " +value);
        if ((value == 0) && (sensorState[k] == 50)) {
          sensorState[k] = 200;
        } else if ((value == 1) && (sensorState[k] == 200)) {
          sensorState[k] = 50;
        } else if (sensorInitial == true) {
          if (value == 0) {
            sensorState[k] = 200;
          } else if (value == 1) {
            sensorState[k] = 50;
          }
        }
      }
      sensorInitial = false;
    }
    /*
    if (inByte.contains("ECHO.")) {
      if (toggleValue == true) {
        cp5.get(Textlabel.class, "connectedLabel").setText("Connected");
        String keyOut = inByte.substring(7, inByte.length());
        char keyId = inByte.charAt(5);
        println("Sent: VERI." +keyId +"." +keyOut);
        connected = 1;
        myPort.write("VERI." +keyId +"." +keyOut +endchar);
      } else if (toggleValue == false) {
        connected = 0;
        cp5.get(Textlabel.class, "connectedLabel").setText("Disconnected");
      }
    }
    */
    
    
    if (inByte.contains("SITREP COMPLETE")) {
      cp5.get("sitrepbutton").setColorBackground(color(0, 255, 0, 255));
      cp5.get("sitrepbutton").setColorForeground(color(0, 255, 0, 180));
      cp5.get("sitrepbutton").setCaptionLabel("Loaded");
    }
    if (inByte.contains("Verified")) {
      connected = 1;
      int pLoc = inByte.indexOf("Ping:");
      int msLoc = inByte.indexOf("ms");
      String ping = inByte.substring(pLoc+5, msLoc);
      cp5.get(Textlabel.class, "connectedLabel").setText("Connected");
      cp5.get(Textlabel.class, "pingStat").setText("Ping: " +ping);
    }
    if (inByte.contains("Override")) {
      char firstvalue = inByte.charAt(9);
      if (firstvalue == 'n'){
        override = true;
      }else{
        override = false;
      }
    }
    if (firstContact == false) {
      if (inByte.equals("<Controller Ready>")) {
        myPort.clear();
        firstContact = true;
        println("SYSTEM: Contact Made");
        delay(100);
        println("Control ready!");
        cp5.get("portOpen").setColorBackground(color(0, 255, 0, 255));
        cp5.get("portOpen").setColorForeground(color(0, 255, 0, 255));
        cp5.get("portOpen").setColorActive(color(0, 255, 0, 255));
      }
    }
  }
}