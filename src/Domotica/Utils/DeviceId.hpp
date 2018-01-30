#pragma once

#include "Arduino.h"

#include <ESP8266WiFi.h>

#include "../Limits.hpp"

namespace DomoticaInternals {
class DeviceId {
 public:
  static void generate();
  static const char* getId();
  static const char* getName();

 private:
  static char _deviceId[MAX_MAC_STRING_LENGTH + 1];
  static char _deviceName[20];

};
};
