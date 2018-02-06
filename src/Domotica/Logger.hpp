#pragma once

#include "Arduino.h"

namespace DomoticaInternals {
class DomoticaClass;
class Logger : public Print {
  friend DomoticaClass;

 public:
  Logger();
  virtual size_t write(uint8_t character);
  virtual size_t write(const uint8_t* buffer, size_t size);
  void setLogging(bool enable);

 private:
  void setPrinter(Print* printer);

  bool _loggingEnabled;
  Print* _printer;
};
}
