/*

SSD1306 - Screen module

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

void _screen_header();

void screen_show_logo();

void screen_off();

void screen_on();

void screen_clear();

void screen_print(const char *text, uint8_t x, uint8_t y, uint8_t alignment);

void screen_print(const char *text, uint8_t x, uint8_t y);

void screen_print(const char *text);

void screen_update();

void screen_setup();

void screen_loop();

void screen_invert();