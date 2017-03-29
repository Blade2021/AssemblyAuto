import processing.serial.*;
import controlP5.*;

ControlP5 cp5;

String textValue = "";
int cage = 1;
boolean active = false;
float count = 20;

void setup() {
  size(700,400);
  PFont font = createFont("arial",20);
  
  cp5 = new ControlP5(this);
  
  cp5.addTextfield("Passcode")
     .setPosition(20,30)
     .setSize(200,40)
     .setFont(font)
     .setFocus(true)
     .setColor(color(255,0,0))
     ;
                 
  cp5.addTextfield("Timer 1")
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
     
  cp5.addTextfield("Timer 3")
     .setPosition(20,210)
     .setSize(200,40)
     .setFont(createFont("arial",20))
     .setAutoClear(true)
     ;
  cp5.addBang("clear")
     .setPosition(240,90)
     .setSize(80,40)
     .getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER)
     ;    
  
  cp5.addTextfield("mouse")
     .setPosition(20,350)
     .setAutoClear(false)  //sets auto clear to erase on enter
     ;
     
  textFont(font);
}

void draw() {
  background(0);
  fill(255);
  //text(cp5.get(Textfield.class,"Passcode").getText(), 360,130);  
  // Displays text to the right
  text(textValue, 360,180);
  if (active == true){
  text(cp5.get(Textfield.class,"mouse").getText(),360,count);
  count = count + 10;
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

public void Passcode(String theValue) {
 int pass = Integer.valueOf(theValue);
 if (pass == 7777){
   active=true;
 }
 else{
   active=false;
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


public void input(String theText) {
  // automatically receives results from controller input
  println("a textfield event for controller 'input' : "+theText);
}
