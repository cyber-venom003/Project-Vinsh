#include <ESP8266WiFi.h>
 
#define SENSOR_PIN  2
 
long currentPulse = 0;
long previousPulse = 0;
int interval = 1000;

float calibrationConstant = 4.5;
volatile byte pulseCount;
byte pulse1Sec = 0;
float flowRate;
float flowLitres;
float totalLitres;
 
void IRAM_ATTR pulseCounter()
{
  pulseCount++;
}
 
WiFiClient client;
 
void setup()
{
  Serial.begin(115200);
  delay(10);

  pinMode(SENSOR_PIN, INPUT_PULLUP);
 
  pulseCount = 0;
  flowRate = 0.0;
  previousPulse = 0;
 
  attachInterrupt(digitalPinToInterrupt(SENSOR_PIN), pulseCounter, FALLING);
}
 
void loop()
{
  currentPulse = millis();
  if (currentPulse - previousPulse > interval) 
  {
    
    pulse1Sec = pulseCount;
    pulseCount = 0;
 
    // Because this loop may not complete in exactly 1 second intervals we calculate
    // the number of milliseconds that have passed since the last execution and use
    // that to scale the output. We also apply the calibrationConstant to scale the output
    // based on the number of pulses per second per units of measure (litres/minute in
    // this case) coming from the sensor.
    flowRate = ((1000.0 / (millis() - previousPulse)) * pulse1Sec) / calibrationConstant;
    previousPulse = millis();
 
    // Divide the flow rate in litres/minute by 60 to determine how many litres have
    // passed through the sensor in this 1 second interval, then multiply by 1000 to
    // convert to millilitres.
    flowLitres = (flowRate / 60);
 
    // Add the millilitres passed in this second to the cumulative total
    totalLitres += flowLitres;
    
    // Print the flow rate for this second in litres / minute
    Serial.print("Flow rate: ");
    Serial.print(float(flowRate));  // Print the integer part of the variable
    Serial.print("L/min");
    Serial.print("\t");       // Print tab space
 
    // Print the cumulative total of litres flowed since starting
    Serial.print("Output Liquid Quantity: ");
    Serial.print(totalLitres);
    Serial.println("L");
  }
}