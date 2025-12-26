/**
 * @file LogHelper.h
 * @brief Helper functions for file system logging and history data generation.
 */

#ifndef LOG_HELPER_H
#define LOG_HELPER_H

#include <Arduino.h>
#include <LittleFS.h>
#include <RTClib.h>
#include <ArduinoJson.h>
#include "AppConfig.h"

/**
 * @brief Generates a JSON string for the history chart.
 * * @param mode Time range mode (1: 1 Hour, 2: 1 Day, 3: 30 Days).
 * @param now Current timestamp.
 * @return String JSON representation of the data.
 */
String getHistoryJson(int mode, long now);

/**
 * @brief Appends a new temperature reading to the log file.
 * * @param timestamp Unix timestamp.
 * @param temp Temperature value.
 */
void appendLog(long timestamp, float temp);

/**
 * @brief Clears the log file.
 */
void clearLogFile();

/**
 * @brief Gets basic file statistics (line count and file size).
 * * @param lines Reference to store line count.
 * @param size Reference to store file size (bytes).
 */
void getLogStats(int &lines, int &size);

#endif // LOG_HELPER_H