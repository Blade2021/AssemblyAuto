import processing.serial.*;
import controlP5.*;

Serial myPort;  // Create object from Serial class
String inByte;
String outByte;
ControlP5 cp5;
String temp;
String endchar = "\n";
Textarea consoletext;
Println console;
//Variables
boolean firstContact = false;


void setup() 
{  
  size(800,450);
  cp5 = new ControlP5(this);
  cp5.enableShortcuts();
  cp5.addTextfield("console")
    .setSize(360,35)
    .setCaptionLabel("Console")
    .setPosition(20,380)
    .setFont(createFont("arial",20))
    .setAutoClear(true)
    ;
    
  cp5.addTextfield("Timer1")
    .setId(1)
    .setPosition(20,20)
    .setSize(200,35)
    .setFont(createFont("arial",20))
    .setAutoClear(true)
    .setCaptionLabel("Timer 1")
    ;
  cp5.addTextfield("Timer2")
    .setId(2)
    .setPosition(20,80)
    .setSize(200,35)
    .setFont(createFont("arial",20))
    .setAutoClear(true)
    .setCaptionLabel("Timer 2")
    ;
  cp5.addTextfield("Timer3")
    .setId(3)
    .setPosition(20,140)
    .setSize(200,35)
    .setFont(createFont("arial",20))
    .setAutoClear(true)
    .setCaptionLabel("Timer 3")
    ;
  cp5.addTextfield("Timer4")
    .setId(4)
    .setPosition(20,200)
    .setSize(200,35)
    .setFont(createFont("arial",20))
    .setAutoClear(true)
    .setCaptionLabel("Timer 4")
    ;
  cp5.addTextfield("Timer5")
    .setId(5)
    .setPosition(20,260)
    .setSize(200,35)
    .setFont(createFont("arial",20))
    .setAutoClear(true)
    .setCaptionLabel("Timer 5")
    ;
  cp5.addTextfield("Timer6")
    .setId(6)
    .setPosition(20,320)
    .setSize(200,35)
    .setFont(createFont("arial",20))
    .setAutoClear(true)
    .setCaptionLabel("Timer 6")
    ;
  cp5.addButton("button1")
    .setSize(60,20)
    .setCaptionLabel("LED On")
    .setPosition(235,20)
    ;
  cp5.addButton("button2")
    .setSize(60,20)
    .setCaptionLabel("LED Off")
    .setPosition(235,50)
    ;
  cp5.addButton("button3")
    .setSize(60,20)
    .setCaptionLabel("Relays On")
    .setPosition(235,80)
    ;
  cp5.addButton("button4")
    .setSize(60,20)
    .setCaptionLabel("Relays Off")
    .setPosition(235,110)
    ;
consoletext = cp5.addTextarea("txt")
                  .setPosition(390,20)
                  .setSize(400, 398)
                  .setFont(createFont("", 12))
                  .setLineHeight(14)
                  .setColor(color(200))
                  .setColorBackground(color(100, 100))
                  .setColorForeground(color(255, 100));
  ;
  cp5.addTextlabel("ctext")
    .setText("Console")
    .setPosition(386,3)
    .setColorValue(255)
    .setFont(createFont("Arial",14));
    
  console = cp5.addConsole(consoletext);//
  //change the 0 to a 1 or 2 etc. to match your port
  myPort = new Serial(this, "COM3", 9600);
  myPort.bufferUntil('\n');
}

void draw() {
  background(0);
  fill(255);
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
      println("SYSTEM TX: LED ON");
    }else if("LED Off".equals(temp)){
      myPort.write("1");
      println("SYSTEM TX: LED OFF");
    }else {
      myPort.write(temp.toUpperCase() + '\n');
      println("SYSTEM TX: " +temp.toUpperCase());
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
  println("SYSTEM TX: pin.10.1");
  myPort.write("PIN.10.1" +endchar);
}
public void button2(){
  println("SYSTEM TX: pin.10.0");
  myPort.write("PIN.10.0" +endchar);
}
public void button3(){
  println("SYSTEM TX: Relays On");
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
  println("SYSTEM TX: Relays Off");
  myPort.write("PIN.22.1" +endchar);
  myPort.write("PIN.24.1" +endchar);
  myPort.write("PIN.26.1" +endchar);
  myPort.write("PIN.28.1" +endchar);
  myPort.write("PIN.30.1" +endchar);
  myPort.write("PIN.32.1" +endchar);
  myPort.write("PIN.34.1" +endchar);
  myPort.write("PIN.36.1" +endchar);
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
    println("SYSTEM RX: " +inByte);
    
    if (firstContact == false) {
      if (inByte.equals("<Controller is ready>")) {
        myPort.clear();
        firstContact = true;
        //myPort.write("A");
        println("SYSTEM: Contact Made");
      }
    }
  }
}