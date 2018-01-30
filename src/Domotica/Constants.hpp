#pragma once

#include <ESP8266WiFi.h>

namespace DomoticaInternals {
  const char DOMOTICA_VERSION[] = "1.0.0";
  const char DOMOTICA_ESP8266_VERSION[] = "1.0.0";

  const uint8_t WIFI_RECONNECT_MAX_BACKOFF = 6;
  const char DEFAULT_WIFI_SSID[] = "Domotica";
  const char DEFAULT_WIFI_PASSWORD[] = "Domotica";

  const char DEFAULT_HTTP_HOST[] = "sksmarthome.local";
  const char DEFAULT_HTTP_PATH[] = "/esp/config.json";

  const IPAddress ACCESS_POINT_IP(192, 168, 123, 1);

  const char  DEFAULT_MQTT_HOST[] = "sksmarthome.local";
  const uint16_t DEFAULT_MQTT_PORT = 1883;
  const char DEFAULT_MQTT_BASE_TOPIC[] = "domotica/";

  const uint8_t DEFAULT_RESET_PIN = 0;  // == D3 on nodeMCU
  const uint8_t DEFAULT_RESET_STATE = LOW;
  const uint16_t DEFAULT_RESET_TIME = 5 * 1000;

  const char DEFAULT_BRAND[] = "Domotica";

  const uint16_t CONFIG_SCAN_INTERVAL = 20 * 1000;
  const uint32_t STATS_SEND_INTERVAL = 1 * 60 * 1000;
  const uint16_t MQTT_RECONNECT_INITIAL_INTERVAL = 1000;
  const uint8_t MQTT_RECONNECT_MAX_BACKOFF = 6;

  const float LED_WIFI_DELAY = 1;
  const float LED_MQTT_DELAY = 0.2;
  const float LED_DEBUG_PIN = LED_BUILTIN;


  const char CONFIG_UI_BUNDLE_PATH[] = "/domotica/ui_bundle.gz";
 // const char CONFIG_NEXT_BOOT_MODE_FILE_PATH[] = "/domotica/NEXTMODE";
  const char CONFIG_FILE_PATH[] = "/domotica/config.json";
}
