#ifndef MSB_intern_library_h
#define MSB_intern_library_h

#include <WiFiNINA.h>
#include <Arduino.h>

#define HTTP_Port 80
#define CURRENT_SENSOR_PIN A0
#define VOLTAGE_DIVIDER_PIN A1

extern const char* ssid; // Declaration of WiFi Network's SSID
extern const char* password; // Declaration of WiFi Network's Password
extern const char* username; // Declaration of WiFi Network's Password
extern const float ratio_Voltage; //Declaration of Ratio Voltage
extern const float ratio_Current; //Declaration of Ratio Current
extern const float R1;
extern const float R2;

void clearThingSpeakChannel();
void writeFieldThingSpeakChannel(int percentageBattery, float batteryVoltage, float batteryCurrent, float batteriePower);
float returnInitialPercentage(float batteryVoltage);
float returnPercentage(float ampereHours, float percentageBattery);
float getMeanVoltage(int nbr_itr);
float getMeanCurrent();

#endif