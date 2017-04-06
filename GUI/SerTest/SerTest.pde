import processing.serial.*;
import controlP5.*;

Serial myPort;  // Create object from Serial class
String inByte;
String outByte;
ControlP5 cp5;
String temp;
String endchar = "\n";

//Variables
boolean firstContact = false;


void setup() 
{  
  size(600,400); //make our canvas 200 x 200 pixels big
  cp5 = new ControlP5(this);
  
  cp5.addTextfield("text1")
    .setSize(100,30)
    .setCaptionLabel("Text test")
    .setPosition(100,10)
    .setAutoClear(true)
    ;
  
  cp5.addButton("button1")
    .setSize(60,20)
    .setCaptionLabel("LED On")
    .setPosition(20,40)
    ;
  cp5.addButton("button2")
    .setSize(60,20)
    .setCaptionLabel("LED Off")
    .setPosition(20,100)
    ;
    
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
      myPort.write(temp + '\n');
      println("SYSTEM TX: " +temp);
    }
    //myPort.write("\n");
  }
}
public void button1(){
  println("SYSTEM TX: 2");
  myPort.write('2' +endchar);
}
public void button2(){
  println("SYSTEM TX: 1");
  myPort.write('1' +endchar);
}

void serialEvent(Serial myPort) {
  inByte = myPort.readStringUntil('\n');
  if (inByte != null){
    inByte = trim(inByte);
    println("SYSTEM RX: " +inByte);
    
    if (firstContact == false) {
      if (inByte.equals("<Arduino is ready>")) {
        myPort.clear();
        firstContact = true;
        //myPort.write("A");
        println("SYSTEM: Contact Made");
      }
    }
  }
}