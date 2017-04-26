import processing.serial.*;
import controlP5.*;

Serial myPort;  // Create object from Serial class
String inByte;
String s;
ControlP5 cp5;
String temp;
String endchar = "\n";
Textarea consoletext;
Textfield Timer1;
Textfield Timer2;
Textfield RPin1;
Textfield RPin2;
Textfield RPin3;
Textfield RPin4;
Textfield RPin5;
Textfield RPin6;
Textfield RPin7;
Textfield RPin8;

Println console;
//Variables
boolean firstContact = false;
boolean globalState [] = {false, false, false, false, false, false, false, false};
int pin [] = {22, 24, 26, 28, 30, 32, 34, 36};
int tabID = 111;


void setup() 
{
  noStroke();
  size(1200, 700);
  cp5 = new ControlP5(this);
  cp5.enableShortcuts();

  cp5.addTextfield("console")
    .setSize(360, 35)
    .setCaptionLabel("Console Injection Module")
    .setPosition(20, 630)
    .setFont(createFont("arial", 14))
    .setAutoClear(true)

    ;

  Timer1 = cp5.addTextfield("Timer1")
    .setId(1)
    .setPosition(20, 70)
    .setSize(200, 35)
    .setFont(createFont("arial", 16))
    .setAutoClear(true)
    .setCaptionLabel("EEPROM: Timer 1")
    ;
  Timer2 = cp5.addTextfield("Timer2")
    .setId(2)
    .setPosition(20, 130)
    .setSize(200, 35)
    .setFont(createFont("arial", 16))
    .setAutoClear(true)
    .setCaptionLabel("EEPROM: Timer 2")
    ;
  cp5.addTextfield("Timer3")
    .setId(3)
    .setPosition(20, 190)
    .setSize(200, 35)
    .setFont(createFont("arial", 12))
    .setAutoClear(true)
    .setCaptionLabel("EEPROM: Timer 3")
    ;
  cp5.addTextfield("Timer4")
    .setId(4)
    .setPosition(20, 250)
    .setSize(200, 35)
    .setFont(createFont("arial", 12))
    .setAutoClear(false)
    .setCaptionLabel("EEPROM: Timer 4")
    ;
  cp5.addTextfield("Timer5")
    .setId(5)
    .setPosition(20, 310)
    .setSize(200, 35)
    .setFont(createFont("arial", 12))
    .setAutoClear(false)
    .setCaptionLabel("EEPROM: Timer 5")
    ;
  cp5.addTextfield("Timer6")
    .setId(6)
    .setPosition(20, 370)
    .setSize(200, 35)
    .setFont(createFont("arial", 12))
    .setAutoClear(false)
    .setCaptionLabel("EEPROM: Timer 6")
    ;
  cp5.addButton("button1")
    .setSize(60, 20)
    .setCaptionLabel("LED On")
    .setPosition(235, 70)
    ;
  cp5.addButton("button2")
    .setSize(60, 20)
    .setCaptionLabel("LED Off")
    .setPosition(235, 100)
    ;
  cp5.addButton("button4")
    .setSize(60, 20)
    .setCaptionLabel("Relays On")
    .setPosition(235, 130)
    ;
  cp5.addButton("button3")
    .setSize(60, 20)
    .setCaptionLabel("Relays Off")
    .setPosition(235, 160)
    ;
  cp5.addBang("Relay1")
    .setPosition(310, 50)
    .setId(1)
    .setSize(60, 30)
    .setCaptionLabel("relay1")
    ;

  cp5.addBang("Relay2")
    .setPosition(310, 95)
    .setSize(60, 30)
    .setId(2)
    .setCaptionLabel("Relay 2")
    ;

  cp5.addBang("Relay3")
    .setPosition(310, 140)
    .setSize(60, 30)
    .setId(3)
    .setCaptionLabel("Relay 3")
    ;

  cp5.addBang("Relay4")
    .setPosition(310, 185)
    .setSize(60, 30)
    .setId(4)
    .setCaptionLabel("Relay 4")
    ;

  cp5.addBang("Relay5")
    .setPosition(310, 230)
    .setSize(60, 30)
    .setId(5)
    .setCaptionLabel("Relay 5")
    ;
  cp5.addBang("Relay6")
    .setPosition(310, 275)
    .setSize(60, 30)
    .setId(6)
    .setCaptionLabel("Relay 6")
    ;
  cp5.addBang("Relay7")
    .setPosition(310, 320)
    .setSize(60, 30)
    .setId(7)
    .setCaptionLabel("Relay 7")
    ;
  cp5.addBang("Relay8")
    .setPosition(310, 365)
    .setSize(60, 30)
    .setId(8)
    .setCaptionLabel("Relay 8")
    ;
  cp5.addTextlabel("stext")
    .setText("Sensor PINs")
    .setPosition(426, 25)
    .setFont(createFont("Arial", 18))
    .setColor(color(25, 143, 250, 255))
    ;
  cp5.addTextfield("Sensor1")
    .setPosition(430, 50)
    .setSize(100, 35)
    .setId(31)
    .setCaptionLabel("Hanger Rack")
    .setFont(createFont("arial", 14))
    ;
  cp5.addTextfield("Sensor2")
    .setPosition(430, 110)
    .setSize(100, 35)
    .setId(32)
    .setCaptionLabel("Main Cycle")
    .setFont(createFont("arial", 14))
    ;
  cp5.addTextfield("Sensor3")
    .setPosition(430, 170)
    .setSize(100, 35)
    .setId(33)
    .setCaptionLabel("LWR Hook Rail")
    .setFont(createFont("arial", 14))
    ;
  cp5.addTextfield("Sensor4")
    .setPosition(430, 230)
    .setSize(100, 35)
    .setId(34)
    .setCaptionLabel("UPR Hook Rail")
    .setFont(createFont("arial", 14))
    ;
  cp5.addTextfield("Sensor5")
    .setPosition(430, 290)
    .setSize(100, 35)
    .setId(35)
    .setCaptionLabel("Strip Off")
    .setFont(createFont("arial", 14))
    ;
  cp5.addTextfield("Sensor6")
    .setPosition(430, 350)
    .setSize(100, 35)
    .setId(36)
    .setCaptionLabel("Head UP")
    .setFont(createFont("arial", 14))
    ;
  cp5.addTextfield("Sensor7")
    .setPosition(430, 410)
    .setSize(100, 35)
    .setId(37)
    .setCaptionLabel("Head Down")
    .setFont(createFont("arial", 14))
    ;
  cp5.addTextfield("Sensor8")
    .setPosition(430, 470)
    .setSize(100, 35)
    .setId(38)
    .setCaptionLabel("Crimp Cycle")
    .setFont(createFont("arial", 14))
    ;
  RPin1 = cp5.addTextfield("RelayPin1")
    .setPosition(560, 50)
    .setSize(100, 35)
    .setId(40)
    .setCaptionLabel("Relay PIN 1")
    .setFont(createFont("arial", 14))
    .setAutoClear(false);
  ;
  RPin2 = cp5.addTextfield("RelayPin2")
    .setPosition(560, 110)
    .setSize(100, 35)
    .setId(41)
    .setCaptionLabel("Relay PIN 2")
    .setFont(createFont("arial", 14))
    .setAutoClear(false);
  ;
  RPin3 = cp5.addTextfield("RelayPin3")
    .setPosition(560, 170)
    .setSize(100, 35)
    .setId(42)
    .setCaptionLabel("Relay PIN 3")
    .setFont(createFont("arial", 14))
    .setAutoClear(false);
  ;
  RPin4 = cp5.addTextfield("RelayPin4")
    .setPosition(560, 230)
    .setSize(100, 35)
    .setId(43)
    .setCaptionLabel("Relay PIN 4")
    .setFont(createFont("arial", 14))
    .setAutoClear(false);
  ;
  RPin5 = cp5.addTextfield("RelayPin5")
    .setPosition(560, 290)
    .setSize(100, 35)
    .setId(44)
    .setCaptionLabel("Relay PIN 5")
    .setFont(createFont("arial", 14))
    .setAutoClear(false);
  ;
  RPin6 = cp5.addTextfield("RelayPin6")
    .setPosition(560, 350)
    .setSize(100, 35)
    .setId(45)
    .setCaptionLabel("Relay PIN 6")
    .setFont(createFont("arial", 14))
    .setAutoClear(false);
  ;
  RPin7 = cp5.addTextfield("RelayPin7")
    .setPosition(560, 410)
    .setSize(100, 35)
    .setId(46)
    .setCaptionLabel("Relay PIN 7")
    .setFont(createFont("arial", 14))
    .setAutoClear(false);
  ;
  RPin8 = cp5.addTextfield("RelayPin8")
    .setPosition(560, 470)
    .setSize(100, 35)
    .setId(47)
    .setCaptionLabel("Relay PIN 8")
    .setFont(createFont("arial", 14))
    .setAutoClear(false);
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

  cp5.addTextlabel("subtitle")
    .setText("Control Panel BETA")
    .setPosition(30, 35)
    .setColorValue(200)
    .setFont(createFont("Arial", 16))
    ;

  console = cp5.addConsole(consoletext);
  RelayUpdate();

  //change the 0 to a 1 or 2 etc. to match your port
  myPort = new Serial(this, "COM3", 9600);
  myPort.bufferUntil('\n');
}

