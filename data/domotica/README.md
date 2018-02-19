`data/Domotica` folder
===================

This folder contains the data you can upload to the SPIFFS of your ESP8266.
This is mandatory to connect to wifi and mqtt.

To upload files to the SPIFFS of your device, first create a folder named `data` in your sketch directory. In this `data` folder, create an `domotica` directory. 
Then  create config.json file inside this folder and write your config data.
Finally initiate the [SPIFFS upload process](http://docs.platformio.org/en/stable/platforms/espressif8266.html?highlight=spiffs#uploading-files-to-file-system-spiffs) via PlatformIO, or via the [Arduino IDE](http://esp8266.github.io/Arduino/versions/2.3.0/doc/filesystem.html#uploading-files-to-file-system)
