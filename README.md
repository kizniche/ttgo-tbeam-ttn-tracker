## TTGO T-Beam Tracker for The Things Network

Uploads GPS data from the TTGO T-Beam to [The Things Network](https://www.thethingsnetwork.org) (TTN) and [TTN Mapper](https://ttnmapper.org) for tracking and determining signal strength of LoRaWAN gateways and nodes.

#### Based on the code from [xoseperez/ttgo-beam-tracker](https://github.com/xoseperez/ttgo-beam-tracker), with excerpts from [dermatthias/Lora-TTNMapper-T-Beam](https://github.com/dermatthias/Lora-TTNMapper-T-Beam) to fix an issue with incorrect GPS data being transmitted to The Things Network. I also added support for the 915 MHz frequency (North and South America).

This is a LoRaWAN node based on the [TTGO T-Beam](https://github.com/LilyGO/TTGO-T-Beam) development platform using the SSD1306 I2C OLED display.
It uses a RFM95 by HopeRF and the MCCI LoRaWAN LMIC stack. This sample code is configured to connect to The Things Network using the US 915 MHz frequency by default, but can be changed to EU 868 MHz.

To start, install the dependencies and board, below, to your Arduino IDE. Then edit ```src/credentials.h``` to use either ```USE_ABP``` or ```USE_OTAA``` and add the Keys/EUIs for your Application's Device from The Things Network. Add the TTN Mapper integration to your Application (and optionally the Data Storage integration if you want to access the GPS location information yourself), then add the Decoder code, below, if using . Compile the Arduino code and upload it to your TTGO T-Beam. Turn on the device and once a GPS lock is acquired, the device will start sending data to TTN and TTn Mapper.

I also developed The [Things Network Tracker (TTN-Tracker)](https://github.com/kizniche/ttn-tracker), a web app that pulls GPS data from TTN and displays it on a map in real-time (TTN Mapper is not real-time).

#### Arduino IDE Board

Follow the directions at [espressif/arduino-esp32](https://github.com/espressif/arduino-esp32) and use board 'Heltec_WIFI_LoRa_32'.

#### Arduino IDE Library Dependencies

 - [mcci-catena/arduino-lmic](https://github.com/mcci-catena/arduino-lmic)
 - [mikalhart/TinyGPSPlus](https://github.com/mikalhart/TinyGPSPlus)
 - [ThingPulse/esp8266-oled-ssd1306](https://github.com/ThingPulse/esp8266-oled-ssd1306)

#### TTN Decoder

```C
function Decoder(bytes, port) {
    var decoded = {};

    decoded.latitude = ((bytes[0]<<16)>>>0) + ((bytes[1]<<8)>>>0) + bytes[2];
    decoded.latitude = (decoded.latitude / 16777215.0 * 180) - 90;
  
    decoded.longitude = ((bytes[3]<<16)>>>0) + ((bytes[4]<<8)>>>0) + bytes[5];
    decoded.longitude = (decoded.longitude / 16777215.0 * 360) - 180;
  
    var altValue = ((bytes[6]<<8)>>>0) + bytes[7];
    var sign = bytes[6] & (1 << 7);
    if(sign)
    {
        decoded.altitude = 0xFFFF0000 | altValue;
    }
    else
    {
        decoded.altitude = altValue;
    }
  
    decoded.hdop = bytes[8] / 10.0;

    return decoded;
}
```

### The TTGO T-Beam development platform

Note: There are now two versions, the first version (Rev0) and a newer version (Rev1). The GPS module on Rev1 is connected to different pins than Rev0. This code has been successfully tested on REV0, and is in the process of being tested on REV1.

### Rev0

![TTGO T-Beam 01](img/TTGO-TBeam-01.jpg)

![TTGO T-Beam 02](img/TTGO-TBeam-02.jpg)

![TTGO T-Beam 03](img/TTGO-TBeam-03.jpg)

### Rev1

![T-BEAM-Rev1-01](img/T-BEAM-Rev1-01.jpg)

![T-BEAM-Rev1-02](img/T-BEAM-Rev1-02.jpg)
