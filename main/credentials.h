/*

Credentials file

*/

#pragma once

// Only one of these settings must be defined
#define USE_ABP
//#define USE_OTAA

#ifdef USE_ABP

    // LoRaWAN NwkSKey, network session key
    static const u1_t PROGMEM NWKSKEY[16] = { 0xD1, 0x8A, 0x1F, 0x4D, 0x74, 0x69, 0x46, 0xE9, 0x20, 0x8F, 0x05, 0xE8, 0xFC, 0xB4, 0x5F, 0xC5 };
    // LoRaWAN AppSKey, application session key
    static const u1_t PROGMEM APPSKEY[16] = { 0x42, 0x42, 0xD0, 0xA2, 0xAB, 0xA8, 0x23, 0x93, 0x19, 0x80, 0x9F, 0x90, 0xB3, 0xB4, 0x82, 0x34 };
    // LoRaWAN end-device address (DevAddr)
    // This has to be unique for every node
    static const u4_t DEVADDR = 0x26041F5B;

#endif

#ifdef USE_OTAA

    // This EUI must be in little-endian format, so least-significant-byte
    // first. When copying an EUI from ttnctl output, this means to reverse
    // the bytes. For TTN issued EUIs the last bytes should be 0x00, 0x00,
    // 0x00.
    static const u1_t PROGMEM APPEUI[8]  = { 0x70, 0xB3, 0xD5, 0x7E, 0xD0, 0x01, 0xE9, 0x44 };

    // This should also be in little endian format, see above.
    static const u1_t PROGMEM DEVEUI[8]  = { 0xCB, 0xBE, 0xEB, 0x0A, 0xDD, 0xEA, 0xDB, 0xEE };

    // This key should be in big endian format (or, since it is not really a
    // number but a block of memory, endianness does not really apply). In
    // practice, a key taken from ttnctl can be copied as-is.
    // The key shown here is the semtech default key.
    static const u1_t PROGMEM APPKEY[16] = { 0x0B, 0xF2, 0xAC, 0xB0, 0x6A, 0xFF, 0x27, 0x1E, 0x89, 0x65, 0x7C, 0xBD, 0x99, 0x9F, 0x7F, 0xE9 };

#endif
