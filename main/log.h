
#pragma once

#include <Arduino.h>

/**
 * A single log entry
 */
struct logEntry
{
    unsigned int id;
    String text;
};

/**
 * Clears the log buffer ans resets all internal counters
*/
void clearLog();

/**
 * Logs a new entry to the log's circular buffer
 */
void log(String entry);
void log(const char *format, ...);

/**
 * Returns a list of log entries starting with the one having the given startId
 */
std::vector<logEntry> getLogEntries(unsigned int startId);