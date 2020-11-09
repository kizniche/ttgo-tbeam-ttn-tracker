/*

Log helpers
Internal logging for use in the config website

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
#include "webserver.h"
#include <Arduino.h>
#include <vector>
#include "configuration.h"
#include <ArduinoJson.h>

// Circular buffer for holding the last LOG_BUFFER_SIZE log entries
logEntry logBuffer[LOG_BUFFER_SIZE];
unsigned int logBufferPos = 0;
unsigned int logIdCounter = 0;

void clearLog()
{
    for (unsigned int i = 0; i < LOG_BUFFER_SIZE; i++)
    {
        logBuffer[i] = {0, ""};
    }
    logBufferPos = 0;
    logIdCounter = 0;
}

void log(String entry)
{
    entry.trim();
    Serial.printf("%s\n", entry.c_str());
    logIdCounter++;
    logEntry e = {logIdCounter, entry};
    logBuffer[logBufferPos] = e;
    logBufferPos++;
    if (logBufferPos >= LOG_BUFFER_SIZE)
    {
        // Cycle back to the first entry
        logBufferPos = 0;
    }
    sendLogEvent(e);
}

void log(const char *format, ...)
{
    char loc_buf[64];
    char *temp = loc_buf;
    va_list arg;
    va_list copy;
    va_start(arg, format);
    va_copy(copy, arg);
    int len = vsnprintf(temp, sizeof(loc_buf), format, copy);
    va_end(copy);
    if (len < 0)
    {
        va_end(arg);
        return;
    };
    if (len >= sizeof(loc_buf))
    {
        temp = (char *)malloc(len + 1);
        if (temp == NULL)
        {
            va_end(arg);
            return;
        }
        len = vsnprintf(temp, len + 1, format, arg);
    }
    va_end(arg);
    log(String(temp));
    if (temp != loc_buf)
    {
        free(temp);
    }
}

std::vector<logEntry> getLogEntries(unsigned int startId)
{
    std::vector<logEntry> out;
    unsigned int currentPos = logBuffer[logBufferPos].id == 0 ? 0 : logBufferPos;
    if (logBuffer[currentPos].id != 0)
    {
        do
        {
            if (logBuffer[currentPos].id >= startId)
            {
                out.push_back({logBuffer[currentPos].id, logBuffer[currentPos].text});
            }
            // Next
            currentPos++;
            if (currentPos >= LOG_BUFFER_SIZE)
            {
                currentPos = 0;
            }
        } while (currentPos != logBufferPos);
    }
    return out;
}
