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
bool wifiActive = false;

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

String validateInt(JsonObject obj, String parent, String key, long min, long max)
{
    if (!obj.containsKey(key) || !obj[key].is<long>())
    {
        return "Property '" + parent + "." + key + "' does not exist or is not an integer value";
    }
    long val = obj[key];
    if (val < min || val > max)
    {
        return "Property '" + parent + "." + key + "' (" + String(val, DEC) + ") is outside of allowd range (" +
               String(min, DEC) + " - " + String(max, DEC) + ")";
    }
    return "";
}

String validateString(JsonObject obj, String parent, String key, unsigned int minLength = 0, unsigned int maxLength = 99999, bool allowEmpty = false)
{
    if (!obj.containsKey(key) || !obj[key].is<char *>())
    {
        return "Property '" + parent + "." + key + "' does not exist or is not a string value";
    }
    auto val = String(obj[key].as<char *>());
    if (val.length() < minLength)
    {
        return "Property '" + parent + "." + key + "' (" + String(val.length(), DEC) +
               ") is shorter than the minimum length of " + String(minLength, DEC);
    }
    if (val.length() > maxLength)
    {
        return "Property '" + parent + "." + key + "' (" + String(val.length(), DEC) +
               ") is longer than the maximum length of " + String(maxLength, DEC);
    }
    if (val.isEmpty())
    {
        return "Property '" + parent + "." + key + "' is empty";
    }
    return "";
}

String validateBool(JsonObject obj, String parent, String key)
{
    if (!obj.containsKey(key) || !obj[key].is<bool>())
    {
        return "Property '" + parent + "." + key + " does not exist or is not a boolean value";
    }
    return "";
}

String validateConfigPayload(JsonVariant &json)
{
    //-- Device config
    if (!json.containsKey("system") || !json["system"].is<JsonObject>())
    {
        return "'system' property missing or not an object";
    }
    JsonObject obj = json["system"];
    String err = validateInt(obj, "system", "interval", 5000, 3600000);
    if (err != "")
    {
        return err;
    }
    err = validateInt(obj, "system", "sleepDelay", 100, 60000);
    if (err != "")
    {
        return err;
    }
    err = validateBool(obj, "system", "sleepBetweenMessages");
    if (err != "")
    {
        return err;
    }

    //-- LoRa config
    obj["useAdr"] = config.loraUseADR;
    obj["confirmedEvery"] = config.loraConfirmedEvery;

    if (!json.containsKey("lora") || !json["lora"].is<JsonObject>())
    {
        return "'lora' property missing or not an object";
    }
    obj = json["lora"];
    /*
        From: https://lora-developers.semtech.com/library/tech-papers-and-guides/the-book/the-port-field/

        FPort 0 is reserved for MAC messages. Do not use this port to transport data fields.
        FPort 224 is reserved for MAC compliance testing and
        FPorts 225-255 are reserved for future standardized application extensions.
        This leaves valid port numbers between 1 and 223.
    */
    err = validateInt(obj, "lora", "port", 1, 223);
    if (err != "")
    {
        return err;
    }
    // SF12 = 0; SF7 = 5
    err = validateInt(obj, "lora", "sf", DR_SF12, DR_SF7);
    if (err != "")
    {
        return err;
    }
    err = validateBool(obj, "lora", "useAdr");
    if (err != "")
    {
        return err;
    }
    err = validateInt(obj, "lora", "confirmedEvery", 0, 1000);
    if (err != "")
    {
        return err;
    }

    //-- WiFi config
    if (!json.containsKey("wifi") || !json["wifi"].is<JsonObject>())
    {
        return "'wifi' property missing or not an object";
    }
    obj = json["wifi"];

    err = validateBool(obj, "wifi", "enabled");
    if (err != "")
    {
        return err;
    }
    err = validateString(obj, "wifi", "ssid", 1, 32);
    if (err != "")
    {
        return err;
    }
    err = validateString(obj, "wifi", "psk", 8, 63);
    if (err != "")
    {
        return err;
    }
    return "";
}

