#include "Domotica.hpp"

using namespace DomoticaInternals;

WiFiUDP udp;
mDNSResolver::Resolver resolver(udp);

// Fill up the map.
enum { REBOOT = 1 };
static std::map<std::string, int> localMap{{"REBOOT", REBOOT}};

DomoticaClass::DomoticaClass() {

  _wifiReconnectCounter = 0;
  _mqttReconnectCounter = 0;
  Interface::get().ready = false;
  Interface::get()._mqttClient = &_mqttClient;
  Interface::get()._logger = &_logger;
  Interface::get()._config = &_config;
  DeviceId::generate();
}

DomoticaClass::~DomoticaClass() {}

void DomoticaClass::setup() {
  DomoticaClass::_setupCallbacks();
  if (isConfigured()) {
    Interface::get().getConfig().log();
    DomoticaClass::_connectWifi();
  }
  if (getConfiguration().ota.enabled) {
    ArduinoOTA.begin();
  }
}

void DomoticaClass::loop() {
  resolver.loop();
  _reconnectTimer.loop();
  if (getConfiguration().ota.enabled) {
    ArduinoOTA.handle();
  }
  // if (!isConnected()) {
  //   if (WiFi.isConnected()) {
  //     //_reconnectTimer.once(3000, std::bind(&DomoticaClass::_connectMqtt,
  //     this));
  //   }
  // }
}

void DomoticaClass::onMqttMessage(callback ptr_reg_callback) {
  printf("inside register_callback\n");
  onMqttMessageCallback = *ptr_reg_callback;
}

void DomoticaClass::_connectWifi() {
  Interface::get().getLogger()
      << F("Connecting to ") << _config.get().wifi.ssid << endl;
  WiFi.hostname(DeviceId::getName());
  WiFi.begin(_config.get().wifi.ssid, _config.get().wifi.password);
}

void DomoticaClass::_onWifiConnect(const WiFiEventStationModeGotIP &event) {
  _reconnectTimer.never(_wifiReconnectTimer);
  _wifiReconnectCounter = 0;
  Interface::get().getLogger() << F("✔  WiFi connected") << endl;
  Interface::get().getLogger() << F("IP Address: ") << WiFi.localIP() << endl;
  _reconnectTimer.queueFunction(std::bind(&DomoticaClass::_connectMqtt, this));
}

void DomoticaClass::_onWifiDisconnect(
    const WiFiEventStationModeDisconnected &event) {
  Interface::get().getLogger() << F("Disconnected from Wi-Fi.") << endl;
  _wifiReconnectTimer =
      _reconnectTimer.once(3000, std::bind(&DomoticaClass::_connectWifi, this));
  Interface::get().getLogger()
      << F("WiFi Reconnect Count:") << _wifiReconnectCounter << endl;
  if (_wifiReconnectCounter++ > WIFI_RECONNECT_MAX_BACKOFF) {
    Interface::get().getLogger() << F("Cannot connect to wifi...") << endl
                                 << F("Rebooting...") << endl;
    ESP.restart();
  };
}

void DomoticaClass::_connectMqtt() {
  _mqttClient.setClientId(DeviceId::getName());
  if ((String(_config.get().mqtt.server.host)).endsWith(".local")) {
    IPAddress ip = resolveName(_config.get().mqtt.server.host);
    _mqttClient.setServer(ip, _config.get().mqtt.server.port);
  } else {
    _mqttClient.setServer(_config.get().mqtt.server.host,
                          _config.get().mqtt.server.port);
  }

  if (_config.get().mqtt.auth) {
    _mqttClient.setCredentials(_config.get().mqtt.username,
                               _config.get().mqtt.password);
  }
  Interface::get().getLogger()
      << F("Connecting to MQTT Broker(") << _config.get().mqtt.server.host
      << F(":") << _config.get().mqtt.server.port << F(")") << endl;

  _mqttClient.connect();
}

void DomoticaClass::_onMqttConnect(bool sessionPresent) {
  _reconnectTimer.never();
  _mqttReconnectCounter = 0;

  Interface::get().ready = true;
  Interface::get().getLogger() << F("✔  Connected to MQTT.") << endl;

  String topic = String(_config.get().mqtt.baseTopic);
  char topic_buffer[MAX_MQTT_TOPIC_LENGTH];
  (topic + "#").toCharArray(topic_buffer, MAX_MQTT_TOPIC_LENGTH);

  getMqttClient().subscribe(topic_buffer, 0);

  topic = String(_config.get().mqtt.baseTopic);
  (topic + _config.get().deviceId)
      .toCharArray(topic_buffer, MAX_MQTT_TOPIC_LENGTH);

  getMqttClient().subscribe(topic_buffer, 0);

  getMqttClient().publish("domotica/connected", 0, true, "true");
  char buf[15];
  WiFi.localIP().toString().toCharArray(buf, 15);
  getMqttClient().publish("domotica/ip", 0, true, buf);
}

