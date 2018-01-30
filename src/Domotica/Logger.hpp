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

 private:
  void setPrinter(Print* printer);
  void setLogging(bool enable);

  bool _loggingEnabled;
  Print* _printer;
};
} 
