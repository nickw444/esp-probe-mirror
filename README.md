# esp-probe-mirror

Currently a work in progress, however this project aims to become a mix of two concepts:

- WiFi Probe sniffer: Listen/collect WiFi probe requests
- WiFi Beacon frame transmitter

Using these two things, the goal of this project is to look for probe frames from devices and re-broadcast them as beacon frames, effectively mirroring all SSID's the user has ever connected to back to them in the WiFi settings menu of their device.


## Related Projects (Code inspiration):

-  [esp8266-probe-sniffer](https://github.com/wojtekka/esp8266-probe-sniffer)
- [esp8266-sniffer](https://github.com/kalanda/esp8266-sniffer)
- [FakeBeaconESP8266](https://github.com/markszabo/FakeBeaconESP8266)
- [ESP8266 and beacon frames
](http://nomartini-noparty.blogspot.com.au/2016/07/esp8266-and-beacon-frames.html)
- [ESP8266 WiFi Beacon Generator ](https://gist.github.com/kost/1e1b13d5796a6649f82ec1c08cd0a835)
- [esp8266_beaconSpam](https://github.com/spacehuhn/esp8266_beaconSpam)
