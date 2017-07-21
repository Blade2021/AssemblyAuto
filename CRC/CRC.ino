#include <util/crc16.h>
#include <EEPROM.h>

void setup(){
  //Start serial
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  //Print length of data to run CRC on.
  Serial.print("EEPROM length: ");
  Serial.println(EEPROM.length());

  //Print the result of calling eeprom_crc()
  Serial.println(make_crc());
  Serial.print("\n\nDone!");
}

void loop() {
  /* Empty loop */
}

uint16_t make_crc()
{
  uint16_t crc = 0;
  for (int i = 0; i < EEPROM.length(); i++)
  {
    crc = _crc16_update(crc, EEPROM.read(i));
  }
  return crc;
}
