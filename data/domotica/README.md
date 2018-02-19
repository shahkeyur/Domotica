`data/Domotica` folder
===================

This folder contains the data you can upload to the SPIFFS of your ESP8266.
This is mandatory to connect to wifi and mqtt.

To upload files to the SPIFFS of your device, first create a folder named `data` in your sketch directory. In this `data` folder, create an `domotica` directory. 
Then  create config.json file inside this folder and write your config data.
Finally initiate the [SPIFFS upload process](http://docs.platformio.org/en/stable/platforms/espressif8266.html?highlight=spiffs#uploading-files-to-file-system-spiffs) via PlatformIO, or via the [Arduino IDE](http://esp8266.github.io/Arduino/versions/2.3.0/doc/filesystem.html#uploading-files-to-file-system)

JSON format for config.json
===================
```json
{
  "name": "The kitchen light",
  "device_id": "kitchen-light",
  "wifi": {
    "ssid": "Network_1",
    "password": "secretkey",
  },
  "mqtt": {
    "host": "192.168.1.103",
    "port": 1883,
    "base_topic": "devices/",
    "auth": true,
    "username": "user",
    "password": "pass"
  },
  "ota": {
    "enabled": true
  }
}
```