void doWriteConfig(JsonVariant &json)
{
    JsonObject obj = json["lora"];
    unsigned short shVal = obj["port"];
    config.loraPort = shVal;
    config.loraSpreadFactor = obj["sf"].as<unsigned char>();
    config.loraUseADR = obj["useAdr"].as<bool>();
    config.loraConfirmedEvery = obj["confirmedEvery"].as<unsigned short>();
    obj = json["wifi"];
    config.wifiEnabled = obj["enabled"].as<bool>();
    auto ssid = String(obj["ssid"].as<char *>());
    auto psk = String(obj["psk"].as<char *>());
    config.wifiSSID = ssid;
    config.wifiPSK = psk;
    obj = json["system"];
    config.sendIntervalMs = obj["interval"].as<unsigned long>();
    config.sleepBetweenMessages = obj["sleepBetweenMessages"].as<unsigned long>();
    config.sleepDelayMs = obj["sleepDelay"].as<unsigned long>();
    config.write();
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

void handleReadConfig(AsyncWebServerRequest *request)
{
    AsyncJsonResponse *response = new AsyncJsonResponse();
    JsonVariant root = response->getRoot();
    auto obj = root.createNestedObject("lora");
    obj["port"] = config.loraPort;
    obj["sf"] = config.loraSpreadFactor;
    obj["useAdr"] = config.loraUseADR;
    obj["confirmedEvery"] = config.loraConfirmedEvery;
    obj = root.createNestedObject("wifi");
    obj["enabled"] = config.wifiEnabled;
    obj["ssid"] = config.wifiSSID;
    obj["psk"] = config.wifiPSK;
    obj = root.createNestedObject("system");
    obj["interval"] = config.sendIntervalMs;
    obj["sleepBetweenMessages"] = config.sleepBetweenMessages;
    obj["sleepDelay"] = config.sleepDelayMs;
    response->setLength();
    request->send(response);
}

void handleWriteConfig(AsyncWebServerRequest *request, JsonVariant &json)
{
    AsyncJsonResponse *response = new AsyncJsonResponse();
    JsonVariant root = response->getRoot();
    bool restart = false;
    String validationError = validateConfigPayload(json);
    if (validationError != "")
    {
        // Validation failed - Send an error response
        response->setCode(400);
        root["error"] = "Config validation failed: " + validationError;
    }
    else
    {
        // Save the changes and then restart the ESP
        screen_print("Got new config");
        doWriteConfig(json);
        restart = true;
    }
    response->setLength();
    request->send(response);
    if (restart)
    {
        screen_print("Config saved.\nRestarting...");
        // We cannot use delay here, so we're doing the waiting ourselves.
        // Just to make sure that the user sees on the display what we're doing
        ostime_t startTime = os_getTime();
        while ((os_getTime() - startTime < ms2osticks(1000)))
        {
            // Idle
        }
        ESP.restart();
    }
}

void handleResetConfig(AsyncWebServerRequest *request)
{
    request->send(204, "text/plain", "");
    screen_print("Resetting config");
    config.reset();
    screen_print("Restarting...");
    ESP.restart();
}

//-- Setup and shutdown ------------------------------------------------------------------------------------------------

// Initialize the SoftAP and the Webserver
void wifiSetup()
{
    screen_print("Initializing WiFi\n");
    WiFi.softAP(
        config.wifiSSID.c_str(),
        config.wifiPSK.c_str());
    IPAddress IP = WiFi.softAPIP();
    String msg = "IP: " + IP.toString() + "\n";
    screen_print(msg.c_str());
    screen_print(("SSID: " + config.wifiSSID + "\n").c_str());

    if (!webserverConfigured)
    {
        server.on("/api/config", HTTP_GET, handleReadConfig);
        server.on("/api/config/reset", HTTP_POST, handleResetConfig);
        auto configHandler = new AsyncCallbackJsonWebHandler("/api/config", handleWriteConfig);
        configHandler->setMethod(HTTP_POST);
        server.addHandler(configHandler);
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
    wifiActive = true;
}

bool wifiEnabled()
{
    return wifiActive;
}

void wifiShutdown()
{
    screen_print("Disabling WiFi\n");
    wifiActive = false;
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
