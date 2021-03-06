import processing.serial.*;
import controlP5.*;
import cc.arduino.*;

ControlP5 cp5;

//Arduino arduino;

String textValue = "";
Textlabel consoletext;
String temp = "";
int cage = 1;
boolean active = false;
String acclvl = "0";
float count = 20;
int x = 1;

Textarea systemTextArea;

void setup() {
  size(800,450);
  PFont font = createFont("arial",20);
  //test
  cp5 = new ControlP5(this);
  
  //arduino = new Arduino(this, Arduino.list()[0], 57600);
  
  cp5.addTab("extra")
    .setLabel("System Override")
    .setColorBackground(color(0, 160, 100))
    .setColorLabel(color(255))
    .setColorActive(color(255,128,0))
    ;
    
  cp5.getTab("default")
    .setId(1)
    .activateEvent(true)
    .setLabel("Home")
    ;
  cp5.getTab("extra")
    .activateEvent(true)
    .setId(2)
    ;
    
  cp5.addTextfield("Passcode")
      .setLabel("Access Code")
     .setPosition(20,30)
     .setSize(200,40)
     .setFont(font)
     .setFocus(true)
     .setColor(color(255,0,0))
     ;
                 
  cp5.addTextfield("Timer1")
     .setPosition(20,90)
     .setSize(200,40)
     .setFont(createFont("arial",20))
     .setAutoClear(true)
     ;
     
  cp5.addTextfield("Timer2")
     .setPosition(20,150)
     .setSize(200,40)
     .setFont(createFont("arial",20))
     .setAutoClear(true)
     ;
     
  cp5.addTextfield("Timer3")
     .setPosition(20,210)
     .setSize(200,40)
     .setFont(createFont("arial",20))
     .setAutoClear(true)
     ;
  cp5.addTextfield("Timer4")
     .setPosition(20,270)
     .setSize(200,40)
     .setFont(createFont("arial",20))
     .setAutoClear(true)
     ;
  cp5.addTextfield("Timer5")
     .setPosition(20,330)
     .setSize(200,40)
     .setFont(createFont("arial",20))
     .setAutoClear(true)
     ;
  cp5.addBang("clear")
     .setPosition(325,30)
     .setSize(60,20)
     .getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER)
     ;    
  systemTextArea = cp5.addTextarea("txt")
    .setPosition(400,30)
    .setSize(370,380)
    .setFont(createFont("arial",12))
    .setLineHeight(14)
    .setColor(color(128))
    .setColorBackground(color(255,100))
    .setColorForeground(color(255,100));
    ;
  cp5.addBang("Logout")
     .setPosition(720,25)
     .setSize(60,20)
     .hide()
     .getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER)
     ;  
     
  cp5.addTextfield("mouse")
     .setPosition(20,410)
     .setAutoClear(false)  //sets auto clear to erase on enter
     .setCaptionLabel("Modifier")
     ;
  consoletext = cp5.addTextlabel("ctext")
    .setText("Console")
    .setPosition(380,5)
    .setColorValue(255)
    .setFont(createFont("Georgia",20));
    
  cp5.addToggle("Relay1")
     .setPosition(20,40)
     .setSize(80,40)
     .hide()
     .setCaptionLabel("Relay 1")
     .getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER)
     ;
  cp5.addToggle("Relay2")
     .setPosition(20,90)
     .setSize(80,40)
     .hide()
     .setCaptionLabel("Relay 2")
     .getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER)
     ;
     
  cp5.addToggle("Relay3")
     .setPosition(20,140)
     .setSize(80,40)
     .hide()
     .setCaptionLabel("Relay 3")
     .getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER)
     ;
    
  cp5.addToggle("Relay4")
     .setPosition(20,190)
     .setSize(80,40)
     .hide()
     .setCaptionLabel("Relay 4")
     .getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER)
     ;
   cp5.addToggle("Relay5")
     .setPosition(20,240)
     .setSize(80,40)
     .hide()
     .setCaptionLabel("Relay 5")
     .getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER)
     ;
   cp5.addToggle("Relay6")
     .setPosition(20,290)
     .setSize(80,40)
     .hide()
     .setCaptionLabel("Relay 6")
     .getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER)
     ;
   cp5.addToggle("Relay7")
     .setPosition(20,340)
     .setSize(80,40)
     .hide()
     .setCaptionLabel("Relay 7")
     .getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER)
     ;
    cp5.addToggle("Relay8")
     .setPosition(20,390)
     .setSize(80,40)
     .hide()
     .setCaptionLabel("Relay 8")
     .getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER)
     ;
  cp5.getController("Timer1").moveTo("default");
  cp5.getController("Timer2").moveTo("default");
  cp5.getController("Timer3").moveTo("default");
  cp5.getController("Timer4").moveTo("default");
  cp5.getController("Timer5").moveTo("default");
  cp5.getController("mouse").moveTo("default");
  cp5.getController("Passcode").moveTo("global");
  cp5.getController("clear").moveTo("default");
  cp5.getController("Logout").moveTo("global");
  cp5.getController("Relay1").moveTo("extra");
  cp5.getController("Relay2").moveTo("extra");
  cp5.getController("Relay3").moveTo("extra");
  cp5.getController("Relay4").moveTo("extra");
  cp5.getController("Relay5").moveTo("extra");
  cp5.getController("Relay6").moveTo("extra");
  cp5.getController("Relay7").moveTo("extra");
  cp5.getController("Relay8").moveTo("extra");
  cp5.getController("ctext").moveTo("default");
  textFont(font);
}

