/*

TTGO T-BEAM Tracker for The Things Network

Copyright (C) 2018 by Xose Pérez <xose dot perez at gmail dot com>

Extended by Stefan Westphal <stefan at westphal dot dev> to support web
configuration

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

#include "configuration.h"
#include "log.h"
#include <Preferences.h>

#define CONFIG_NAMESPACE "t-beam"

DeviceConfig config;

DeviceConfig::DeviceConfig()
{
    sleepBetweenMessages = SLEEP_BETWEEN_MESSAGES;
    sleepDelayMs = MESSAGE_TO_SLEEP_DELAY;
    sendIntervalMs = SEND_INTERVAL;

    loraPort = LORAWAN_PORT;
    loraConfirmedEvery = LORAWAN_CONFIRMED_EVERY;
    loraSpreadFactor = LORAWAN_SF;
    loraUseADR = LORAWAN_ADR;

    wifiEnabled = WIFI_ENABLED;
    wifiSSID = WIFI_SSID;
    wifiPSK = WIFI_PSK;
}

void DeviceConfig::read()
{
    Preferences p;
    log("Reading configuration...");
    p.begin(CONFIG_NAMESPACE);
    sleepBetweenMessages = p.getBool("sleep", SLEEP_BETWEEN_MESSAGES);
    sleepDelayMs = p.getULong("sleepDelay", MESSAGE_TO_SLEEP_DELAY);
    sendIntervalMs = p.getULong("sendInterval", SEND_INTERVAL);

    loraPort = p.getUChar("loraPort", LORAWAN_PORT);
    loraConfirmedEvery = p.getUShort("loraAckEvery", LORAWAN_CONFIRMED_EVERY);
    loraSpreadFactor = p.getUChar("loraSF", LORAWAN_SF);
    loraUseADR = p.getBool("loraADR", LORAWAN_ADR);

    wifiEnabled = p.getBool("wifiEnabled", WIFI_ENABLED);
    wifiSSID = p.getString("wifiSSID", WIFI_SSID);
    wifiPSK = p.getString("wifiPSK", WIFI_PSK);
    log("SleepBetweenMessages: " + String(sleepBetweenMessages, DEC));
    log("sleepDelayMs: " + String(sleepDelayMs, DEC));
    log("sendIntervalMs: " + String(sendIntervalMs, DEC));
    log("loraPort: " + String(loraPort, DEC));
    log("loraConfirmedEvery: " + String(loraConfirmedEvery, DEC));
    log("loraSpreadFactor: " + String(loraSpreadFactor, DEC));
    log("loraUseADR: " + String(loraUseADR, DEC));
    log("wifiEnabled: " + String(wifiEnabled, DEC));
    log("wifiSSID: " + wifiSSID);
    log("wifiPSK: " + wifiPSK);

    p.end();
    log("Config read done...");
}

void DeviceConfig::write()
{
    Preferences p;
    log("Writing configuration...");
    p.begin(CONFIG_NAMESPACE);
    log("SleepBetweenMessages: " + String(sleepBetweenMessages, DEC));
    log("sleepDelayMs: " + String(sleepDelayMs, DEC));
    log("sendIntervalMs: " + String(sendIntervalMs, DEC));
    log("loraPort: " + String(loraPort, DEC));
    log("loraConfirmedEvery: " + String(loraConfirmedEvery, DEC));
    log("loraSpreadFactor: " + String(loraSpreadFactor, DEC));
    log("loraUseADR: " + String(loraUseADR, DEC));
    log("wifiEnabled: " + String(wifiEnabled, DEC));
    log("wifiSSID: " + wifiSSID);
    log("wifiPSK: " + wifiPSK);
    p.putBool("sleep", sleepBetweenMessages);
    p.putULong("sleepDelay", sleepDelayMs);
    p.putULong("sendInterval", sendIntervalMs);

    p.putUChar("loraPort", loraPort);
    p.putUShort("loraAckEvery", loraConfirmedEvery);
    p.putUChar("loraSF", loraSpreadFactor);
    p.putBool("loraADR", loraUseADR);

    p.putBool("wifiEnabled", wifiEnabled);
    p.putString("wifiSSID", wifiSSID);
    p.putString("wifiPSK", wifiPSK);

    p.end();
    log("Config write done...");
}

void DeviceConfig::reset()
{
    Preferences p;
    p.begin(CONFIG_NAMESPACE);
    p.clear();
    p.end();
}