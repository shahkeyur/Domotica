#include "Config.hpp"

using namespace DomoticaInternals;

Config::Config()
: _configStruct()
, _spiffsBegan(false)
{
}

bool Config::_spiffsBegin() {
  if (!_spiffsBegan) {
    _spiffsBegan = SPIFFS.begin();
    if (!_spiffsBegan) {
      Serial.println("✖ Cannot mount SPIFFS");
    }
  }
  return _spiffsBegan;
}

bool Config::load() {
  if (!SPIFFS.begin()) {
    Serial.println("✖  Failed to mount file system");
    return false;
  }

  Serial.println("✔  SPIFFS Mounted");

  File configFile = SPIFFS.open("/domotica/config.json", "r");
  if (!configFile) {
    Serial.println("Failed to open config file");
    //Serial.println("Downloading config file from default location");
    return false;
  }

  size_t size = configFile.size();
  if (size > 1024) {
    Serial.println("Config file size is too large");
    return false;
  }

  std::unique_ptr<char[]> buf(new char[size]);

  configFile.readBytes(buf.get(), size);

  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& parsedJson = jsonBuffer.parseObject(buf.get());

  if (!parsedJson.success()) {
    Serial.println("✖  Failed to parse config file");
    return false;
  }

  const char* name = DeviceId::getName();
  if (parsedJson.containsKey("name")) {
    name = parsedJson["name"];
  }

  const char* deviceId = DeviceId::getId();
  if (parsedJson.containsKey("device_id")) {
    deviceId = parsedJson["device_id"];
  }

  //get data to temporary variables
  const char* wifi_ssid = DEFAULT_WIFI_SSID;
  const char* wifi_password = DEFAULT_WIFI_PASSWORD;
  if (parsedJson["wifi"].as<JsonObject&>().containsKey("ssid")) {
    wifi_ssid = parsedJson["wifi"]["ssid"];
    wifi_password  = parsedJson["wifi"]["password"];

  }

  const char* mqtt_host = DEFAULT_MQTT_HOST;
  if (parsedJson["mqtt"].as<JsonObject&>().containsKey("host")) {
    mqtt_host = parsedJson["mqtt"]["host"];
  }

  bool auth = false;
  if (parsedJson["mqtt"].as<JsonObject&>().containsKey("auth")) {
    auth = parsedJson["mqtt"]["auth"];
  }

  const char* mqtt_username = "";
  if (parsedJson["mqtt"].as<JsonObject&>().containsKey("username")) {
   mqtt_username = parsedJson["mqtt"]["username"];
  }

  const char* mqtt_password = "";
  if (parsedJson["mqtt"].as<JsonObject&>().containsKey("password")) {
   mqtt_password = parsedJson["mqtt"]["password"];
  }

  strlcpy(_configStruct.name,name,MAX_FRIENDLY_NAME_LENGTH);
  strlcpy(_configStruct.deviceId,deviceId,MAX_FRIENDLY_NAME_LENGTH);

  strlcpy(_configStruct.wifi.ssid, wifi_ssid, MAX_WIFI_SSID_LENGTH);
  if (wifi_password) strlcpy(_configStruct.wifi.password, wifi_password, MAX_WIFI_PASSWORD_LENGTH);

  strlcpy(_configStruct.mqtt.server.host, mqtt_host, MAX_HOSTNAME_LENGTH);

  _configStruct.mqtt.server.port = DEFAULT_MQTT_PORT;
  if (parsedJson["mqtt"].as<JsonObject&>().containsKey("port")) {
    _configStruct.mqtt.server.port = parsedJson["mqtt"]["port"];
  }

  strlcpy(_configStruct.mqtt.baseTopic, DEFAULT_MQTT_BASE_TOPIC, MAX_MQTT_BASE_TOPIC_LENGTH);
  if (parsedJson["mqtt"].as<JsonObject&>().containsKey("baseTopic")) {
    strlcpy(_configStruct.mqtt.baseTopic, parsedJson["mqtt"]["baseTopic"], MAX_MQTT_BASE_TOPIC_LENGTH);
  }

  _configStruct.mqtt.auth = auth;
  strlcpy(_configStruct.mqtt.username, mqtt_username, MAX_MQTT_CREDS_LENGTH);
  strlcpy(_configStruct.mqtt.password, mqtt_password, MAX_MQTT_CREDS_LENGTH);

  if (parsedJson["mqtt"].as<JsonObject&>().containsKey("baseTopic")) {
    strlcpy(_configStruct.mqtt.baseTopic, parsedJson["mqtt"]["baseTopic"], MAX_MQTT_BASE_TOPIC_LENGTH);
  }
  return true;
}
/*
void Config::erase() {
  if (!_spiffsBegin()) { return; }

  SPIFFS.remove(CONFIG_FILE_PATH);
  SPIFFS.remove(CONFIG_NEXT_BOOT_MODE_FILE_PATH);
}
*/
void Config::write(const JsonObject& config) {
  if (!_spiffsBegin()) { return; }

  SPIFFS.remove(CONFIG_FILE_PATH);

  File configFile = SPIFFS.open(CONFIG_FILE_PATH, "w");
  if (!configFile) {
    Interface::get().getLogger() << F("✖ Cannot open config file") << endl;
    return;
  }

  config.printTo(configFile);
  configFile.close();
}

