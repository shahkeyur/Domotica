#pragma once

#include "Arduino.h"
#include <ArduinoJson.h>
#include "FS.h"
#include "Datatypes/ConfigStruct.hpp"
#include "Datatypes/Interface.hpp"

#include "Utils/DeviceId.hpp"
#include "Constants.hpp"
#include "Limits.hpp"

namespace DomoticaInternals {
class Config {
 public:
  Config();
  bool load();
  inline const ConfigStruct& get() const;
  char* getSafeConfigFile() const;
  void erase();
  //void setHomieBootModeOnNextBoot(HomieBootMode bootMode);
  //HomieBootMode getHomieBootModeOnNextBoot();
  void write(const JsonObject& config);
  bool patch(const char* patch);
  void log() const;  // print the current config to log output
  //bool isValid() const;

 private:
  ConfigStruct _configStruct;
  bool _spiffsBegan;
  bool _valid;

  bool _spiffsBegin();
};

const ConfigStruct& Config::get() const {
  return _configStruct;
}
}
