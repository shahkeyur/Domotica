#pragma once

#include "Arduino.h"
#include "AsyncMqttClient.h"
#include "H4.h"

#include "FS.h"
#include "mDNSResolver.h"
 #include <map>
#include "Domotica/Datatypes/ConfigStruct.hpp"
#include "Domotica/Datatypes/Interface.hpp"
#include "Domotica/Utils/DeviceId.hpp"
#include "Domotica/Config.hpp"
#include "Domotica/Logger.hpp"
#include "Domotica/ExponentialBackoffTimer.hpp"

#include "StreamingOperator.hpp"


namespace DomoticaInternals {

class DomoticaClass {
 public:
  DomoticaClass();
  ~DomoticaClass();

  void setup();
  void loop();
  bool isConfigured();
  bool isConnected();

  typedef void (*callback)(char*,char*);
  void onMqttMessage(callback ptr_reg_callback);

  AsyncMqttClient& getMqttClient();
  Logger& getLogger();
  IPAddress resolveName(const char* name);
  static const ConfigStruct& getConfiguration();

 private:

  void _getConfig();
  void _connectWifi();
  void _connectMqtt();
  void _setupCallbacks();
  void _checkForEvent(char* topic,char* payload);
  callback onMqttMessageCallback;

  H4 _reconnectTimer;
  uint32_t _wifiReconnectCounter;
  uint32_t _mqttReconnectCounter;

  uint32_t _mqttReconnectTimer;
  uint32_t _wifiReconnectTimer;


  WiFiEventHandler _wifiConnectHandler;
  WiFiEventHandler _wifiDisconnectHandler;

  AsyncMqttClient _mqttClient;
  Logger _logger;
  Config _config;

  void _onWifiConnect(const WiFiEventStationModeGotIP& event);
  void _onWifiDisconnect(const WiFiEventStationModeDisconnected& event);
  void _onMqttConnect(bool sessionPresent);
  void _onMqttDisconnect(AsyncMqttClientDisconnectReason reason);
  void _onMqttPublish(uint16_t packetId);
  void _onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total);
};
};

extern DomoticaInternals::DomoticaClass Domotica;
