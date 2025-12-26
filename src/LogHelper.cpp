/**
 * @file LogHelper.cpp
 * @brief Implementation of logging and data processing logic.
 */

#include "LogHelper.h"

String getHistoryJson(int mode, long now) {
    long period = (mode == 1) ? 3600 : (mode == 2) ? 86400 : 2592000;
    long bin = (mode == 1) ? 300 : (mode == 2) ? 3600 : 86400;

    File f = LittleFS.open(LOG_FILE_PATH, "r");
    if (!f) return "[]";

    DynamicJsonDocument doc(4096);
    JsonArray arr = doc.to<JsonArray>();

    float sum = 0;
    int count = 0;
    long lastBin = -1;
    long binTime = 0;

    while (f.available()) {
        String line = f.readStringUntil('\n');
        int commaPos = line.indexOf(',');
        if (commaPos == -1) continue;

        long t = line.substring(0, commaPos).toInt();
        float temp = line.substring(commaPos + 1).toFloat();

        if (t > (now - period)) {
        long currentBin = t / bin;
        if (lastBin == -1) { lastBin = currentBin; binTime = t; }

        if (currentBin != lastBin) {
            if (count > 0) {
            JsonObject obj = arr.createNestedObject();
            DateTime dt((uint32_t)binTime);
            if (mode == 1) obj["l"] = String(dt.hour()) + ":" + (dt.minute() < 10 ? "0" : "") + String(dt.minute());
            else if (mode == 2) obj["l"] = String(dt.hour()) + ":00";
            else obj["l"] = String(dt.day()) + "/" + String(dt.month());
            
            obj["f"] = String(dt.year()) + "/" + String(dt.month()) + "/" + String(dt.day()) + " " + 
                        String(dt.hour()) + ":" + String(dt.minute());
            obj["t"] = sum / count;
            }
            sum = 0; count = 0; lastBin = currentBin; binTime = t;
        }
        sum += temp; count++;
        }
        yield();
    }

    if (count > 0) {
        JsonObject obj = arr.createNestedObject();
        DateTime dt((uint32_t)binTime);
        if (mode == 1) obj["l"] = String(dt.hour()) + ":" + (dt.minute() < 10 ? "0" : "") + String(dt.minute());
        else if (mode == 2) obj["l"] = String(dt.hour()) + ":00";
        else obj["l"] = String(dt.day()) + "/" + String(dt.month());
        
        obj["f"] = String(dt.year()) + "/" + String(dt.month()) + "/" + String(dt.day()) + " " + 
                String(dt.hour()) + ":" + String(dt.minute());
        obj["t"] = sum / count;
    }

    f.close();
    String out;
    serializeJson(doc, out);
    return out;
}

void appendLog(long timestamp, float temp) {
    File f = LittleFS.open(LOG_FILE_PATH, "a");
    if (f) {
        f.printf("%ld,%.2f\n", timestamp, temp);
        f.close();
    }
}

void clearLogFile() {
    LittleFS.remove(LOG_FILE_PATH);
    }

    void getLogStats(int &lines, int &size) {
    lines = 0;
    size = 0;
    File f = LittleFS.open(LOG_FILE_PATH, "r");
    if (f) {
        size = f.size();
        while (f.available()) {
        f.readStringUntil('\n');
        lines++;
        }
        f.close();
    }
}