/**
 * @file main.cpp
 * @brief Main controller for Temperature Monitoring System.
 * @details Handles hardware initialization, loop logic, and web server routing.
 */

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <LittleFS.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <TM1637Display.h>
#include <Wire.h>
#include <RTClib.h>

#include "AppConfig.h"
#include "WebPages.h"
#include "LogHelper.h"

// --- Global Objects ---
OneWire oneWire(PIN_DS18B20);
DallasTemperature sensors(&oneWire);
TM1637Display display(PIN_CLK_DIS, PIN_DIO_DIS);
ESP8266WebServer server(80);
RTC_DS3231 rtc;

// --- Global Variables ---
SystemConfig config;
float currentTemp = 0.0;
unsigned long lastTempRead = 0;
unsigned long lastLogFile = 0;
unsigned long buzzerTimer = 0;
bool buzzerState = false;
bool showTempOnDisplay = true;

void loadConfig() {
  EEPROM.begin(512);
  EEPROM.get(0, config);
  if (isnan(config.greenLimit)) { 
    config.greenLimit = 25.0; 
    config.yellowLimit = 30.0;
    config.webSoundEnabled = true;
  }
}

void saveConfig() {
  EEPROM.put(0, config);
  EEPROM.commit();
}

void setup() {
  pinMode(PIN_CLK_DIS, OUTPUT);
  pinMode(PIN_LED_GREEN, OUTPUT);
  pinMode(PIN_LED_YELLOW, OUTPUT);
  pinMode(PIN_LED_RED, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_TOUCH, INPUT);

  // Peripherals Init
  Wire.begin(SDA_PIN, SCL_PIN);
  rtc.begin();
  loadConfig();
  LittleFS.begin();
  sensors.begin();
  display.setBrightness(0x0f);
  
  // WiFi Setup
  if (!WiFi.config(LOCAL_IP, GATEWAY, SUBNET, DNS_SERVER)) {
    // Fallback or error handling if needed
  }
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  // --- Web Server Routes ---
  server.on("/", [](){ server.send_P(200, "text/html", INDEX_HTML); });
  server.on("/limits", [](){ server.send_P(200, "text/html", LIMITS_HTML); });
  server.on("/set_time", [](){ server.send_P(200, "text/html", SET_TIME_HTML); });
  server.on("/history", [](){ server.send_P(200, "text/html", HISTORY_HTML); });

  server.on("/status", [](){
    DateTime now = rtc.now();
    int lines, sz;
    getLogStats(lines, sz);
    
    char buf[512];
    sprintf(buf, "{\"temp\":%.1f,\"time\":\"%02d:%02d:%02d\",\"date\":\"%04d/%02d/%02d\",\"green\":%.1f,\"yellow\":%.1f,\"webSound\":%s,\"logCount\":%d,\"logSize\":%d}",
            currentTemp, now.hour(), now.minute(), now.second(), now.year(), now.month(), now.day(),
            config.greenLimit, config.yellowLimit, config.webSoundEnabled?"true":"false", lines, sz/1024);
    server.send(200, "application/json", buf);
  });

  server.on("/get_limits", [](){
    char buf[60];
    sprintf(buf, "{\"green\":%.1f,\"yellow\":%.1f}", config.greenLimit, config.yellowLimit);
    server.send(200, "application/json", buf);
  });

  server.on("/save_limits", [](){
    if(server.hasArg("green") && server.hasArg("yellow")){
      config.greenLimit = server.arg("green").toFloat();
      config.yellowLimit = server.arg("yellow").toFloat();
      saveConfig();
    }
    server.send(200, "text/plain", "OK");
  });

  server.on("/toggle_sound", [](){
    config.webSoundEnabled = !config.webSoundEnabled;
    saveConfig();
    server.send(200, "text/plain", "OK");
  });

  server.on("/toggle_disp", [](){
    showTempOnDisplay = !showTempOnDisplay;
    server.send(200, "text/plain", "OK");
  });

  server.on("/clear_log", [](){
    clearLogFile();
    server.send(200, "text/plain", "OK");
  });

  server.on("/save_time", [](){
    if(server.hasArg("dt")){
      String dt = server.arg("dt");
      int y = dt.substring(0,4).toInt();
      int m = dt.substring(5,7).toInt();
      int d = dt.substring(8,10).toInt();
      int hr = dt.substring(11,13).toInt();
      int mn = dt.substring(14,16).toInt();
      rtc.adjust(DateTime(y, m, d, hr, mn, 0));
    }
    server.send(200, "text/html", "<html><body style='font-family:tahoma;text-align:center;padding:50px'><h2>OK</h2><br><a href='/'>Back</a></body></html>");
  });

  server.on("/get_hist", [](){
      int mode = server.arg("m").toInt();
      long now = rtc.now().unixtime();
      String json = getHistoryJson(mode, now);
      server.send(200, "application/json", json);
  });

  server.begin();
}

void loop() {
  server.handleClient();
  unsigned long nowMs = millis();

  // Read Sensors & Update Display
  if (nowMs - lastTempRead >= 1000) {
    lastTempRead = nowMs;
    sensors.requestTemperatures();
    float t = sensors.getTempCByIndex(0);
    if(t > -50 && t < 100) currentTemp = t;
    
    if(showTempOnDisplay) {
      display.showNumberDecEx((int)(currentTemp * 100), 0b01000000, true);
    } else {
      DateTime now = rtc.now();
      display.showNumberDecEx((now.hour() * 100) + now.minute(), 0b01000000, true);
    }

    // Update Status LEDs
    digitalWrite(PIN_LED_GREEN, currentTemp <= config.greenLimit);
    digitalWrite(PIN_LED_YELLOW, currentTemp > config.greenLimit && currentTemp <= config.yellowLimit);
    digitalWrite(PIN_LED_RED, currentTemp > config.yellowLimit);
    
    // Handle Buzzer Logic
    bool touchActive = (digitalRead(PIN_TOUCH) == HIGH);
    bool shouldBeep = (currentTemp > config.yellowLimit) && touchActive && config.webSoundEnabled;
    
    if(shouldBeep) {
      if(nowMs - buzzerTimer >= 200) {
        buzzerTimer = nowMs;
        buzzerState = !buzzerState;
        if(buzzerState) {
          analogWrite(PIN_BUZZER, 128);
          tone(PIN_BUZZER, 1000, 100);
        } else {
          analogWrite(PIN_BUZZER, 128);
          tone(PIN_BUZZER, 1500, 100);
        }
      }
    } else {
      noTone(PIN_BUZZER);
      analogWrite(PIN_BUZZER, 0);
      digitalWrite(PIN_BUZZER, LOW);
    }
  }

  // Logger Logic
  if (nowMs - lastLogFile >= 60000) {
    lastLogFile = nowMs;
    appendLog(rtc.now().unixtime(), currentTemp);
  }
}