/*

Webserver module
Simple webserver for configuring the mapper

Copyright (C) 2020 by Stefan Westphal <stefan at westphal dot dev>

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

#include "log.h"
#include <ESPAsyncWebServer.h>
#include <Arduino.h>

//-- Helpers -----------------------------------------------------------------------------------------------------------

String getContentType(String filename);

//-- Requests ----------------------------------------------------------------------------------------------------------

/**
 * Checks in the SPIFFS file system if the requested file does exist - if not, it returns a 404 response
 */
void handleNotFound(AsyncWebServerRequest *request);

//void handleInfo(AsyncWebServerRequest *request);

void handleGetLog(AsyncWebServerRequest *request);

//-- Setup and shutdown ------------------------------------------------------------------------------------------------

/**
 * Initialize the SoftAP and the Webserver
 */
void wifiSetup();

/**
 * Shuts down webserver and SoftAP
 */
void wifiShutdown();

/**
 * Returns if the WiFi mode is currently enabled
 */
bool wifiEnabled();

//-- SSE handling ------------------------------------------------------------------------------------------------------

/**
 * Sends a new log entry as event
 */
void sendLogEvent(logEntry entry);
