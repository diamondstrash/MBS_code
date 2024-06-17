#include "MSB_intern_library.h"

int analogVolt_A1 = 0; //Variable to store bridge divider voltage output (0-1023)
float percentageBattery = 0.0; //Variable to store percentage of battery
float voltage_sensed_A1 = 0.0; //Variable to store bridge divider voltage output (continue value)
float batteryVoltage = 0.0; //Variable to store battery voltage
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
  delay(120000);
  analogVolt_A1 = analogRead(VOLTAGE_DIVIDER_PIN); //Get Voltage Analog Value
  voltage_sensed_A1 = analogVolt_A1 * 5 / 1023.0; //Convert the Analog Value to a Continu Values
  batteryVoltage = voltage_sensed_A1 * ((R1+R2)/R2); //Calculate the true voltage value
  percentageBattery = returnInitialPercentage(batteryVoltage); //Calculate initial percentage of the battery
  printBatteryVoltage(voltage_sensed_A1);
  printBatteryVoltage(batteryVoltage);
  printBatteryPercentage(percentageBattery);
}

void loop() {
  //variables declaration
  float batteryCurrent = 0.0;
  float batteryPower = 0.0;
  
  //Calculate Voltage Value
  analogVolt_A1 = analogRead(VOLTAGE_DIVIDER_PIN); //Get Voltage Analog Value
  voltage_sensed_A1 = analogVolt_A1 * 5 / 1023.0; //Convert the Analog Value to a Continu Values
  batteryVoltage = voltage_sensed_A1 * ((R1+R2)/R2); //Calculate the true voltage value

  //Calculate Current Value
  float sampleValue = getMeanCurrent(); //Get mean current value
  float current_sensed_A0 = sampleValue * 5 / 1023.0; //Convert the Analog Value to a Continu Values
  batteryCurrent = ( ( current_sensed_A0 -  2.5 ) * ratio_Current ) / 0.185; //Calculate the true current value

  // Calculate watt
  batteryPower = batteryCurrent * batteryVoltage;

  // Calculate ampere-hours
  unsigned long currentMillis = millis(); // Get the current time
  unsigned long elapsedTime = currentMillis - previousMillis; // Calculate elapsed time in milliseconds

  // Convert elapsed time to hours and update ampere-hours
  ampereHours += (batteryCurrent * elapsedTime) / (1000.0 * 3600.0); // Convert milliseconds to hours

  // Calculate battery percentage
  percentageBattery = returnPercentage(ampereHours, percentageBattery);

  // Update previous time
  previousMillis = currentMillis;

  //print to delete
  printBatteryPercentage(percentageBattery);
  printBatteryVoltage(batteryVoltage);
  printBatteryCurrent(batteryCurrent);
  printBatteryAmpereHours(ampereHours);
  printBatteryPower(batteryPower);

  writeFieldThingSpeakChannel(percentageBattery, batteryVoltage, batteryCurrent, batteryPower);
}