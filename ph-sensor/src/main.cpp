#include <Arduino.h> // Include this for compatibility with C++ in Arduino

const int analogPhPin = A0; //PH module pin P0 connected to analog pin A0
long phTot, temTot;
float phAvg, temAvg;
int x;
float C = 21.04; //Constant of straight line (Y = mx + C)
float m = -5.67; // Slope of straight line (Y = mx + C)
 
void setup() {
 
// sensors.begin(); //Start the DS18B20 Library
  Serial.begin(9600);
}
void loop() {
  phTot = 0;
  temTot = 0;
  phAvg = 0;
  temAvg = 0;
  
  //taking 10 sample and adding with 10 milli second delay
  for(x=0; x<10 ; x++)
  {
  phTot += analogRead(A0);
  temTot += analogRead(A1);
  delay(10);
  }
  float temAvg = temTot/10;
  float phAvg = temTot/10;
  float temVoltage = temAvg * (5000.0 / 1023.0); //convert sensor reading into milli volt
  float phVoltage = phAvg * 5.0 / 1024.0 / 1.7; //convert sensor reading into milli volt
  

  
  // float Etemp = temVoltage*0.1; //convert milli volt to temperature degree Celsius
  float pHValue = phVoltage*m+C;
  
  Serial.print("phVoltage = ");
  Serial.print(phVoltage);
  Serial.print(" ");
  Serial.print("pH=");
  Serial.println(pHValue);
  
  delay(1000);
}