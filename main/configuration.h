/*

TTGO-BEAM based TTN Tracker

Copyright (C) 2018 by Xose Pérez <xose dot perez at gmail dot com>

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
void ttn_register(void (*callback)(uint8_t message));

// -----------------------------------------------------------------------------
// Version
// -----------------------------------------------------------------------------

#define APP_NAME                "TTN MAP-TRACK"
#define APP_VERSION             "1.1.0"

// -----------------------------------------------------------------------------
// Configuration
// -----------------------------------------------------------------------------

#define DEBUG_PORT              Serial      // Serial debug port
#define SERIAL_BAUD             115200      // Serial debug baud rate
#define SLEEP_BETWEEN_MESSAGES  0           // Do sleep between messages
#define SEND_INTERVAL           10000       // Sleep for these many millis
#define MESSAGE_TO_SLEEP_DELAY  5000        // Time after message before going to sleep
#define LOGO_DELAY              5000        // Time to show logo on first boot
#define LORAWAN_PORT            10          // Port the messages will be sent to
#define LORAWAN_CONFIRMED_EVERY 0           // Send confirmed message every these many messages (0 means never)
#define LORAWAN_SF              DR_SF7      // Spreading factor
#define LORAWAN_ADR             0           // Enable ADR
#define GPS_WAIT_FOR_LOCK       5000        // Wait 5s after every boot for GPS lock

// Only enable one
//#define PAYLOAD_USE_CAYENNE
#define PAYLOAD_USE_FULL

// -----------------------------------------------------------------------------
// DEBUG
// -----------------------------------------------------------------------------

#ifdef DEBUG_PORT
#define DEBUG_MSG(...) DEBUG_PORT.printf( __VA_ARGS__ )
#else
#define DEBUG_MSG(...)
#endif

// -----------------------------------------------------------------------------
// Custom messages
// -----------------------------------------------------------------------------

#define EV_QUEUED       100
#define EV_PENDING      101
#define EV_ACK          102
#define EV_RESPONSE     103

// -----------------------------------------------------------------------------
// General
// -----------------------------------------------------------------------------

#define BUTTON_PIN      39
#define LED_PIN         14

// -----------------------------------------------------------------------------
// OLED
// -----------------------------------------------------------------------------

#define OLED_ADDRESS    0x3C
#define OLED_SDA        21
#define OLED_SCL        22

// -----------------------------------------------------------------------------
// GPS
// -----------------------------------------------------------------------------

#define GPS_SERIAL_NUM  1
#define GPS_RX_PIN      12
#define GPS_TX_PIN      15
#define GPS_BAUDRATE    9600
#define USE_GPS         1

// -----------------------------------------------------------------------------
// LoRa SPI
// -----------------------------------------------------------------------------

#define SCK_GPIO        5
#define MISO_GPIO       19
#define MOSI_GPIO       27
#define NSS_GPIO        18
#define RESET_GPIO      23
#define DIO0_GPIO       26
#define DIO1_GPIO       33
#define DIO2_GPIO       32
