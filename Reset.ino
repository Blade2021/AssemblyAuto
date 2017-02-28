#include <avr/wdt.h>
void setup() {
  // put your setup code here, to run once:
  MCUSR = 0;
  }
}

void loop() {
  // put your main code here, to run repeatedly:

}
void reset(){
  wdt_Enable(WDTO_15MS);
  for(;;){
  }
}
