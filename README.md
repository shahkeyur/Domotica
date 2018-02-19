# Domotica for ESP8266

An Arduino for ESP8266 implementation of [Domotica](https://github.com/shahkeyur/Domotica), an MQTT convention for the IoT.

## Using with PlatformIO

[PlatformIO](http://platformio.org) is an open source ecosystem for IoT development with cross platform build system, library manager and full support for Espressif ESP8266 development. It works on the popular host OS: Mac OS X, Windows, Linux 32/64, Linux ARM (like Raspberry Pi, BeagleBone, CubieBoard).

1. Install [PlatformIO IDE](http://platformio.org/platformio-ide)
2. Create new project using "PlatformIO Home > New Project"
3. Open [Project Configuration File `platformio.ini`](http://docs.platformio.org/page/projectconf.html)
4. Add "Domotica" to project using `platformio.ini` and [lib_deps](http://docs.platformio.org/page/projectconf/section_env_library.html#lib-deps) option:
```ini
[env:myboard]
platform = espressif8266
board = ...
framework = arduino
lib_deps = Domotica
```

Happy coding with PlatformIO!

## Features

* Automatic connection/reconnection to Wi-Fi/MQTT
* [JSON configuration](https://github.com/shahkeyur/Domotica/tree/master/data/domotica) file to configure the device
* Automatic hostname resolution for MQTT broker
* [OTA support](http://docs.platformio.org/en/latest/platforms/espressif8266.html#over-the-air-ota-update)
* Available in the [PlatformIO registry](https://platformio.org/lib/show/2980/Domotica)
* Easy to use

```c++
#include <Domotica.h>

void setup(){
  Serial.begin(74880);
  Domotica.setup();
}

void loop(){
  Domotica.loop();
}

```

## Donate

I am a student and I will continue this project even without donations. If you like my work and you can help me. :smile:

[![Donate button](https://www.paypal.com/en_US/i/btn/btn_donateCC_LG.gif)](https://www.paypal.me/keyur5156)
