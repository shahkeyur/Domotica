#include "Domotica.h"

void onMqttMessageCb(char *topic, char *msg) {
  Domotica.getLogger() << F("Got ") << msg << F(" on ") << topic;
}

void setup() {
  Serial.begin(74880);
  Domotica.getLogger().setLogging(true); // By degault enabled
  Domotica.onMqttMessage(onMqttMessageCb);
  Domotica.setup();
}

void loop() { Domotica.loop(); }
