#include "MSB_intern_library.h"

const char* ssid = "[YOUR SSID NETWORK]"; // WiFi Network's SSID
const char* password = "[NETWORK PASSWORD]";  //WiFi Network's Password
const char* username = "[NETWORK USERNAME]";  //WiFi Network's Username
const char* server = "api.thingspeak.com"; //API website
const char* APIKey = "[USER API KEY]"; //User API Key
const char* writeAPIKey = "[WRITE API KEY]"; //Write API Key (used to update field in ThingSpeak)
const char* channelID = "[CHANNEL ID]"; //channel ID
const float ratio_Voltage = 0.9658; //ratio to apply in order to have more accurate values for the voltage
const float ratio_Current = 0.9790; //ratio to apply in order to have more accurate values for the current
const float R1 = 820000.0; //value of resistor R1 for the voltage divider bridge
const float R2 = 430000.0; //value of resistor R2 for the voltage divider bridge
const float totalCapacity = 80.0; //The total capacity of the battery is 80 Ah (Ampere Hour)

WiFiClient client;

/* 
  Print out Battery Voltage on Serial 
*/
void printBatteryPercentage(int percentageBattery){
  Serial.print("Battery Percentage: ");
  Serial.print(percentageBattery);
  Serial.print("%\n");
}

/*
  Print out Load Voltage on Serial 
*/
void printBatteryVoltage(float batteryVoltage){
  Serial.print("Battery Voltage: ");
  Serial.print(batteryVoltage);
  Serial.print("V\n");
}

/*
  Print out Battery Current on Serial
*/
void printBatteryCurrent(float batteryCurrent){
  Serial.print("Current: ");
  Serial.print(batteryCurrent);
  Serial.print("A\n");
}

/*
  Print out Battery Power on Serial
*/
void printBatteryPower(float batteryPower){
  Serial.print("Power: ");
  Serial.print(batteryPower);
  Serial.print("W\n");
}

/*
  Print out Load Voltage on Serial 
*/
void printBatteryAmpereHours(float ampereHours){
  Serial.print("Ampere-hours: ");
  Serial.print(ampereHours); // Print the accumulated ampere-hours
  Serial.print("Ah\n");
}

/*
  Return the approximate initial percentage of the battery.
  Note if the percentage is above 100%, the percentage is automatically adjust to 100%.
*/
float returnInitialPercentage(float batteryVoltage){
  if(batteryVoltage <= 10.5){
    return 0.0;
  }else if(batteryVoltage >= 12.6){
    return 100.0;
  }else{
    return ( ( batteryVoltage - 11.51) / (1.22) ) * 100; // (12.73-11.51) = 1.22
  }
}

/*
  Return the percentage of the battery.
  Note: 
    - if the percentage is above 100%, the percentage is automatically adjust to 100%.
    - if the percentage is below 0%, the percentage is automatically adjust to 0%.
*/
float returnPercentage(float ampereHours, float percentageBattery){
  float percentage = ( ( ( (percentageBattery / 100.0) * totalCapacity ) + ampereHours) / totalCapacity ) * 100;
  if(percentage <= 0.0){
    return 0.0;
  }else if(percentage >= 100.0){
    return 100.0;
  }else{
    return percentage;
  }
}

/*
  Return mean current:
    Because the current sensor is not very precise, it is prefer to return an average value for the current
    in order to send a more precise current. The average is done with 1500 values
*/
float getMeanCurrent(){
  float sampleValue = 0.0;
  int analogVolt_A0 = 0;
  for(int i=0; i<4500; i++){
    analogVolt_A0 = analogRead(CURRENT_SENSOR_PIN); //Get Voltage Analog Value
    sampleValue += analogVolt_A0;
    delay(10);
  }
  sampleValue = sampleValue/4500.0;
  return sampleValue;
}

void clearThingSpeakChannel() {
  // Send HTTP DELETE request:
  if (client.connect(server, HTTP_Port)) {
    String url = "/channels/";
    url += channelID;
    url += "/feeds.json";
    String body = String("api_key=") + APIKey;

    client.print(String("DELETE ") + url + " HTTP/1.1\r\n" +
               "Host: " + server + "\r\n" +
               "Content-Type: application/x-www-form-urlencoded\r\n" +
               "Content-Length: " + body.length() + "\r\n" +
               "Connection: close\r\n\r\n" +
               body);
    Serial.print("Clear Successfully :)\n");
  }
}

/*
  Send HTTP POST Request:
    Argument : battery percentage, battery voltage, battery current & battery power
*/
void writeFieldThingSpeakChannel(int percentageBattery, float batteryVoltage, float batteryCurrent, float batteryPower){
  //Send HTTP POST request:
  if (client.connect(server, HTTP_Port)) {
    String postStr = writeAPIKey;
    postStr += "&field1=";
    postStr += String(percentageBattery);
    postStr += "&field2=";
    postStr += String(batteryVoltage);
    postStr += "&field3=";
    postStr += String(batteryCurrent);
    postStr += "&field4=";
    postStr += String(batteryPower);
    postStr += "\r\n\r\n\r\n";
 
    client.print("POST /update HTTP/1.1\n");
    delay(100);
    client.print("Host: api.thingspeak.com\n");
    delay(100);
    client.print("Connection: close\n");
    delay(100);
    client.print(String("X-THINGSPEAKAPIKEY: ") + writeAPIKey + "\n");
    delay(100);
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    delay(100);
    client.print("Content-Length: ");
    delay(100);
    client.print(postStr.length());
    delay(100);
    client.print("\n\n\n\n");
    delay(100);
    client.print(postStr);
    delay(100);
    client.stop();
    Serial.print("Sent Successfully :)\n");
    delay(100);
  }
}