/*

  GPS module

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

#pragma once

#include <Arduino.h>

void gps_time(char *buffer, uint8_t size);

float gps_latitude();

float gps_longitude();

float gps_altitude();

float gps_hdop();

uint8_t gps_sats();

void gps_setup();

void gps_loop();

void buildPacket(uint8_t txBuffer[10]);