void draw() {
  background(0);
  fill(255);
  if (tabID == 111) {
    for (int i=0; i<globalState.length; i++) {
      if (globalState[i] == false) {
        fill(color(200, 20, 0, 255));
        rect(385, 50+i*45, 30, 30);
      } else {
        fill(color(123, 255, 0, 255));
        rect(385, 50+i*45, 30, 30);
      }
    }
  }
}
void controlEvent(ControlEvent test) {
  if (test.isAssignableFrom(Textfield.class)) {
    if ("console".equals(test.getName())) {
      temp = test.getStringValue();
      if ("LED On".equals(temp)) {
        myPort.write("2");
        println("Sent: LED ON");
      } else if ("LED Off".equals(temp)) {
        myPort.write("1");
        println("Sent: LED OFF");
      } else if ("help".equals(temp)) {
        println("Help:");
        println("/  Commands:");
        println("/  - Update.x.y - Updates control pin layout.");
        println("     (x) - Array location  (y) - PIN");
        println("    Starts at 0!");
        println("/  - EEPROM.x.y - Updates EEPROM address (x) with value (y)");
        println("/  - PIN.x.y - Updates PIN (x) with value of (y)"); 
        println("/    [ Value should be 0 or 1 ONLY!]");
        println("/  - Call.x - Calls current value of position (x) in array");
        println("/  - Varu.x.y - Updates the position (x) with value of (y) in array");
        println("/");
        println("/");
        println("/  Timer values should stay within value of 5100");
        println("/");
      } else {
        String result = temp.substring(0, 6);
        //if(temp.contains("Update"){
        if ("Update".equals(result)) {
          String apple = temp.substring(7, 8);
          String pear = temp.substring(9, temp.length());
          int orange = Integer.parseInt(apple);
          int grape = Integer.parseInt(pear);
          pin[orange] = grape;
          RelayUpdate();
        } else {
          myPort.write(temp.toUpperCase() + '\n');
          println("Sent: " +temp.toUpperCase());
        }
      }
    }
    boolean StrTest = test.getName().startsWith("Timer");

    if (StrTest == true) {
      println("Sending EEPROM Update to controller");
      println("Updating Timer: " +test.getId() +" to Value: " +test.getStringValue());
      Timefunc(test.getId()-1, test.getStringValue());
    }
    //Recieve Pin Update for Relays
    if (test.getId()>=40) {
      for (byte t=0; t<8; t++) {
        if (test.getId()==(40+t)) {
          int tempx = Integer.parseInt(test.getStringValue());
          pin[t]=tempx;
        }
      }
      RelayUpdate();
    }
  }
  if (test.isTab()) {
    String tempx = test.getTab().getName();
    if (tempx == "Sensors") {
      tabID = 222;
    } else {
      tabID = 111;
    }
  }
}
public void button1() {
  println("Sent: pin.10.1");
  myPort.write("PIN.10.1" +endchar);
}
public void button2() {
  println("Sent: pin.10.0");
  myPort.write("PIN.10.0" +endchar);
}
public void button3() {
  println("SYSTEM: Relays On");
  myPort.write("PIN.22.0" +endchar);
  myPort.write("PIN.24.0" +endchar);
  myPort.write("PIN.26.0" +endchar);
  myPort.write("PIN.28.0" +endchar);
  myPort.write("PIN.30.0" +endchar);
  myPort.write("PIN.32.0" +endchar);
  myPort.write("PIN.34.0" +endchar);
  myPort.write("PIN.36.0" +endchar);
}
public void button4() {
  println("SYSTEM: Relays Off");
  myPort.write("PIN.22.1" +endchar);
  myPort.write("PIN.24.1" +endchar);
  myPort.write("PIN.26.1" +endchar);
  myPort.write("PIN.28.1" +endchar);
  myPort.write("PIN.30.1" +endchar);
  myPort.write("PIN.32.1" +endchar);
  myPort.write("PIN.34.1" +endchar);
  myPort.write("PIN.36.1" +endchar);
}
public void Relay1() {
  RelayControl(pin[0], !globalState[0]);
}
public void Relay2() {
  RelayControl(pin[1], !globalState[1]);
}
public void Relay3() {
  RelayControl(pin[2], !globalState[2]);
}
public void Relay4() {
  RelayControl(pin[3], !globalState[3]);
}
public void Relay5() {
  RelayControl(pin[4], !globalState[4]);
}
public void Relay6() {
  RelayControl(pin[5], !globalState[5]);
}
public void Relay7() {
  RelayControl(pin[6], !globalState[6]);
}
public void Relay8() {
  RelayControl(pin[7], !globalState[7]);
}
public void RelayControl(int Id, boolean Flag) {
  byte status = 0;
  if (Flag == true) {
    status = 1;
  } else {
    status = 0;
  }
  myPort.write("PIN." +Id +'.' +status +endchar);
}

