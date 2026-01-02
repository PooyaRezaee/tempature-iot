/**
 * @file AppConfig.h
 * @brief Configuration constants, pin definitions, and data structures.
 */

#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#include <Arduino.h>
#include <ESP8266WiFi.h>


// --- Pin Definitions ---
#define PIN_DS18B20      D5
#define PIN_LED_GREEN    D8
#define PIN_LED_YELLOW   D4
#define PIN_LED_RED      D0
#define PIN_TOUCH        D6
#define PIN_BUZZER       D7
#define PIN_CLK_DIS      3  // RX Pin used as GPIO
#define PIN_DIO_DIS      D3
#define SDA_PIN          D2
#define SCL_PIN          D1

// --- Network Configuration ---
const IPAddress LOCAL_IP(10, 71, 74, 100); 
const IPAddress GATEWAY(10, 71, 74, 98);
const IPAddress SUBNET(255, 255, 255, 0);
const IPAddress DNS_SERVER(8, 8, 8, 8);

// --- System Constants ---
const char* const WIFI_SSID = "YOUR_SSID";
const char* const WIFI_PASS = "YOUR_PASSWORD";
const char* const LOG_FILE_PATH = "/log.txt";

/**
 * @brief Configuration structure stored in EEPROM.
 */
struct SystemConfig {
    float greenLimit;
    float yellowLimit;
    bool webSoundEnabled;
};

#endif // APP_CONFIG_H