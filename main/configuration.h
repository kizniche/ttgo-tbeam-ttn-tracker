/*

TTGO T-BEAM Tracker for The Things Network

Copyright (C) 2018 by Xose Pérez <xose dot perez at gmail dot com>
Extended 2020 by Stefan Westphal <stefan at westphal dot dev>

This code requires LMIC library by Matthijs Kooijman
https://github.com/matthijskooijman/arduino-lmic

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#pragma once

#include <Arduino.h>
#include <lmic.h>

class DeviceConfig
{
public:
    bool sleepBetweenMessages; // Should the controller sleep between messages
    uint32_t sleepDelayMs;     // Time before going to sleep after a message has been sent

    uint32_t sendIntervalMs; // Time between two messages

    uint8_t loraPort;               // LoRaWAN port number to use when sending a message
    uint16_t loraConfirmedEvery;    // Request a confirmed message every N messages
    unsigned char loraSpreadFactor; // The SF to use for sending out messages
    bool loraUseADR;                // Should ADR be used?

    bool wifiEnabled; // Should the WiFi be enabled on startup?
    String wifiSSID;  // The SSID to use for the SoftAP
    String wifiPSK;   // The pre-shared-key for the SoftAP

    DeviceConfig();

    /**
     * Reads the configuration from the nvs partition
     */
    void read();
    /**
     * Writes the current config to the nvs partition
     */
    void write();
    /**
     * Erases the config from the nvs partition
     */
    void reset();
};

extern DeviceConfig config;

void ttn_register(void (*callback)(uint8_t message));

// -----------------------------------------------------------------------------
// Version
// -----------------------------------------------------------------------------

#define APP_NAME "TTN MAP-TRACK"
#define APP_VERSION "1.2.0"

// -----------------------------------------------------------------------------
// Configuration
// -----------------------------------------------------------------------------

// Select which T-Beam board is being used. Only uncomment one.
// #define T_BEAM_V07  // AKA Rev0 (first board released)
#define T_BEAM_V10 // AKA Rev1 (second board released)

// Select the payload format. Change on TTN as well. Only uncomment one.
#define PAYLOAD_USE_FULL
//#define PAYLOAD_USE_CAYENNE

// If using a single-channel gateway, uncomment this next option and set to your gateway's channel
//#define SINGLE_CHANNEL_GATEWAY  0

// If you are having difficulty sending messages to TTN after the first successful send,
// uncomment the next option and experiment with values (~ 1 - 5)
//#define CLOCK_ERROR             5

#define SERIAL_BAUD 115200 // Serial debug baud rate
#define LOGO_DELAY 5000    // Time to show logo on first boot
#define REQUIRE_RADIO true // If true, we will fail to start if the radio is not found

// If not defined, we will wait for lock forever
#define GPS_WAIT_FOR_LOCK (60 * 1000) // Wait after every boot for GPS lock (may need longer than 5s because we turned the gps off during deep sleep)

// -----------------------------------------------------------------------------
// Default configuration
//
// These values will be used when there is no configuration saved in the NVM
// storage (or when the config is being reset).
// -----------------------------------------------------------------------------

// -- Lora config --------------------------------------------------------------

#define SEND_INTERVAL (20 * 1000) // Sleep for these many millis
#define LORAWAN_PORT 10           // Port the messages will be sent to
#define LORAWAN_CONFIRMED_EVERY 0 // Send confirmed message every these many messages (0 means never)
#define LORAWAN_SF DR_SF7         // Spreading factor (recommended DR_SF7 for ttn network map purposes, DR_SF10 works for slow moving trackers)
#define LORAWAN_ADR 0             // Enable ADR

// -- Device config ------------------------------------------------------------

#define SLEEP_BETWEEN_MESSAGES false // Do sleep between messages
#define MESSAGE_TO_SLEEP_DELAY 5000  // Time after message before going to sleep

// -- Web and WiFi config ------------------------------------------------------

#define WIFI_ENABLED 1            // Enable WiFi?
#define WIFI_SSID "T-Beam Mapper" // Default SSID for the AP
#define WIFI_PSK "changeme"       // Default pre-shared-key for the AP

#define WEBSERVER_PORT 80 // The port the webserver should listen on

// -----------------------------------------------------------------------------
// Custom messages
// -----------------------------------------------------------------------------

#define EV_QUEUED 100
#define EV_PENDING 101
#define EV_ACK 102
#define EV_RESPONSE 103

// -----------------------------------------------------------------------------
// General
// -----------------------------------------------------------------------------

#define I2C_SDA 21
#define I2C_SCL 22

#if defined(T_BEAM_V07)
#define LED_PIN 14
#define BUTTON_PIN 39
#elif defined(T_BEAM_V10)
#define BUTTON_PIN 38
#endif

// -----------------------------------------------------------------------------
// OLED
// -----------------------------------------------------------------------------

#define SSD1306_ADDRESS 0x3C

// -----------------------------------------------------------------------------
// GPS
// -----------------------------------------------------------------------------

#define GPS_SERIAL_NUM 1
#define GPS_BAUDRATE 9600
#define USE_GPS 1

#if defined(T_BEAM_V07)
#define GPS_RX_PIN 12
#define GPS_TX_PIN 15
#elif defined(T_BEAM_V10)
#define GPS_RX_PIN 34
#define GPS_TX_PIN 12
#endif

// -----------------------------------------------------------------------------
// LoRa SPI
// -----------------------------------------------------------------------------

#define SCK_GPIO 5
#define MISO_GPIO 19
#define MOSI_GPIO 27
#define NSS_GPIO 18
#if defined(T_BEAM_V10)
#define RESET_GPIO 14
#else
#define RESET_GPIO 23
#endif
#define DIO0_GPIO 26
#define DIO1_GPIO 33 // Note: not really used on this board
#define DIO2_GPIO 32 // Note: not really used on this board

// -----------------------------------------------------------------------------
// AXP192 (Rev1-specific options)
// -----------------------------------------------------------------------------

// #define AXP192_SLAVE_ADDRESS  0x34 // Now defined in axp20x.h
#define GPS_POWER_CTRL_CH 3
#define LORA_POWER_CTRL_CH 2
#define PMU_IRQ 35

// -----------------------------------------------------------------------------
// Logging
// -----------------------------------------------------------------------------

#define LOG_BUFFER_SIZE 100
