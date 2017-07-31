#include <EEPROM.h>
#include <LiquidCrystal.h>

// start of TEST VARIABLES
int NextButton = LOW;
int DownButton = LOW;
int UpButton = LOW;
int currentTime = 0;
int preButtonTime = 0;
int ButtonWait = 0;

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup(){
}

void loop() {
}
// end of TEST VARIABLES

void ReadErrors(){
  byte ErrorArray[10] = {0};
  byte ErrorStart = 20;
  ErrorArray[0] = EEPROM.read(ErrorStart);
  if (ErrorArray[0] > 10){
    ErrorArray[0] = 10;
  }
  byte quickexit = 0;
  for(byte k = 1; k < ErrorArray[0];k++){
    ErrorArray[k] = EEPROM.read(ErrorStart+k);
  }
  byte whilevariable = 0;
  while(whilevariable < ErrorArray[0]){
    switch(whilevariable){
      case 001:
        break;
      default:
        break;
    }
    if((NextButton == HIGH) && (currentTime - preButtonTime >= ButtonWait)){
      whilevariable++;
    }
    if(DownButton == HIGH){
      quickexit = 1;
      break;
    }
  }
  if(quickexit == 1){
    lcd.print("Left Error List");
  }
  if(quickexit == 0){
    lcd.print("End of ERRORS");
  }
  byte erase = 0;
  while(1==1){
    lcd.print("Erase Error List?");
    if(UpButton == HIGH){
      EEPROM.update(ErrorStart, 0);
      break;
    }
    if(DownButton == HIGH){
      break;
    }
  }
}

