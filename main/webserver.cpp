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

#include "webserver.h"
#include "screen.h"
#include "log.h"
#include "credentials.h"
#include "configuration.h"
#include <Arduino.h>
#ifdef ESP32
#include <vector>
#include <FS.h>
#include <SPIFFS.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <AsyncJson.h>

// #include <ArduinoJson.h>
// #include <AsyncJson.h>

const String TYPE_JSON = "application/json";
bool webserverConfigured = false;

// Our internal webserver for the configuration website
AsyncWebServer server(WEBSERVER_PORT);
AsyncEventSource logEventSource("/sse/log");

//-- Helpers -----------------------------------------------------------------------------------------------------------

String getContentType(String filename)
{
    if (filename.endsWith(".html"))
        return "text/html";
    else if (filename.endsWith(".css"))
        return "text/css";
    else if (filename.endsWith(".js"))
        return "application/javascript";
    else if (filename.endsWith(".otf"))
        return "font/opentype";
    return "text/plain";
}

//-- Requests ----------------------------------------------------------------------------------------------------------

// Checks in the SPIFFS file system if the requested file does exist - if not, it returns a 404 response
void handleNotFound(AsyncWebServerRequest *request)
{
    String path = request->url();
    if (path.endsWith("/"))
        path += "index.html";
    if (SPIFFS.exists(path))
    {
        request->send(SPIFFS, path, getContentType(path));
    }
    else
    {
        // Just return a normal HTTP 404 response
        request->send(404, TYPE_JSON, "{\"error\": \"File not found\"}");
    }
}

void handleInfo(AsyncWebServerRequest *request)
{
    AsyncJsonResponse *response = new AsyncJsonResponse();
    JsonVariant root = response->getRoot();
    root["name"] = APP_NAME;
    root["version"] = APP_VERSION;
    response->setLength();
    request->send(response);
}

//-- Setup and shutdown ------------------------------------------------------------------------------------------------

// Initialize the SoftAP and the Webserver
void wifiSetup()
{
    screen_print("Initializing WiFi\n");
    String ssid = DEFAULT_WIFI_SSID;
    String pass = DEFAULT_WIFI_PASSWORD;
    WiFi.softAP(ssid.c_str(), pass.c_str());
    IPAddress IP = WiFi.softAPIP();
    String msg = "IP: " + IP.toString() + "\n";
    screen_print(msg.c_str());

    if (!webserverConfigured)
    {
        server.on("/api/info", handleInfo);
        server.onNotFound(handleNotFound);
        logEventSource.onConnect([](AsyncEventSourceClient *client) {
            unsigned int pos = client->lastId() || 0;
            auto entries = getLogEntries(pos);
            for (auto &i : entries)
            {
                sendLogEvent(i);
            }
        });
        server.addHandler(&logEventSource);
        webserverConfigured = true;
    }

    server.begin();
}

void wifiShutdown()
{
    server.end();
    WiFi.softAPdisconnect();
}

void wifiLoop()
{
}

//-- Sending events ----------------------------------------------------------------------------------------------------

void sendLogEvent(logEntry entry)
{
    logEventSource.send(entry.text.c_str(), "log", entry.id);
}
