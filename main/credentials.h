/*

Credentials file

*/

#pragma once

// Only one of these settings must be defined
#define USE_ABP
//#define USE_OTAA

#ifdef USE_ABP

    // LoRaWAN NwkSKey, network session key
    static const u1_t PROGMEM NWKSKEY[16] = { 0x59, 0xA5, 0xCC, 0xD8, 0xD0, 0x43, 0x20, 0x56, 0xCE, 0xB6, 0x93, 0x3C, 0x97, 0xFF, 0x21, 0xE1 };
    // LoRaWAN AppSKey, application session key
    static const u1_t PROGMEM APPSKEY[16] = { 0x83, 0xF1, 0x0F, 0xE6, 0xFE, 0xB9, 0xBE, 0x37, 0xA2, 0x47, 0x96, 0xAA, 0xB4, 0x8D, 0xF6, 0x61 };
    // LoRaWAN end-device address (DevAddr)
    // This has to be unique for every node
    static const u4_t DEVADDR = 0x26021BCB;

#endif

#ifdef USE_OTAA

    // copy these variables to ../credentials-private.h and customize with your secret appkey.

    // This EUI must be in little-endian format, so least-significant-byte
    // first. When copying an EUI from ttnctl output, this means to reverse
    // the bytes. For TTN issued EUIs the last bytes should be 0x00, 0x00,
    // 0x00.
    static const u1_t PROGMEM APPEUI[8]  =  { 0x20, 0x77, 0x01, 0xD0, 0x7E, 0xD5, 0xB3, 0x70 };

    // This should also be in little endian format, see above.
    // Note: You do not need to set this field, if unset it will be generated automatically based on the device macaddr
    static u1_t DEVEUI[8]  = { 0xB7, 0x0F, 0x8E, 0x33, 0xF9, 0x37, 0x9E, 0x00 };

    // This key should be in big endian format (or, since it is not really a
    // number but a block of memory, endianness does not really apply). In
    // practice, a key taken from ttnctl can be copied as-is.
    // The key shown here is the semtech default key.
    static const u1_t PROGMEM APPKEY[16] = { 0xC4, 0xAB, 0xDA, 0xB6, 0xDC, 0xD6, 0xEC, 0x56, 0xDD, 0xAC, 0x90, 0x67, 0xCE, 0x17, 0x0D, 0x8A };

#endif