void DomoticaClass::_onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  Interface::get().ready = false;
  Interface::get().getLogger() << F("Disconnected from MQTT.") << endl;
  // Serial.println(static_cast<uint8_t>(reason));
  if (WiFi.isConnected()) {
    _mqttReconnectTimer = _reconnectTimer.once(
        3000, std::bind(&DomoticaClass::_connectMqtt, this));
  }
  Interface::get().getLogger()
      << F("MQTT Reconnect Count:") << _mqttReconnectCounter << endl;
  if (_mqttReconnectCounter++ > MQTT_RECONNECT_MAX_BACKOFF) {
    Interface::get().getLogger() << F("Cannot connect to wifi...") << endl
                                 << F("Rebooting...") << endl;
    ESP.restart();
  }
}

void DomoticaClass::_onMqttMessage(char *topic, char *payload,
                                   AsyncMqttClientMessageProperties properties,
                                   size_t len, size_t index, size_t total) {
  Interface::get().getLogger() << F("Message Received:") << endl
                               << topic << F(" --> ") << payload << endl;
  if (*onMqttMessageCallback != NULL) {
    (*onMqttMessageCallback)(topic, payload);
  }

  _checkForEvent(topic, payload);
}

void DomoticaClass::_checkForEvent(char *topic, char *payload) {

  // switch (val)
  // {
  //     case REBOOT:    // distance from focal point to iso center
  //         ESP.restart();
  //         break;
  //     default:      // name is unknown
  //         break;
  // }
}

void DomoticaClass::_onMqttPublish(uint16_t packetId) {
  Interface::get().getLogger() << F("✔  Data Published") << endl;
}

void DomoticaClass::_getConfig() {
  IPAddress ip = resolveName(DEFAULT_HTTP_HOST);
  char hostName[15];
  (ip.toString()).toCharArray(hostName, 15);
  Serial.println(hostName);
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(hostName, httpPort)) {
    Interface::get().getLogger() << F("Connection failed") << endl;
    return;
  }

  String url = "/test/config.json";

  client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + hostName +
               "\r\n" + "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 25000) {
      Interface::get().getLogger() << F(">>> Client timeout !") << endl;
      client.stop();
      return;
    }
  }

  while (client.available()) {
    String line = client.readStringUntil('\r');
    Interface::get().getLogger() << line << endl;
  }
  Interface::get().getLogger() << endl << F("Closing connection") << endl;
  client.stop();
}

IPAddress DomoticaClass::resolveName(const char *name) {
  Interface::get().getLogger() << F("Resolving hostname: ") << name << endl;

  resolver.setLocalIP(WiFi.localIP());
  IPAddress ip = resolver.search(_config.get().mqtt.server.host);
  if (ip != INADDR_NONE) {
    Interface::get().getLogger() << F("Hostname resolved to: ") << ip << endl;
  } else {
    Interface::get().getLogger() << F("Could not resolve hostname") << endl;
  }
  return ip;
}

void DomoticaClass::_setupCallbacks() {
  _wifiConnectHandler = WiFi.onStationModeGotIP(
      std::bind(&DomoticaClass::_onWifiConnect, this, std::placeholders::_1));
  _wifiDisconnectHandler = WiFi.onStationModeDisconnected(std::bind(
      &DomoticaClass::_onWifiDisconnect, this, std::placeholders::_1));
  Interface::get().getMqttClient().onConnect(
      std::bind(&DomoticaClass::_onMqttConnect, this, std::placeholders::_1));
  Interface::get().getMqttClient().onDisconnect(std::bind(
      &DomoticaClass::_onMqttDisconnect, this, std::placeholders::_1));
  Interface::get().getMqttClient().onMessage(std::bind(
      &DomoticaClass::_onMqttMessage, this, std::placeholders::_1,
      std::placeholders::_2, std::placeholders::_3, std::placeholders::_4,
      std::placeholders::_5, std::placeholders::_6));
  Interface::get().getMqttClient().onPublish(
      std::bind(&DomoticaClass::_onMqttPublish, this, std::placeholders::_1));
  ArduinoOTA.onStart([]() { Serial.println("Start"); });
  ArduinoOTA.onEnd([]() { Serial.println("\nEnd"); });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR)
      Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR)
      Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR)
      Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR)
      Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR)
      Serial.println("End Failed");
  });
}

bool DomoticaClass::isConfigured() {
  return Interface::get().getConfig().load();
}

bool DomoticaClass::isConnected() { return Interface::get().ready; }

const ConfigStruct &DomoticaClass::getConfiguration() {
  return Interface::get().getConfig().get();
}

AsyncMqttClient &DomoticaClass::getMqttClient() { return _mqttClient; }

Logger &DomoticaClass::getLogger() { return _logger; }
DomoticaClass Domotica;
