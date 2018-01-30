#pragma once

#include <AsyncMqttClient.h>
#include "../Logger.hpp"
#include "../Constants.hpp"
#include "../Config.hpp"
#include "../Limits.hpp"
#include "../../StreamingOperator.hpp"


namespace DomoticaInternals {
class Logger;
class Config;
class DomoticaClass;

class InterfaceData {
  friend DomoticaClass;

 public:
  InterfaceData();

  /***** User configurable data *****/
  char brand[MAX_BRAND_LENGTH];

  struct ConfigurationAP {
    bool secured;
    char password[MAX_WIFI_PASSWORD_LENGTH];
  } configurationAp;

  struct Firmware {
    char name[MAX_FIRMWARE_NAME_LENGTH];
    char version[MAX_FIRMWARE_VERSION_LENGTH];
  } firmware;

  struct LED {
    bool enabled;
    uint8_t pin;
    uint8_t on;
  } led;

  struct Reset {
    bool enabled;
    bool idle;
    uint8_t triggerPin;
    uint8_t triggerState;
    uint16_t triggerTime;
    bool resetFlag;
  } reset;

  bool disable;
  bool flaggedForSleep;

  /***** Runtime data *****/
  bool ready;
  Logger& getLogger() { return *_logger; }
  Config& getConfig() { return *_config; }
  AsyncMqttClient& getMqttClient() { return *_mqttClient; }
 private:
  Logger* _logger;
  Config* _config;
  AsyncMqttClient* _mqttClient;
};

class Interface {
 public:
  static InterfaceData& get();

 private:
  static InterfaceData _interface;
};
}
