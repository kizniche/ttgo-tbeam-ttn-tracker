# TTN MAPPER-TRACKER

### Based on the code from [xoseperez/ttgo-beam-tracker](https://github.com/xoseperez/ttgo-beam-tracker), with excerpts from [dermatthias/Lora-TTNMapper-T-Beam](https://github.com/dermatthias/Lora-TTNMapper-T-Beam) to fix an issue with incorrect GPS data being transmitted to The Things Network. I aslo added support for the US 915 MHz frequency, which is now enabled by default but can easily be changed.

This is a LoRaWAN node based on the [TTGO T-Beam](https://github.com/LilyGO/TTGO-T-Beam) development platform.
It uses a RFM95 by HopeRF and the MCCI LoRaWAN LMIC stack.
The sample code is configured to connect to The Things Network using EU frequency, but you can easily change that.

Arduino Library Dependencies:

[mcci-catena/arduino-lmic](https://github.com/mcci-catena/arduino-lmic)
[mikalhart/TinyGPSPlus](https://github.com/mikalhart/TinyGPSPlus)
[ThingPulse/esp8266-oled-ssd1306](https://github.com/ThingPulse/esp8266-oled-ssd1306)
