#include "Domotica.h"

void setup(){
  Serial.begin(74880);
  Domotica.setup();
}

void loop(){
  Domotica.loop();
}