bool Config::patch(const char* patch) {
    if (!_spiffsBegin()) { return false; }

    StaticJsonBuffer<MAX_JSON_CONFIG_ARDUINOJSON_BUFFER_SIZE> patchJsonBuffer;
    JsonObject& patchObject = patchJsonBuffer.parseObject(patch);

    if (!patchObject.success()) {
      Interface::get().getLogger() << F("✖ Invalid or too big JSON") << endl;
      return false;
    }

    File configFile = SPIFFS.open(CONFIG_FILE_PATH, "r");
    if (!configFile) {
      Interface::get().getLogger() << F("✖ Cannot open config file") << endl;
      return false;
    }

    size_t configSize = configFile.size();

    char configJson[MAX_JSON_CONFIG_FILE_SIZE];
    configFile.readBytes(configJson, configSize);
    configFile.close();
    configJson[configSize] = '\0';

    StaticJsonBuffer<MAX_JSON_CONFIG_ARDUINOJSON_BUFFER_SIZE> configJsonBuffer;
    JsonObject& configObject = configJsonBuffer.parseObject(configJson);

    for (JsonObject::iterator it = patchObject.begin(); it != patchObject.end(); ++it) {
      if (patchObject[it->key].is<JsonObject&>()) {
        JsonObject& subObject = patchObject[it->key].as<JsonObject&>();
        for (JsonObject::iterator it2 = subObject.begin(); it2 != subObject.end(); ++it2) {
          if (!configObject.containsKey(it->key) || !configObject[it->key].is<JsonObject&>()) {
            String error = "✖ Config does not contain a ";
            error.concat(it->key);
            error.concat(" object");
            Interface::get().getLogger() << error << endl;
            return false;
          }
          JsonObject& subConfigObject = configObject[it->key].as<JsonObject&>();
          subConfigObject[it2->key] = it2->value;
        }
      } else {
        configObject[it->key] = it->value;
      }
    }
    write(configObject);
    return true;
}

void Config::log() const {
  Interface::get().getLogger() << F("{} Stored configuration") << endl;
  Interface::get().getLogger() << F("  • Hardware device ID: ") << DeviceId::getId() << endl;
  Interface::get().getLogger() << F("  • Device ID: ") << DeviceId::getName() << endl;
  Interface::get().getLogger() << F("  • Name: ") << _configStruct.name << endl;

  Interface::get().getLogger() << F("  • Wi-Fi: ") << endl;
  Interface::get().getLogger() << F("    ◦ SSID: ") << _configStruct.wifi.ssid << endl;
  Interface::get().getLogger() << F("    ◦ Password not shown") << endl;

  Interface::get().getLogger() << F("  • MQTT: ") << endl;
  Interface::get().getLogger() << F("    ◦ Host: ") << _configStruct.mqtt.server.host << endl;
  Interface::get().getLogger() << F("    ◦ Port: ") << _configStruct.mqtt.server.port << endl;
  Interface::get().getLogger() << F("    ◦ Base topic: ") << _configStruct.mqtt.baseTopic << endl;
  Interface::get().getLogger() << F("    ◦ Auth? ") << (_configStruct.mqtt.auth ? F("yes") : F("no")) << endl;
  if (_configStruct.mqtt.auth) {
    Interface::get().getLogger() << F("    ◦ Username: ") << _configStruct.mqtt.username << endl;
    Interface::get().getLogger() << F("    ◦ Password not shown") << endl;
  }

  Interface::get().getLogger() << F("  • OTA: ") << endl;
  Interface::get().getLogger() << F("    ◦ Enabled? ") << (_configStruct.ota.enabled ? F("yes") : F("no")) << endl;

}
