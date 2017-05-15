#include <EEPROM.h>
ErrorArray[]={0};

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
  while(byte k < ErrorArray[0]){
    switch(errorID){
      case 001:
        break;
      default:
        break;
    }
    if((NextButton == HIGH) && (currentTime - preButtonTime >= ButtonWait)){
      k++
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
  while(byte erase == 0){
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

