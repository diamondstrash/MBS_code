#ifndef MSB_intern_library_h
#define MSB_intern_library_h

#include <WiFiNINA.h>
#include <Arduino.h>

#define HTTP_Port 80
#define CURRENT_SENSOR_PIN A0
#define VOLTAGE_DIVIDER_PIN A1
#define VOLTAGE_REF 5 // Reference voltage in Volts (5 Volts)
#define VOLTAGE_MAXIMUM_BATTERY 12 // Reference Maximum Voltage Battery in Volts (12 Volts)

extern const char* ssid; // Declaration of WiFi Network's SSID
extern const char* password; // Declaration of WiFi Network's Password
extern const char* username; // Declaration of WiFi Network's Password
extern const float ratio_Voltage; //Declaration of Ratio Voltage
extern const float ratio_Current; //Declaration of Ratio Current
extern const float R1;
extern const float R2;

void printBatteryPercentage(int percentageBattery);
void printBatteryVoltage(float batteryVoltage);
void printBatteryCurrent(float batteryCurrent);
void printBatteryAmpereHours(float ampereHours);
void printBatteryPower(float batteryPower);
void clearThingSpeakChannel();
void writeFieldThingSpeakChannel(int percentageBattery, float batteryVoltage, float batteryCurrent, float batteriePower);
float returnInitialPercentage(float batteryVoltage);
float returnPercentage(float ampereHours, float percentageBattery);
float getMeanCurrent();

#endif