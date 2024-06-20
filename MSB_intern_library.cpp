#include "MSB_intern_library.h"

const char* ssid = "eduroam"; // WiFi Network's SSID
const char* password = "heimaeM4ja";  //WiFi Network's Password
const char* username = "fortina";  //WiFi Network's Username
const char* server = "api.thingspeak.com"; //API website
const char* APIKey = "2TP37RKYAJT1A1ID"; //User API Key
const char* writeAPIKey = "Z6OJTDDYVTCEU4XV"; //Write API Key (used to update field in ThingSpeak)
const char* channelID = "2558267"; //channel ID
const float ratio_Voltage = 0.9658; //ratio to apply in order to have more accurate values for the voltage
const float ratio_Current = 0.9790; //ratio to apply in order to have more accurate values for the current
const float R1 = 820000.0; //value of resistor R1 for the voltage divider bridge
const float R2 = 430000.0; //value of resistor R2 for the voltage divider bridge
const float totalCapacity = 80.0; //The total capacity of the battery is 80 Ah (Ampere Hour)

WiFiClient client;

/*
  Return the approximate initial percentage of the battery.
  Note if the percentage is above 100%, the percentage is automatically adjust to 100%.
  Note if the percentage is below 0%, the percentage is automatically adjust to 0%.
*/
float returnInitialPercentage(float batteryVoltage){
  if(batteryVoltage <= 10.5){
    return 0.0;
  }else if(batteryVoltage >= 12.6){
    return 100.0;
  }else if(batteryVoltage >= 11.62 & batteryVoltage < 12.6){
    return ( ( batteryVoltage - 11.51) / (1.09) ) * 100; // (12.6-11.51) = 1.09
  }else{
    return ( ( batteryVoltage - 10.5) / (1.01) ) * 10; // (11.51-10.5) = 1.01
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

/*
  Return mean current:
    Because the current sensor is not very precise, it is prefer to return an average value for the current
    in order to send a more precise current. The average is done with 1500 values
*/
float getMeanVoltage(int nbr_itr){
  float sampleValue = 0.0;
  int analogVolt_A1 = 0;
  for(int i=0; i<nbr_itr; i++){
    analogVolt_A1 = analogRead(VOLTAGE_DIVIDER_PIN); //Get Voltage Analog Value
    sampleValue += analogVolt_A1;
    delay(10);
  }
  sampleValue = sampleValue/nbr_itr;
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