void draw() {
  background(0);
  fill(255);
  //text(cp5.get(Textfield.class,"Passcode").getText(), 360,130);  
  // Displays text to the right
  text(textValue, 360,180);
  if (active == true){
  //text(cp5.get(Textfield.class,"mouse").getText(),360,count);
  //count = count + 10;
  text("Access Level: "+acclvl, 600,20); 
  }
}

public void clear() {
  systemTextArea.setText("");
}

public void mouse(String theValue) {
  int mod = Integer.valueOf(theValue);
  cage = mod;
}

public void Passcode(String theValue) {    
 int pass = Integer.valueOf(theValue);
 if (pass == 0000){
   active=true;
   int i = 1;
   String r = "Relay";
   while (i < 9){
    r = "Relay"+i;
    cp5.get(controlP5.Controller.class,r).show(); 
    i++;
   }
   acclvl = "1";
   cp5.get(controlP5.Controller.class,"Logout").show();
   cp5.get(controlP5.Controller.class,"Passcode").hide();
 }
 else if (pass == 7777) {
   active=true;
   int i = 1;
   String r = "Relay";
   while (i < 9){
    r = "Relay"+i;
    cp5.get(controlP5.Controller.class,r).show(); 
    i++;
   }
   acclvl = "ADMIN";
   cp5.get(controlP5.Controller.class,"Logout").show();
   cp5.get(controlP5.Controller.class,"Passcode").hide();
 }
 else {
   temp = systemTextArea.getText();
   systemTextArea.setText(""+temp+"\n"+"ERROR: Wrong access code entered.");
   return;
 }
 temp = systemTextArea.getText();
 systemTextArea.setText(""+temp+"\n"+"Access Level "
 +acclvl
 +" has logged in");
}

public void Logout(){
 active=false;
 cp5.get(controlP5.Controller.class,"Logout").hide();
 cp5.get(controlP5.Controller.class,"Passcode").show();
   int i = 1;
   String r = "Relay";
   while (i < 9){
    r = "Relay"+i;
    cp5.get(controlP5.Controller.class,r).hide(); 
    i++;
   }
 temp = systemTextArea.getText();
 systemTextArea.setText(""+temp+"\n"+"Logged Out");
}

void controlEvent(ControlEvent test) {
  x = test.getId();
  println(x);
  if(test.isAssignableFrom(Textfield.class)) {
    println("controlEvent: accessing a string from controller '"
            +test.getName()+"': "
            +test.getStringValue()
            );
    int foo = Integer.parseInt(test.getStringValue());
    /* Another possibly preferred method would be to get the value of the string
    instead of parsing the string. ex.
    
    Integer y = Integer.valueOf(test.getStringValue());
    println(y);
    
    */
    int rat = (cage*foo);
    println("Integer: "
    +foo //+ sign used to add to the println function 
    +"  Rat = "
    +rat
    +" ACTIVE: "
    +active
    );
    String pen = test.getName();
    if (pen != "Passcode") {
    temp = systemTextArea.getText();
    systemTextArea.setText(""
    +temp
    +"\n"
    +test.getName()+" : "
    +rat);
    }
  }
}
void keyPressed(){
 if(keyCode==TAB){
   if ((x==-1) || (x==1)){
     cp5.getTab("extra").bringToFront();
     x = 2;
   }
   else {
     cp5.getTab("default").bringToFront();
     x = 1;
   }
 }
}

public void input(String theText) {
  // automatically receives results from controller input
  //test test
  println("a textfield event for controller 'input' : "+theText);
}