import processing.serial.*;
import controlP5.*;

ControlP5 cp5;

String textValue = "";
int cage = 1;
boolean active = false;
float count = 20;

void setup() {
  size(800,450);
  PFont font = createFont("arial",20);
  //test
  cp5 = new ControlP5(this);
  
  cp5.addTab("extra")
    .setLabel("extra")
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
     .setAutoClear(false)
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
     .setPosition(240,90)
     .setSize(80,40)
     .getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER)
     ;    
  
  cp5.addBang("Logout")
     .setPosition(720,40)
     .setSize(60,20)
     .hide()
     .getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER)
     ;  
     
  cp5.addTextfield("mouse")
     .setPosition(20,350)
     .setAutoClear(false)  //sets auto clear to erase on enter
     .setCaptionLabel("Modifier")
     ;
     
  cp5.addBang("Relay1")
     .setPosition(240,10)
     .setSize(80,40)
     .hide()
     .setCaptionLabel("Relay 1")
     .getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER)
     ;
  cp5.addBang("Relay2")
     .setPosition(240,60)
     .setSize(80,40)
     .hide()
     .setCaptionLabel("Relay 2")
     .getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER)
     ;
     
  cp5.addBang("Relay3")
     .setPosition(240,110)
     .setSize(80,40)
     .hide()
     .setCaptionLabel("Relay 3")
     .getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER)
     ;
     
  cp5.addBang("Relay4")
     .setPosition(240,160)
     .setSize(80,40)
     .hide()
     .setCaptionLabel("Relay 4")
     .getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER)
     ;
   cp5.addBang("Relay5")
     .setPosition(240,210)
     .setSize(80,40)
     .hide()
     .setCaptionLabel("Relay 5")
     .getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER)
     ;
   cp5.addBang("Relay6")
     .setPosition(240,260)
     .setSize(80,40)
     .hide()
     .setCaptionLabel("Relay 6")
     .getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER)
     ;
   cp5.addBang("Relay7")
     .setPosition(240,310)
     .setSize(80,40)
     .hide()
     .setCaptionLabel("Relay 7")
     .getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER)
     ;
    cp5.addBang("Relay8")
     .setPosition(240,360)
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
  cp5.getController("mouse").moveTo("extra");
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
  text("Welcome Back", 660,20); 
  if (count > 400) {
    count = 0;
  }
  delay(100);
  }
}

public void clear() {
  cp5.get(Textfield.class,"Timer 1").clear();
  count = (count+10);
  //Clear button
}

public void mouse(String theValue) {
  int mod = Integer.valueOf(theValue);
  cage = mod;
}
public void welcome() {
 text("Welcome Back", 550,20);
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
   cp5.get(controlP5.Controller.class,"Logout").show();
 }
 else{
   active=false;
   cp5.get(controlP5.Controller.class,"Logout").hide();
   int i = 1;
   String r = "Relay";
   while (i < 9){
    r = "Relay"+i;
    cp5.get(controlP5.Controller.class,r).hide(); 
    i++;
   }
 }
}

public void Logout(){
 active=false;
 cp5.get(controlP5.Controller.class,"Logout").hide();
   int i = 1;
   String r = "Relay";
   while (i < 9){
    r = "Relay"+i;
    cp5.get(controlP5.Controller.class,r).hide(); 
    i++;
   }
}

void controlEvent(ControlEvent test) {
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
  }
}

void keyPressed(){
 if(keyCode==TAB){
   cp5.getTab("extra").bringToFront();
 }
}

public void input(String theText) {
  // automatically receives results from controller input
  //test test
  println("a textfield event for controller 'input' : "+theText);
}