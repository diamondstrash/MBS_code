#include "MSB_intern_library.h"

float percentageBattery = 0.0; //Variable to store percentage of battery
unsigned long previousMillis = 0; // Variable to store the previous time

void setup() {
  Serial.begin(9600); //set the data rate to 9600 bits per second
  while (!Serial); // wait until serial operational
  
  Serial.println("\nStarting Wi-Fi connection...");

  // Attempt to connect to Wi-Fi network:
  int status = WiFi.beginEnterprise(ssid, username, password);

  // Check if the connection attempt failed:
  if (status!= WL_CONNECTED) {
    Serial.println("Connection to Wi-Fi failed!");
    return;
  }

  // Connected successfully:
  Serial.print("Connected to Wi-Fi\n");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP()); // Print the IP address

  clearThingSpeakChannel(); //clear ThingSpeak Channel
  
  /*
    Get initial battery percentage
  */
  delay(5000);
  float sampleValue_Volt = getMeanVoltage(500); //Get mean voltage value
  float batteryVoltage = sampleValue_Volt * ((R1+R2)/R2) * ratio_Voltage; //Calculate the "true" voltage value
  percentageBattery = returnInitialPercentage(batteryVoltage); //Calculate initial percentage of the battery
}

void loop() {
  
  //Calculate Voltage Value
  float sampleValue_Volt = getMeanVoltage(1500); //Get mean voltage value
  float batteryVoltage = sampleValue_Volt * ((R1+R2)/R2) * ratio_Voltage; //Calculate the "true" current value

  //Calculate Current Value
  float sampleValue_Current = getMeanCurrent(); //Get mean current value
  float batteryCurrent = ( ( sampleValue_Current -  2.5 ) * ratio_Current ) / 0.185; //Calculate the "true" current value

  // Calculate watt
  float batteryPower = batteryCurrent * batteryVoltage; //calculate instant power

  // Calculate ampere-hours
  unsigned long currentMillis = millis(); // Get the current time
  unsigned long elapsedTime = currentMillis - previousMillis; // Calculate elapsed time in milliseconds

  // Convert elapsed time to hours and update ampere-hours
  float ampereHours = (batteryCurrent * elapsedTime) / (1000.0 * 3600.0); // Convert milliseconds to hours

  // Calculate battery percentage
  percentageBattery = returnPercentage(ampereHours, percentageBattery);

  //Send HTTP POST Request:
  writeFieldThingSpeakChannel(percentageBattery, batteryVoltage, batteryCurrent, batteryPower);

  // Update previous time
  previousMillis = currentMillis;
}