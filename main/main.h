/*

  Main module

  # Modified by Kyle T. Gabriel to fix issue with incorrect GPS data for TTNMapper

  Copyright (C) 2018 by Xose Pérez <xose dot perez at gmail dot com>

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

#include <Arduino.h>
#include "axp20x.h"

extern bool ssd1306_found;
extern bool axp192_found;

extern AXP20X_Class axp;

extern bool pmu_irq;
extern String baChStatus;

/**
 * If we have a valid position send it to the server.
 * @return true if we decided to send.
 */
bool trySend();

void doDeepSleep(uint64_t msecToWake);

void sleep();

void callback(uint8_t message);

void scanI2Cdevice(void);

void buildPacket(uint8_t txBuffer[]);

/**
 * Init the power manager chip
 *
 * axp192 power
    DCDC1 0.7-3.5V @ 1200mA max -> OLED // If you turn this off you'll lose comms to the axp192 because the OLED and the axp192 share the same i2c bus, instead use ssd1306 sleep mode
    DCDC2 -> unused
    DCDC3 0.7-3.5V @ 700mA max -> ESP32 (keep this on!)
    LDO1 30mA -> charges GPS backup battery // charges the tiny J13 battery by the GPS to power the GPS ram (for a couple of days), can not be turned off
    LDO2 200mA -> LORA
    LDO3 200mA -> GPS
 */
void axp192Init();

/**
 * Perform power on init that we do on each wake from deep sleep
 */
void initDeepSleep();

/**
 * Arduino setup routine
 */
void setup();

/**
 * Arduino loop routine
 */
void loop();