void SensorCheck(int value) {
  myPort.write("SC." +value +endchar);
}

public void Timefunc(int Id, String value) {
  byte overboard = 0;
  int control = Integer.parseInt(value);
  if (control > 5100) {
    control=5100;
  }
  control = control/10;
  if (control >= 256) {
    overboard = 1;
  }
  if (overboard == 0) {
    myPort.write("EEPROM." +Id +'.' +control +endchar);
    Id++;
    myPort.write("EEPROM." +Id +'.' +"0" +endchar);
  }
  if (overboard == 1) {
    control = control - 255;
    myPort.write("EEPROM." +Id +'.' +"255" +endchar);
    Id++;
    myPort.write("EEPROM." +Id +'.' +control +endchar);
  }
}

void serialEvent(Serial myPort) {
  inByte = myPort.readStringUntil('\n');
  if (inByte != null) {
    inByte = trim(inByte);
    println("Recieved: " +inByte);
    if (inByte.contains("PIN.")) {
      String result = inByte.substring(4, 6);
      char state = inByte.charAt(7);
      boolean isOn;
      if (state == '1') {
        isOn = true;
      } else {
        isOn = false;
      }
      int endresult = parseInt(result);
      for (int i = 0; i<pin.length; i++) {
        if (endresult == pin[i]) {
          globalState[i] = isOn;
        }
      }
      delay(10);
    }//End of PIN Function
    if (inByte.contains("SEN.")) {
      char firstvalue = inByte.charAt(4);
      String secondvalue = inByte.substring(6, inByte.length());
      int fvresult = Character.getNumericValue(firstvalue);
      //int svresult = Integer.parseInt(secondvalue);
      //String fvstring = "Timer" + firstvalue;
      //println("FirstValue String =" +fvstring);
      //Controller jess = cp5.getController(fvstring);
      //println(jess.getValue());
      switch (fvresult) {
      case 1:
        Timer1.setText(secondvalue);
        break;
      case 2:
        Timer2.setText(secondvalue);
        break;
      default:
        println("SYSTEM: Value recieved was irregular");
        break;
      }
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

void RelayUpdate() {
  println("Updating relay variables to current settings");
  for (byte t=0; t<8; t++) {
    switch(t) {
    case 0:
      RPin1.setText(Integer.toString(pin[0]));
      break;
    case 1:
      RPin2.setText(Integer.toString(pin[1]));
      break;
    case 2:
      RPin3.setText(Integer.toString(pin[2]));
      break;
    case 3:
      RPin4.setText(Integer.toString(pin[3]));
      break;
    case 4:
      RPin5.setText(Integer.toString(pin[4]));
      break;
    case 5:
      RPin6.setText(Integer.toString(pin[5]));
      break;
    case 6:
      RPin7.setText(Integer.toString(pin[6]));
      break;
    case 7:
      RPin8.setText(Integer.toString(pin[7]));
      break;
    default:
      break;
    }
  }
}