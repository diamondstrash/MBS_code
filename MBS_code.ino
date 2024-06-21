#include "MSB_intern_library.h"

float percentageBattery = 0.0; //Variable to store percentage of battery
float ampereHours = 0.0; // Variable to store the accumulated ampere-hours
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
    Get initial battery capacity
  */
  delay(5000);
  float sampleValue_Volt = getMeanVoltage(500); //Get mean voltage value
  float voltage_sensed_A1 = sampleValue_Volt * 5 / 1023.0; //Convert the Analog Value to a Continu Values
  float batteryVoltage = voltage_sensed_A1 * ((R1+R2)/R2); //Calculate the true voltage value
  percentageBattery = returnInitialPercentage(batteryVoltage); //Calculate initial percentage of the battery
}

void loop() {
  //variables declaration
  float batteryCurrent = 0.0;
  float batteryPower = 0.0;
  
  //Calculate Voltage Value
  float sampleValue_Volt = getMeanVoltage(1500);
  float voltage_sensed_A1 = sampleValue_Volt * 5 / 1023.0; //Convert the Analog Value to a Continu Values
  float batteryVoltage = voltage_sensed_A1 * ((R1+R2)/R2);

  //Calculate Current Value
  float sampleValue = getMeanCurrent(); //Get mean current value
  float current_sensed_A0 = sampleValue * 5 / 1023.0; //Convert the Analog Value to a Continu Values
  batteryCurrent = ( ( current_sensed_A0 -  2.5 ) ) / 0.185; //Calculate the true current value

  // Calculate watt
  batteryPower = batteryCurrent * batteryVoltage;

  // Calculate ampere-hours
  unsigned long currentMillis = millis(); // Get the current time
  unsigned long elapsedTime = currentMillis - previousMillis; // Calculate elapsed time in milliseconds

  // Convert elapsed time to hours and update ampere-hours
  ampereHours += (batteryCurrent * elapsedTime) / (1000.0 * 3600.0); // Convert milliseconds to hours

  // Calculate battery percentage
  percentageBattery = returnPercentage(ampereHours, percentageBattery);

  writeFieldThingSpeakChannel(percentageBattery, batteryVoltage, batteryCurrent, batteryPower);

  // Update previous time
  previousMillis = currentMillis;
}