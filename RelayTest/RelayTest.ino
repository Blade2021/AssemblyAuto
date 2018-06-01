#include <LiquidCrystal.h>
#define SOLARRAYSIZE 8

const byte solenoidArray[SOLARRAYSIZE] = {12, 11, 10, 9, 8, 7, 6, 17};
/*
  12 - [AL-0] Hanger Feed
  11 - [AL-1] Hook Stopper
  10 - [AL-2] Head/Tooling
   9 - [AL-3] Strip Off
   8 - [AL-4] Crimp Stopper
   7 - [AL-5] Crimp
   6 - [AL-6] Vibrator
  17 - [AL-7] MainAir
   x - [AL-8] Motor Relay
*/

LiquidCrystal lcd(18, 19, 5, 4, 3, 2);
unsigned long previousWaitTime;
int waitTime = 1300;
boolean value = HIGH;
int count = 0;

void setup(){
    for(byte k;k< SOLARRAYSIZE;k++){
        pinMode(solenoidArray[k], OUTPUT);
    }

    lcd.begin(20,4);
    lcd.setCursor(0, 0);
}

void loop(){
    lcd.setCursor(0,0);
    lcd.print(millis()/1000);

    if(millis() - previousWaitTime >= waitTime){
        for(byte k; k < SOLARRAYSIZE; k++){
            digitalWrite(solenoidArray[k], value);
            delay(100);
        }
        value = !value;
        count++;
        previousWaitTime = millis();
        lcd.setCursor(0,1);
        lcd.print(count);
    }
}