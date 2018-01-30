#include "Interface.hpp"

using namespace DomoticaInternals;

InterfaceData Interface::_interface;  // need to define the static variable

InterfaceData::InterfaceData()
  : ready{ false }
  ,_logger{ nullptr }
  , _config{ nullptr }
  , _mqttClient{ nullptr }{
    }

InterfaceData& Interface::get() {
  return _interface;
}
