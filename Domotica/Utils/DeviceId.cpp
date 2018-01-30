#include "DeviceId.hpp"

using namespace DomoticaInternals;

char DeviceId::_deviceId[];  // need to define the static variable
char DeviceId::_deviceName[];

void DeviceId::generate() {
  uint8_t mac[6];
  WiFi.macAddress(mac);
  snprintf(DeviceId::_deviceId, MAX_MAC_STRING_LENGTH+1 , "%02x%02x%02x%02x%02x%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  sprintf(DeviceId::_deviceName, "Domotica-%06d", DeviceId::_deviceId + millis());

}

const char* DeviceId::getId() {
  return DeviceId::_deviceId;
}

const char* DeviceId::getName() {
  return DeviceId::_deviceName;
}
